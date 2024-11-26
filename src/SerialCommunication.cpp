#include "SerialCommunication.h"
#include <QSerialPortInfo>
#include <QSettings>
#include <QDebug>
#include <QDateTime>
#include <QElapsedTimer>
#include <QCoreApplication>
#include <QThread>
#include <numeric>
#include <algorithm>

SerialCommunication::SerialCommunication(QObject *parent)
    : QObject(parent)
    , m_serialPort(new QSerialPort(this))
    , m_keepaliveEnabled(false)
{
    m_defaultPort = getDefaultPort();
    
    connect(m_serialPort, &QSerialPort::readyRead, this, &SerialCommunication::handleReadyRead);
    connect(m_serialPort, &QSerialPort::errorOccurred, this, &SerialCommunication::handleError);
    
    setupWatchdog();
    setupKeepalive();
}

SerialCommunication::~SerialCommunication()
{
    closePort();
    delete m_serialPort;
}

void SerialCommunication::setupWatchdog()
{
    m_watchdogTimer.setInterval(WATCHDOG_TIMEOUT_MS);
    connect(&m_watchdogTimer, &QTimer::timeout, [this]() {
        if (m_serialPort->isOpen()) {
            // Simple status check instead of sending command
            if (!m_serialPort->isWritable() || !m_serialPort->isReadable()) {
                qDebug() << "Watchdog: Port no longer accessible";
                closePort();
                emit error("Connection lost - port not accessible");
            }
        }
    });
}

void SerialCommunication::setupKeepalive()
{
    m_keepaliveTimer.setInterval(KEEPALIVE_INTERVAL_MS);
    connect(&m_keepaliveTimer, &QTimer::timeout, this, &SerialCommunication::sendKeepalive);
}

void SerialCommunication::sendKeepalive()
{
    if (m_serialPort->isOpen()) {
        QByteArray keepalive("00");
        m_serialPort->write(keepalive);
        emit keepaliveMessage(QString("%1 - Sent keepalive")
            .arg(QDateTime::currentDateTime().toString()));
    }
}

void SerialCommunication::closePort()
{
    m_watchdogTimer.stop();
    m_keepaliveTimer.stop();
    
    if (m_serialPort->isOpen()) {
        try {
            // Ensure all data is written before closing
            m_serialPort->flush();
            
            // Wait for any pending operations
            QThread::msleep(100);
            
            // Clear buffers
            m_serialPort->clear();
            
            // Reset control lines
            m_serialPort->setDataTerminalReady(false);
            m_serialPort->setRequestToSend(false);
            
            // Close the port
            m_serialPort->close();
            
            qDebug() << QDateTime::currentDateTime().toString()
                     << "- Closed port:" << m_serialPort->portName();
                     
            // Clear any remaining data in our buffer
            m_responseBuffer.clear();
            
        } catch (const std::exception& e) {
            qDebug() << "Exception during port closure:" << e.what();
        } catch (...) {
            qDebug() << "Unknown exception during port closure";
        }
    }
    
    emit portStatusChanged(false);
}

bool SerialCommunication::sendCommand(const QByteArray &command)
{
    if (!m_serialPort->isOpen()) {
        logError("Cannot send command - port not open");
        return false;
    }

    // Store the command type
    m_lastCommand = command;

    // If this is a keepalive command, emit through keepaliveMessage
    if (command == "00") {
        emit keepaliveMessage(QString("%1 - Sending command: %2")
            .arg(QDateTime::currentDateTime().toString())
            .arg(QString(command.toHex())));
    } else {
        emit normalMessage(QString("%1 - Sending command: %2")
            .arg(QDateTime::currentDateTime().toString())
            .arg(QString(command.toHex())));
    }

    qint64 bytesWritten = m_serialPort->write(command);
    return bytesWritten == command.size();
}

