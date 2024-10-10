#include "MockSerialCommunication.h"
#include <QDebug>

MockSerialCommunication::MockSerialCommunication(QObject *parent)
    : QObject(parent), m_isOpen(false), m_receiveTimer(new QTimer(this))
{
    m_defaultPort = getDefaultPort();
    connect(m_receiveTimer, &QTimer::timeout, this, &MockSerialCommunication::simulateDataReceived);
}

MockSerialCommunication::~MockSerialCommunication()
{
    closePort();
}

bool MockSerialCommunication::openPort(const QString &portName)
{
    qDebug() << "Mock: Opening port" << portName;
    m_isOpen = true;
    emit portStatusChanged(true);
    m_receiveTimer->start(5000); // Simulate receiving data every 5 seconds
    return true;
}

void MockSerialCommunication::closePort()
{
    if (m_isOpen) {
        qDebug() << "Mock: Closing port";
        m_isOpen = false;
        emit portStatusChanged(false);
        m_receiveTimer->stop();
    }
}

bool MockSerialCommunication::sendCommand(const QByteArray &command)
{
    if (m_isOpen) {
        qDebug() << "Mock: Sending command" << command.toHex();
        return true;
    }
    return false;
}

QStringList MockSerialCommunication::getAvailablePorts()
{
    return QStringList() << "MOCK_PORT1" << "MOCK_PORT2" << "MOCK_PORT3";
}

QString MockSerialCommunication::getDefaultPort()
{
    return "MOCK_PORT1";
}

void MockSerialCommunication::setDefaultPort(const QString &portName)
{
    m_defaultPort = portName;
}

bool MockSerialCommunication::isPortOpen() const
{
    return m_isOpen;
}

void MockSerialCommunication::simulateDataReceived()
{
    QByteArray mockData = QByteArray::fromHex("0A0102030405");
    emit dataReceived(mockData);
}