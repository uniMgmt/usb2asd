#ifndef KEYPRESSCOMMANDS_H
#define KEYPRESSCOMMANDS_H

#include <QObject>
#include "SerialCommunication.h"
#include "MockSerialCommunication.h"

class KeypressCommands : public QObject
{
    Q_OBJECT

public:
    explicit KeypressCommands(QObject *serialComm, QObject *parent = nullptr);

public slots:
    void sendKeypress1();
    void sendKeypress2();
    void sendKeypress3();
    void sendKeypress4();
    void sendKeypress5();
    void sendKeypress6();
    void sendKeypress7();
    void sendKeypress8();
    void sendKeypress9();
    void sendKeypress0();
    void sendKeypressStar();
    void sendKeypressHash();
    bool sendSetPriceCommand(int price);

private:
    SerialCommunication *m_serialComm;
    MockSerialCommunication *m_mockSerialComm;
    bool m_useMockSerial;
    void logAction(const QString &action);
    void errorLog(const QString &error);
    bool sendCommand(const QByteArray &command);
};

#endif // KEYPRESSCOMMANDS_H