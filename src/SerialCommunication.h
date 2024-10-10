#ifndef SERIALCOMMUNICATION_H
#define SERIALCOMMUNICATION_H

#include <QObject>
#include <QSerialPort>
#include <QStringList>

class SerialCommunication : public QObject
{
    Q_OBJECT

public:
    explicit SerialCommunication(QObject *parent = nullptr);
    ~SerialCommunication();

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

private slots:
    void handleReadyRead();

private:
    QSerialPort *m_serialPort;
    QString m_defaultPort;
};

#endif // SERIALCOMMUNICATION_H