bool SerialCommunication::waitForResponse(int timeout)
{
    if (!m_serialPort->isOpen()) {
        logError("Cannot wait for response - port not open");
        return false;
    }

    QElapsedTimer timer;
    timer.start();
    
    qDebug() << "Waiting for response, timeout:" << timeout << "ms";

    while (timer.elapsed() < timeout) {
        // Check if port is still valid
        if (!m_serialPort->isOpen() || !m_serialPort->isReadable()) {
            logError("Port became inaccessible while waiting for response");
            return false;
        }

        // Wait for data with shorter timeout
        if (m_serialPort->waitForReadyRead(10)) {
            QByteArray newData = m_serialPort->readAll();
            if (!newData.isEmpty()) {
                m_responseBuffer.append(newData);
                qDebug() << "Response received in" << timer.elapsed() 
                         << "ms, data:" << newData.toHex();
                return true;
            }
        }
        
        // Process events less frequently
        if (timer.elapsed() % 50 == 0) {  // Reduced frequency of event processing
            QCoreApplication::processEvents(QEventLoop::ExcludeUserInputEvents);
        }
    }

    qDebug() << "Response timeout after" << timer.elapsed() << "ms";
    return false;
}

QStringList SerialCommunication::getAvailablePorts()
{
    QStringList ports;
    const auto serialPortInfos = QSerialPortInfo::availablePorts();
    
    for (const QSerialPortInfo &portInfo : serialPortInfos) {
        // Store just the essential info in a cleaner format
        QString portDescription;
        if (portInfo.manufacturer().isEmpty() && portInfo.description().isEmpty()) {
            portDescription = portInfo.portName();
        } else {
            portDescription = QString("%1 (%2)")
                .arg(portInfo.portName())
                .arg(portInfo.description());
        }
        
        ports << portDescription;
        
        // Keep debug info for troubleshooting
        qDebug() << "Found port:" << portInfo.portName();
        qDebug() << "  Manufacturer:" << portInfo.manufacturer();
        qDebug() << "  Description:" << portInfo.description();
        qDebug() << "  Serial Number:" << portInfo.serialNumber();
        qDebug() << "  System Location:" << portInfo.systemLocation();
    }
    
    return ports;
}

bool SerialCommunication::openPort(const QString &portName, const SerialConfig &config)
{
    // Ensure we're not already trying to open the port
    static bool isOpening = false;
    if (isOpening) return false;
    isOpening = true;

    try {
        if (m_serialPort->isOpen()) {
            closePort();
        }

        // Force close any existing connections to this port
        QString actualPortName = portName.split(" ").first();
        for (const auto& info : QSerialPortInfo::availablePorts()) {
            if (info.portName() == actualPortName && info.isValid()) {
                QSerialPort tempPort(info);
                if (tempPort.isOpen()) {
                    tempPort.close();
                }
            }
        }

        // Add a delay after closing
        QThread::msleep(250);

        m_serialPort->setPortName(actualPortName);
        m_serialPort->setBaudRate(config.baudRate);
        m_serialPort->setDataBits(config.dataBits);
        m_serialPort->setParity(config.parity);
        m_serialPort->setStopBits(config.stopBits);
        m_serialPort->setFlowControl(config.flowControl);

        // Set additional port parameters for Windows
        #ifdef Q_OS_WIN
            m_serialPort->setReadBufferSize(1024);
            m_serialPort->setDataTerminalReady(true);
            m_serialPort->setRequestToSend(true);
        #endif

        // Attempt to open with timeout
        QElapsedTimer timer;
        timer.start();
        const int openTimeout = 2000; // 2 second timeout

        while (timer.elapsed() < openTimeout) {
            if (m_serialPort->open(QIODevice::ReadWrite)) {
                m_serialPort->clear();
                m_serialPort->flush();
                
                emit normalMessage(QString("Successfully opened port %1").arg(actualPortName));
                emit portStatusChanged(true);
                
                if (m_keepaliveEnabled) {
                    m_keepaliveTimer.start();
                }
                m_watchdogTimer.start();
                
                isOpening = false;
                return true;
            }
            
            QThread::msleep(100);
            QCoreApplication::processEvents(QEventLoop::ExcludeUserInputEvents);
        }

        QString errorDetails = QString("Failed to open port %1: %2 (Error: %3)")
            .arg(actualPortName)
            .arg(m_serialPort->errorString())
            .arg(m_serialPort->error());
        logError(errorDetails);
        
    } catch (const std::exception& e) {
        logError(QString("Exception while opening port: %1").arg(e.what()));
    } catch (...) {
        logError("Unknown exception while opening port");
    }

    isOpening = false;
    emit portStatusChanged(false);
    return false;
}

QString SerialCommunication::getDefaultPort()
{
    QSettings settings("YourCompany", "asdKeypad");
    return settings.value("DefaultPort", "").toString();
}

