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
    void enableKeepalive(bool enable);
    bool isKeepaliveEnabled() const;

signals:
    void portStatusChanged(bool isOpen);
    void dataReceived(const QByteArray &data);
    void error(const QString &errorMessage);
    void keepaliveMessage(const QString &message);
    void normalMessage(const QString &message);

private slots:
    void handleReadyRead();
    void handleError(QSerialPort::SerialPortError error);

private:
    QSerialPort *m_serialPort;
    QString m_defaultPort;
    QString m_lastError;
    QTimer m_watchdogTimer;
    QTimer m_keepaliveTimer;
    bool m_keepaliveEnabled;
    static const int COMMAND_TIMEOUT_MS = 100;     // Time to wait for command to be written
    static const int RESPONSE_TIMEOUT_MS = 250;    // Time to wait for response
    static const int WATCHDOG_TIMEOUT_MS = 1000;   // Watchdog interval
    static const int KEEPALIVE_INTERVAL_MS = 5000;  // 5 seconds
    QByteArray m_responseBuffer;
    bool waitForResponse(int timeout = RESPONSE_TIMEOUT_MS);

    void setupWatchdog();
    void logError(const QString &error);
    void setupKeepalive();
    void sendKeepalive();

#ifdef QT_DEBUG
    bool testResponseTimes();
#endif

    QByteArray m_lastCommand;  // Add this to track command type
};

#endif // SERIALCOMMUNICATION_H