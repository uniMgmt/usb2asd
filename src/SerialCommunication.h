#ifndef SERIALCOMMUNICATION_H
#define SERIALCOMMUNICATION_H

#include <QObject>
#include <QSerialPort>
#include <QStringList>
#include <QTimer>

class SerialCommunication : public QObject
{
    Q_OBJECT

public:
    struct SerialConfig {
        QSerialPort::BaudRate baudRate = QSerialPort::Baud9600;
        QSerialPort::DataBits dataBits = QSerialPort::Data8;
        QSerialPort::Parity parity = QSerialPort::NoParity;
        QSerialPort::StopBits stopBits = QSerialPort::OneStop;
        QSerialPort::FlowControl flowControl = QSerialPort::NoFlowControl;
    };

    explicit SerialCommunication(QObject *parent = nullptr);
    ~SerialCommunication();

    bool openPort(const QString &portName, const SerialConfig &config = SerialConfig());
    void closePort();
    bool sendCommand(const QByteArray &command);
    QStringList getAvailablePorts();
    QString getDefaultPort();
    void setDefaultPort(const QString &portName);
    bool isPortOpen() const;
    QString getLastError() const { return m_lastError; }
    QString getCurrentPortName() const;

signals:
    void portStatusChanged(bool isOpen);
    void dataReceived(const QByteArray &data);
    void error(const QString &errorMessage);

private slots:
    void handleReadyRead();
    void handleError(QSerialPort::SerialPortError error);

private:
    QSerialPort *m_serialPort;
    QString m_defaultPort;
    QString m_lastError;
    QTimer m_watchdogTimer;
    
    static const int SERIAL_TIMEOUT_MS = 1000;
    static const int WATCHDOG_TIMEOUT_MS = 5000;

    void setupWatchdog();
    void logError(const QString &error);
};

#endif // SERIALCOMMUNICATION_H