void SerialCommunication::setDefaultPort(const QString &portName)
{
    QSettings settings("YourCompany", "asdKeypad");
    settings.setValue("DefaultPort", portName);
    m_defaultPort = portName;
}

bool SerialCommunication::isPortOpen() const
{
    return m_serialPort->isOpen();
}

QString SerialCommunication::getCurrentPortName() const
{
    return m_serialPort->portName();
}

void SerialCommunication::handleReadyRead()
{
    if (!m_serialPort->isOpen()) {
        return;
    }
    
    QByteArray data = m_serialPort->readAll();
    if (!data.isEmpty()) {
        m_responseBuffer.append(data);
        
        QString message = QString("%1 - Received data (hex): %2 ascii: %3")
            .arg(QDateTime::currentDateTime().toString())
            .arg(QString(data.toHex()))
            .arg(QString(data));

        // Check if this is a keepalive response (0B0FFA)
        bool isKeepaliveResponse = data.contains("0B0FFA") || 
                                 data.toHex().contains("304230464641");

        if (isKeepaliveResponse) {
            emit keepaliveMessage(message);
        } else {
            emit normalMessage(message);
        }
        
        emit dataReceived(data);
    }
}

void SerialCommunication::handleError(QSerialPort::SerialPortError error)
{
    if (error == QSerialPort::NoError || 
        error == QSerialPort::TimeoutError) {  // Ignore timeout errors
        return;
    }

    QString errorString = QString("Serial port error: %1 - %2")
                         .arg(error)
                         .arg(m_serialPort->errorString());
    
    logError(errorString);
    
    if (error != QSerialPort::NotOpenError) {
        closePort();
    }
}

void SerialCommunication::logError(const QString &error)
{
    m_lastError = error;
    qDebug() << QDateTime::currentDateTime().toString()
             << "- ERROR:" << error;
    emit this->error(error);
}

#ifdef QT_DEBUG
bool SerialCommunication::testResponseTimes()
{
    QElapsedTimer totalTimer;
    totalTimer.start();
    
    const int TEST_ITERATIONS = 10;
    int successCount = 0;
    QVector<qint64> responseTimes;

    for (int i = 0; i < TEST_ITERATIONS; i++) {
        QElapsedTimer commandTimer;
        commandTimer.start();
        
        // Send a simple command
        if (sendCommand(QByteArray::fromHex("0B0003C8D6"))) {
            responseTimes.append(commandTimer.elapsed());
            successCount++;
        }
        
        QThread::msleep(100); // Brief pause between tests
    }

    // Calculate statistics
    if (!responseTimes.isEmpty()) {
        qint64 avgTime = std::accumulate(responseTimes.begin(), responseTimes.end(), 0LL) / responseTimes.size();
        qint64 maxTime = *std::max_element(responseTimes.begin(), responseTimes.end());
        qint64 minTime = *std::min_element(responseTimes.begin(), responseTimes.end());
        
        qDebug() << "Response Time Statistics:";
        qDebug() << "  Average:" << avgTime << "ms";
        qDebug() << "  Maximum:" << maxTime << "ms";
        qDebug() << "  Minimum:" << minTime << "ms";
        qDebug() << "  Success Rate:" << (successCount * 100 / TEST_ITERATIONS) << "%";
    }

    return successCount > 0;
}
#endif

void SerialCommunication::enableKeepalive(bool enable)
{
    m_keepaliveEnabled = enable;
    if (enable && m_serialPort->isOpen()) {
        m_keepaliveTimer.start();
    } else {
        m_keepaliveTimer.stop();
    }
}

bool SerialCommunication::isKeepaliveEnabled() const
{
    return m_keepaliveEnabled;
}

bool SerialCommunication::isPortAvailable(const QString &portName) const
{
    QString actualPortName = portName.split(" ").first();
    for (const QSerialPortInfo &info : QSerialPortInfo::availablePorts()) {
        if (info.portName() == actualPortName) {
            return checkPortAccess(actualPortName);
        }
    }
    return false;
}

bool SerialCommunication::checkPortAccess(const QString &portName) const
{
    QSerialPort testPort;
    testPort.setPortName(portName);
    
    if (testPort.open(QIODevice::ReadWrite)) {
        testPort.close();
        return true;
    }
    return false;
}