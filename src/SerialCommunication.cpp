#include "SerialCommunication.h"
#include <QSerialPortInfo>
#include <QSettings>
#include <QDebug>

SerialCommunication::SerialCommunication(QObject *parent)
    : QObject(parent), m_serialPort(new QSerialPort(this))
{
    m_defaultPort = getDefaultPort();
    connect(m_serialPort, &QSerialPort::readyRead, this, &SerialCommunication::handleReadyRead);
}

SerialCommunication::~SerialCommunication()
{
    closePort();
}

bool SerialCommunication::openPort(const QString &portName)
{
    closePort();  // Close any open port first

    m_serialPort->setPortName(portName);
    m_serialPort->setBaudRate(QSerialPort::Baud9600);
    m_serialPort->setDataBits(QSerialPort::Data8);
    m_serialPort->setParity(QSerialPort::NoParity);
    m_serialPort->setStopBits(QSerialPort::OneStop);
    m_serialPort->setFlowControl(QSerialPort::NoFlowControl);

    if (m_serialPort->open(QIODevice::ReadWrite)) {
        qDebug() << "Serial port opened successfully";
        emit portStatusChanged(true);
        return true;
    } else {
        qDebug() << "Failed to open serial port:" << m_serialPort->errorString();
        emit portStatusChanged(false);
        return false;
    }
}

void SerialCommunication::closePort()
{
    if (m_serialPort->isOpen()) {
        m_serialPort->close();
        qDebug() << "Serial port closed";
        emit portStatusChanged(false);
    }
}

bool SerialCommunication::sendCommand(const QByteArray &command)
{
    if (m_serialPort->isOpen()) {
        qint64 bytesWritten = m_serialPort->write(command);
        return bytesWritten == command.size();
    }
    return false;
}

QStringList SerialCommunication::getAvailablePorts()
{
    QStringList ports;
    const auto serialPortInfos = QSerialPortInfo::availablePorts();
    for (const QSerialPortInfo &portInfo : serialPortInfos) {
        ports << portInfo.portName();
    }
    return ports;
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

void SerialCommunication::handleReadyRead()
{
    QByteArray data = m_serialPort->readAll();
    emit dataReceived(data);
}