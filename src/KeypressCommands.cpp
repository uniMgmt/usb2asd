#include "KeypressCommands.h"
#include <QDebug>

KeypressCommands::KeypressCommands(QObject *serialComm, QObject *parent)
    : QObject(parent), m_serialComm(nullptr), m_mockSerialComm(nullptr)
{
    m_useMockSerial = qobject_cast<MockSerialCommunication*>(serialComm) != nullptr;
    if (m_useMockSerial) {
        m_mockSerialComm = qobject_cast<MockSerialCommunication*>(serialComm);
    } else {
        m_serialComm = qobject_cast<SerialCommunication*>(serialComm);
    }
}

bool KeypressCommands::sendCommand(const QByteArray &command)
{
    if (m_useMockSerial) {
        return m_mockSerialComm->sendCommand(command);
    } else {
        return m_serialComm->sendCommand(command);
    }
}

void KeypressCommands::sendKeypress1()
{
    QByteArray command = QByteArray::fromHex("0B0003C8D6");
    if (sendCommand(command)) {
        logAction("Simulate Key Press 1");
    } else {
        errorLog("Failed to send Key Press 1");
    }
}

void KeypressCommands::sendKeypress2()
{
    QByteArray command = QByteArray::fromHex("0B00036467");
    if (sendCommand(command)) {
        logAction("Simulate Key Press 2");
    } else {
        errorLog("Failed to send Key Press 2");
    }
}

void KeypressCommands::sendKeypress3()
{
    QByteArray command = QByteArray::fromHex("0B0102505E");
    if (sendCommand(command)) {
        logAction("Simulate Key Press 3");
    } else {
        errorLog("Failed to send Key Press 3");
    }
}

void KeypressCommands::sendKeypress4()
{
    QByteArray command = QByteArray::fromHex("0B0103505F");
    if (sendCommand(command)) {
        logAction("Simulate Key Press 4");
    } else {
        errorLog("Failed to send Key Press 4");
    }
}

void KeypressCommands::sendKeypress5()
{
    QByteArray command = QByteArray::fromHex("0B0202505F");
    if (sendCommand(command)) {
        logAction("Simulate Key Press 5");
    } else {
        errorLog("Failed to send Key Press 5");
    }
}

void KeypressCommands::sendKeypress6()
{
    QByteArray command = QByteArray::fromHex("0B02035060");
    if (sendCommand(command)) {
        logAction("Simulate Key Press 6");
    } else {
        errorLog("Failed to send Key Press 6");
    }
}

void KeypressCommands::sendKeypress7()
{
    QByteArray command = QByteArray::fromHex("0B03025060");
    if (sendCommand(command)) {
        logAction("Simulate Key Press 7");
    } else {
        errorLog("Failed to send Key Press 7");
    }
}

void KeypressCommands::sendKeypress8()
{
    QByteArray command = QByteArray::fromHex("0B03035061");
    if (sendCommand(command)) {
        logAction("Simulate Key Press 8");
    } else {
        errorLog("Failed to send Key Press 8");
    }
}

void KeypressCommands::sendKeypress9()
{
    QByteArray command = QByteArray::fromHex("0B04025061");
    if (sendCommand(command)) {
        logAction("Simulate Key Press 9");
    } else {
        errorLog("Failed to send Key Press 9");
    }
}

void KeypressCommands::sendKeypress0()
{
    QByteArray command = QByteArray::fromHex("0B04035062");
    if (sendCommand(command)) {
        logAction("Simulate Key Press 0");
    } else {
        errorLog("Failed to send Key Press 0");
    }
}

void KeypressCommands::sendKeypressStar()
{
    QByteArray command = QByteArray::fromHex("0B0400505F");
    if (sendCommand(command)) {
        logAction("Simulate Key Press *");
    } else {
        errorLog("Failed to send Key Press *");
    }
}

void KeypressCommands::sendKeypressHash()
{
    QByteArray command = QByteArray::fromHex("0B04015060");
    if (sendCommand(command)) {
        logAction("Simulate Key Press #");
    } else {
        errorLog("Failed to send Key Press #");
    }
}

void KeypressCommands::logAction(const QString &action)
{
    qDebug() << "Action:" << action;
    // You can emit a signal here to update the UI log
}

void KeypressCommands::errorLog(const QString &error)
{
    qDebug() << "Error:" << error;
    // You can emit a signal here to update the UI error log
}