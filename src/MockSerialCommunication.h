#ifndef MOCKSERIALCOMMUNICATION_H
#define MOCKSERIALCOMMUNICATION_H

#include <QObject>
#include <QStringList>
#include <QTimer>

class MockSerialCommunication : public QObject
{
    Q_OBJECT

public:
    explicit MockSerialCommunication(QObject *parent = nullptr);
    ~MockSerialCommunication();

    bool openPort(const QString &portName);
    void closePort();
    bool sendCommand(const QByteArray &command);
    QStringList getAvailablePorts();
    QString getDefaultPort();
    void setDefaultPort(const QString &portName);
    bool isPortOpen() const;

signals:
    void portStatusChanged(bool isOpen);
    void dataReceived(const QByteArray &data);

private:
    bool m_isOpen;
    QString m_defaultPort;
    QTimer *m_receiveTimer;

    void simulateDataReceived();
};

#endif // MOCKSERIALCOMMUNICATION_H