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
{
    m_defaultPort = getDefaultPort();
    
    connect(m_serialPort, &QSerialPort::readyRead, this, &SerialCommunication::handleReadyRead);
    connect(m_serialPort, &QSerialPort::errorOccurred, this, &SerialCommunication::handleError);
    
    setupWatchdog();
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

void SerialCommunication::closePort()
{
    m_watchdogTimer.stop();
    
    if (m_serialPort->isOpen()) {
        m_serialPort->clear();
        m_serialPort->flush();
        m_serialPort->close();
        qDebug() << QDateTime::currentDateTime().toString()
                 << "- Closed port:" << m_serialPort->portName();
        emit portStatusChanged(false);
    }
}

bool SerialCommunication::sendCommand(const QByteArray &command)
{
    if (!m_serialPort->isOpen()) {
        logError("Cannot send command - port not open");
        return false;
    }

    qDebug() << "Sending command:" << command.toHex();

    // Clear any existing data
    m_serialPort->clear();
    m_responseBuffer.clear();

    // Write command with error checking
    qint64 bytesWritten = 0;
    int retries = 3;  // Add retries for write operation
    
    while (retries > 0 && bytesWritten != command.size()) {
        bytesWritten = m_serialPort->write(command);
        if (bytesWritten == -1) {
            logError("Write error: " + m_serialPort->errorString());
            retries--;
            QThread::msleep(10);  // Brief pause before retry
            continue;
        }
        break;
    }

    if (bytesWritten != command.size()) {
        logError(QString("Failed to write all data - wrote %1 of %2 bytes")
                .arg(bytesWritten).arg(command.size()));
        return false;
    }

    // Wait for command to be written
    if (!m_serialPort->waitForBytesWritten(COMMAND_TIMEOUT_MS)) {
        logError("Write timeout");
        return false;
    }

    // Brief pause to allow device processing
    QThread::msleep(20);

    // Wait for response with safer handling
    bool responseReceived = waitForResponse();
    if (!responseReceived) {
        logError("No response received");
        return false;
    }

    qDebug() << "Command completed successfully";
    return true;
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
    if (m_serialPort->isOpen()) {
        closePort();
    }

    // Extract just the COM port name from the description
    QString actualPortName = portName.split(" ").first();
    m_serialPort->setPortName(actualPortName);

    // Configure port settings
    m_serialPort->setBaudRate(config.baudRate);
    m_serialPort->setDataBits(config.dataBits);
    m_serialPort->setParity(config.parity);
    m_serialPort->setStopBits(config.stopBits);
    m_serialPort->setFlowControl(config.flowControl);

    // Debug output for troubleshooting
    qDebug() << "Attempting to open port:" << actualPortName;
    qDebug() << "Port settings:"
             << "Baud:" << m_serialPort->baudRate()
             << "Data:" << m_serialPort->dataBits()
             << "Parity:" << m_serialPort->parity()
             << "Stop:" << m_serialPort->stopBits()
             << "Flow:" << m_serialPort->flowControl();

    // Try to open the port
    if (!m_serialPort->open(QIODevice::ReadWrite)) {
        QString errorDetails = QString("Failed to open port %1: %2 (Error: %3)")
            .arg(actualPortName)
            .arg(m_serialPort->errorString())
            .arg(m_serialPort->error());
        logError(errorDetails);
        emit portStatusChanged(false);
        return false;
    }

    // Clear any existing data
    m_serialPort->clear();
    m_serialPort->flush();

    qDebug() << QDateTime::currentDateTime().toString()
             << "- Opened port:" << actualPortName
             << "at" << config.baudRate << "baud";

    emit portStatusChanged(true);
    m_watchdogTimer.start();
    return true;
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
        return;  // Safety check
    }
    
    QByteArray data = m_serialPort->readAll();
    if (!data.isEmpty()) {
        m_responseBuffer.append(data);
        
        qDebug() << QDateTime::currentDateTime().toString()
                 << "- Received data (hex):" << data.toHex()
                 << "ascii:" << data;
        
        emit dataReceived(data);
    }
}

void SerialCommunication::handleError(QSerialPort::SerialPortError error)
{
    if (error == QSerialPort::NoError) {
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