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
        SerialConfig() {
            // Default configuration that works with most hardware
            baudRate = QSerialPort::Baud9600;
            dataBits = QSerialPort::Data8;
            parity = QSerialPort::NoParity;
            stopBits = QSerialPort::OneStop;
            flowControl = QSerialPort::NoFlowControl;
        }

        QSerialPort::BaudRate baudRate;
        QSerialPort::DataBits dataBits;
        QSerialPort::Parity parity;
        QSerialPort::StopBits stopBits;
        QSerialPort::FlowControl flowControl;
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
    
    // Timeout constants
    static const int COMMAND_TIMEOUT_MS = 50;      // Time to wait for command to be written
    static const int RESPONSE_TIMEOUT_MS = 100;    // Time to wait for response
    static const int WATCHDOG_TIMEOUT_MS = 1000;   // Watchdog interval
    
    QByteArray m_responseBuffer;
    bool waitForResponse(int timeout = RESPONSE_TIMEOUT_MS);

    void setupWatchdog();
    void logError(const QString &error);

#ifdef QT_DEBUG
    bool testResponseTimes();
#endif
};

#endif // SERIALCOMMUNICATION_H