#include "MainWindow.h"
#include <QDebug>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QMenuBar>
#include <QMessageBox>
#include <QApplication>
#include "AutoKeypress.h"
#include "Colors.h"
#include "SetPriceDialog.h"
#include <QTextEdit>
#include <QSplitter>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),
      m_useMockSerial(false),
      m_serialComm(nullptr),
      m_mockSerialComm(nullptr),
      m_keypressCommands(nullptr),
      m_autoKeypress(nullptr),
      m_showKeepaliveLogs(false)
{
    if (m_useMockSerial) {
        m_mockSerialComm = new MockSerialCommunication(this);
        m_keypressCommands = new KeypressCommands(m_mockSerialComm, this);
    } else {
        m_serialComm = new SerialCommunication(this);
        m_keypressCommands = new KeypressCommands(m_serialComm, this);
    }

    setupUi();
    m_menuBar = new QMenuBar(this);
    setMenuBar(m_menuBar);
    setupMenuBar();
    setupAutoKeypress();
    connectSignalsAndSlots();
    qDebug() << "MainWindow constructed";
}

MainWindow::~MainWindow()
{
    qDebug() << "MainWindow destructed";
}

void MainWindow::setupUi()
{
    QString buttonStyle = QString(
        "QPushButton {"
        "    background-color: %1;"
        "    color: %2;"
        "    border: 2px solid %3;"
        "    border-radius: 5px;"
        "}"
        "QPushButton:pressed {"
        "    background-color: %3;"
        "}"
    ).arg(AppColors::Brown.name(), AppColors::Gold.name(), AppColors::Beige.name());

    setWindowTitle("asdKeypad C++ Port");
    
    // Create main splitter
    m_mainSplitter = new QSplitter(Qt::Horizontal, this);
    setCentralWidget(m_mainSplitter);

    // Create and setup console output widget
    m_consoleOutput = new QTextEdit(this);
    m_consoleOutput->setReadOnly(true);
    m_consoleOutput->setFont(QFont("Courier", 9));
    m_consoleOutput->setStyleSheet(QString(
        "QTextEdit {"
        "    background-color: %1;"
        "    color: %2;"
        "    border: 2px solid %3;"
        "}"
    ).arg(AppColors::Brown.name(), AppColors::Gold.name(), AppColors::Beige.name()));

    // Create widget for keypad interface
    QWidget *keypadWidget = new QWidget(this);
    QVBoxLayout *keypadLayout = new QVBoxLayout(keypadWidget);

    // Add widgets to splitter
    m_mainSplitter->addWidget(m_consoleOutput);
    m_mainSplitter->addWidget(keypadWidget);

    // Set initial sizes (adjust these values as needed)
    m_mainSplitter->setSizes(QList<int>() << 400 << 400);

    // Serial port controls
    QHBoxLayout *portLayout = new QHBoxLayout();
    m_portComboBox = new QComboBox(this);
    if (m_useMockSerial) {
        m_portComboBox->addItems(m_mockSerialComm->getAvailablePorts());
    } else {
        m_portComboBox->addItems(m_serialComm->getAvailablePorts());
    }
    m_connectButton = new QPushButton("Connect", this);
    m_portStatusLabel = new QLabel("Not Connected", this);
    portLayout->addWidget(m_portComboBox);
    portLayout->addWidget(m_connectButton);
    portLayout->addWidget(m_portStatusLabel);
    keypadLayout->addLayout(portLayout);

    // Display
    m_display = new QLineEdit(this);
    m_display->setReadOnly(true);
    m_display->setAlignment(Qt::AlignRight);
    keypadLayout->addWidget(m_display);

    // Keypad
    QGridLayout *numpadLayout = new QGridLayout();
    keypadLayout->addLayout(numpadLayout);

    const char* buttonLabels[12] = {"1", "2", "3", "4", "5", "6", "7", "8", "9", "*", "0", "#"};
    for (int i = 0; i < 12; ++i) {
        m_buttons[i] = new QPushButton(buttonLabels[i], this);
        connect(m_buttons[i], &QPushButton::clicked, this, &MainWindow::onDigitClicked);
        numpadLayout->addWidget(m_buttons[i], i / 3, i % 3);
        m_buttons[i]->setStyleSheet(buttonStyle);
    }

    // Clear and Enter buttons
    QHBoxLayout *controlLayout = new QHBoxLayout();
    m_clearButton = new QPushButton("Clear", this);
    m_enterButton = new QPushButton("Enter", this);
    controlLayout->addWidget(m_clearButton);
    controlLayout->addWidget(m_enterButton);
    keypadLayout->addLayout(controlLayout);
    m_clearButton->setStyleSheet(buttonStyle);
    m_enterButton->setStyleSheet(buttonStyle);

    // Add Auto Keypress button
    m_autoKeypressButton = new QPushButton("Start Auto Keypress", this);
    m_autoKeypressButton->setCheckable(true);
    keypadLayout->addWidget(m_autoKeypressButton);
    m_autoKeypressButton->setStyleSheet(buttonStyle);

    // Connect signals and slots
    connect(m_clearButton, &QPushButton::clicked, this, &MainWindow::onClearClicked);
    connect(m_enterButton, &QPushButton::clicked, this, &MainWindow::onEnterClicked);
    connect(m_connectButton, &QPushButton::clicked, this, &MainWindow::onConnectClicked);
    if (m_useMockSerial) {
        connect(m_mockSerialComm, &MockSerialCommunication::portStatusChanged, this, &MainWindow::onPortStatusChanged);
    } else {
        connect(m_serialComm, &SerialCommunication::portStatusChanged, this, &MainWindow::onPortStatusChanged);
    }

    // Style for display
    QString displayStyle = QString(
        "QLineEdit {"
        "    background-color: %1;"
        "    color: %2;"
        "    border: 2px solid %3;"
        "    border-radius: 5px;"
        "    padding: 5px;"
        "}"
    ).arg(AppColors::Brown.name(), AppColors::Gold.name(), AppColors::Beige.name());

    m_display->setStyleSheet(displayStyle);

    // Style for combo box
    QString comboBoxStyle = QString(
        "QComboBox {"
        "    background-color: %1;"
        "    color: %2;"
        "    border: 2px solid %3;"
        "    border-radius: 5px;"
        "    padding: 5px;"
        "}"
        "QComboBox::drop-down {"
        "    border: none;"
        "}"
    ).arg(AppColors::Brown.name(), AppColors::Gold.name(), AppColors::Beige.name());

    m_portComboBox->setStyleSheet(comboBoxStyle);

    // Install event filter to capture qDebug output
    qInstallMessageHandler(MainWindow::messageHandler);
}

void MainWindow::setupMenuBar()
{
    // Create menus
    QMenu *fileMenu = m_menuBar->addMenu(tr("&File"));
    QMenu *toolsMenu = m_menuBar->addMenu(tr("&Tools"));
    QMenu *helpMenu = m_menuBar->addMenu(tr("&Help"));

    // Add actions to File menu
    fileMenu->addAction(tr("E&xit"), this, &MainWindow::onExitClicked);

    // Add actions to Tools menu
    toolsMenu->addAction(tr("&Clear Log"), this, &MainWindow::onClearLogClicked);
    toolsMenu->addAction(tr("Change &Default Port"), this, &MainWindow::onChangeDefaultPortClicked);
    toolsMenu->addAction(tr("Clear VMC &Error"), this, &MainWindow::onClearVMCErrorClicked);
    toolsMenu->addAction(tr("Set VMC &Prices"), this, &MainWindow::onSetVMCPricesClicked);

    // Add keepalive mechanism toggle
    m_toggleKeepaliveAction = toolsMenu->addAction(tr("Enable &Keepalive"));
    m_toggleKeepaliveAction->setCheckable(true);
    m_toggleKeepaliveAction->setChecked(false);
    connect(m_toggleKeepaliveAction, &QAction::toggled, this, &MainWindow::onToggleKeepalive);

    // Add keepalive logs toggle
    m_showKeepaliveLogsAction = toolsMenu->addAction(tr("Show &Keepalive Logs"));
    m_showKeepaliveLogsAction->setCheckable(true);
    m_showKeepaliveLogsAction->setChecked(false);
    m_showKeepaliveLogsAction->setEnabled(false);  // Disabled by default
    connect(m_showKeepaliveLogsAction, &QAction::toggled, this, &MainWindow::onToggleKeepaliveLogs);

    // Add actions to Help menu
    helpMenu->addAction(tr("&About"), this, &MainWindow::onAboutClicked);

    // Set the menu bar style
    m_menuBar->setStyleSheet(QString(
        "QMenuBar {"
        "    background-color: %1;"
        "    color: %2;"
        "}"
        "QMenuBar::item:selected {"
        "    background-color: %3;"
        "}"
    ).arg(AppColors::Brown.name(), AppColors::Gold.name(), AppColors::Beige.name()));
}

void MainWindow::setupAutoKeypress()
{
    m_autoKeypress = new AutoKeypress(m_keypressCommands, this);
    
    // The button is now created in setupUi(), so we don't need to create it here
    // Just connect the signals and slots
    connect(m_autoKeypressButton, &QPushButton::toggled, this, &MainWindow::onAutoKeypressToggled);
    connect(m_autoKeypress, &AutoKeypress::sequenceCompleted, this, &MainWindow::onAutoKeypressCompleted);
    connect(m_autoKeypress, &AutoKeypress::keyPressed, this, &MainWindow::onAutoKeypressKeyPressed);
}

void MainWindow::onAutoKeypressToggled(bool checked)
{
    if (checked) {
        m_autoKeypress->startSequence();
        m_autoKeypressButton->setText("Stop Auto Keypress");
        logAction("Auto Keypress sequence started");
    } else {
        m_autoKeypress->stopSequence();
        m_autoKeypressButton->setText("Start Auto Keypress");
        logAction("Auto Keypress sequence stopped");
    }
}

void MainWindow::onAutoKeypressCompleted()
{
    m_autoKeypressButton->setChecked(false);
    m_autoKeypressButton->setText("Start Auto Keypress");
    logAction("Auto Keypress sequence completed");
}

void MainWindow::onAutoKeypressKeyPressed(const QString &key)
{
    // Update the display
    m_display->setText(m_display->text() + key);
    logAction("Auto Keypress: " + key);
}

void MainWindow::logAction(const QString &action)
{
    appendToConsole(QString("Action: %1").arg(action));
}

void MainWindow::errorLog(const QString &error)
{
    appendToConsole(QString("ERROR: %1").arg(error));
}

void MainWindow::onDigitClicked()
{
    QPushButton *button = qobject_cast<QPushButton*>(sender());
    if (button) {
        m_display->setText(m_display->text() + button->text());
        logAction("Digit clicked: " + button->text());
    }
}

void MainWindow::onClearClicked()
{
    m_display->clear();
    logAction("Display cleared");
}

void MainWindow::onEnterClicked()
{
    logAction("Enter clicked, value: " + m_display->text());
    // Here you would typically send the entered value to the serial port
    if (m_useMockSerial) {
        if (m_mockSerialComm->isPortOpen()) {
            QByteArray command = m_display->text().toUtf8();
            if (m_mockSerialComm->sendCommand(command)) {
                logAction("Command sent: " + m_display->text());
            } else {
                errorLog("Failed to send command");
            }
        } else {
            errorLog("Mock serial port is not open");
        }
    } else {
        if (m_serialComm->isPortOpen()) {
            QByteArray command = m_display->text().toUtf8();
            if (m_serialComm->sendCommand(command)) {
                logAction("Command sent: " + m_display->text());
            } else {
                errorLog("Failed to send command");
            }
        } else {
            errorLog("Serial port is not open");
        }
    }
    m_display->clear();
}

void MainWindow::onConnectClicked()
{
    try {
        QString portName = m_portComboBox->currentText();
        
        if (!m_useMockSerial) {
            if (m_serialComm->isPortOpen()) {
                m_serialComm->closePort();
            } else {
                // Check port availability first
                if (!m_serialComm->isPortAvailable(portName)) {
                    errorLog(QString("Port %1 is not available or is in use by another application").arg(portName));
                    return;
                }

                qDebug() << "Attempting to connect to port:" << portName;
                
                SerialCommunication::SerialConfig config;
                config.baudRate = QSerialPort::Baud9600;
                config.dataBits = QSerialPort::Data8;
                config.parity = QSerialPort::NoParity;
                config.stopBits = QSerialPort::OneStop;
                config.flowControl = QSerialPort::NoFlowControl;

                if (m_serialComm->openPort(portName, config)) {
                    logAction("Connected to port: " + portName);
                } else {
                    errorLog("Failed to connect to port: " + portName);
                }
            }
        }
        // ... rest of the mock serial handling ...
    } catch (const std::exception& e) {
        errorLog(QString("Exception during connect: %1").arg(e.what()));
    } catch (...) {
        errorLog("Unknown exception during connect");
    }
}

void MainWindow::onPortStatusChanged(bool isOpen)
{
    m_portStatusLabel->setText(isOpen ? "Connected" : "Not Connected");
    m_connectButton->setText(isOpen ? "Disconnect" : "Connect");
}

void MainWindow::onClearLogClicked()
{
    m_consoleOutput->clear();
    logAction("Log cleared");
}

void MainWindow::onChangeDefaultPortClicked()
{
    QString newDefaultPort = m_portComboBox->currentText();
    if (m_useMockSerial) {
        m_mockSerialComm->setDefaultPort(newDefaultPort);
    } else {
        m_serialComm->setDefaultPort(newDefaultPort);
    }
    logAction("Default port changed to: " + newDefaultPort);
}

void MainWindow::onExitClicked()
{
    QApplication::quit();
}

void MainWindow::onAboutClicked()
{
    QMessageBox::about(this, "About asdKeypad C++ Port", "This is a C++ port of the asdKeypad application.");
}

void MainWindow::onClearVMCErrorClicked()
{
    logAction("Clear VMC Error clicked");
    // Implement the clear VMC error functionality here
    // For now, we'll just send a placeholder command
    if (m_useMockSerial) {
        if (m_mockSerialComm->isPortOpen()) {
            QByteArray command = "CLEAR_VMC_ERROR";
            if (m_mockSerialComm->sendCommand(command)) {
                logAction("Clear VMC Error command sent");
            } else {
                errorLog("Failed to send Clear VMC Error command");
            }
        } else {
            errorLog("Mock serial port is not open");
        }
    } else {
        if (m_serialComm->isPortOpen()) {
            QByteArray command = "CLEAR_VMC_ERROR";
            if (m_serialComm->sendCommand(command)) {
                logAction("Clear VMC Error command sent");
            } else {
                errorLog("Failed to send Clear VMC Error command");
            }
        } else {
            errorLog("Serial port is not open");
        }
    }
}

void MainWindow::onSetVMCPricesClicked()
{
    showSetPriceDialog();
}

void MainWindow::showSetPriceDialog()
{
    SetPriceDialog dialog(this);
    if (dialog.exec() == QDialog::Accepted) {
        int price = dialog.getPrice();
        if (m_keypressCommands->sendSetPriceCommand(price)) {
            logAction(QString("Price set to $%1.%2").arg(price / 100).arg(price % 100, 2, 10, QChar('0')));
        } else {
            errorLog("Failed to set price");
        }
    }
}

void MainWindow::onAutoKeypressClicked()
{
    m_autoKeypressButton->toggle();
}

void MainWindow::connectSignalsAndSlots()
{
    // Connect button clicks to keypress commands
    connect(m_buttons[0], &QPushButton::clicked, m_keypressCommands, &KeypressCommands::sendKeypress1);
    connect(m_buttons[1], &QPushButton::clicked, m_keypressCommands, &KeypressCommands::sendKeypress2);
    connect(m_buttons[2], &QPushButton::clicked, m_keypressCommands, &KeypressCommands::sendKeypress3);
    connect(m_buttons[3], &QPushButton::clicked, m_keypressCommands, &KeypressCommands::sendKeypress4);
    connect(m_buttons[4], &QPushButton::clicked, m_keypressCommands, &KeypressCommands::sendKeypress5);
    connect(m_buttons[5], &QPushButton::clicked, m_keypressCommands, &KeypressCommands::sendKeypress6);
    connect(m_buttons[6], &QPushButton::clicked, m_keypressCommands, &KeypressCommands::sendKeypress7);
    connect(m_buttons[7], &QPushButton::clicked, m_keypressCommands, &KeypressCommands::sendKeypress8);
    connect(m_buttons[8], &QPushButton::clicked, m_keypressCommands, &KeypressCommands::sendKeypress9);
    connect(m_buttons[9], &QPushButton::clicked, m_keypressCommands, &KeypressCommands::sendKeypress0);
    connect(m_buttons[10], &QPushButton::clicked, m_keypressCommands, &KeypressCommands::sendKeypressStar);
    connect(m_buttons[11], &QPushButton::clicked, m_keypressCommands, &KeypressCommands::sendKeypressHash);

    // Connect serial port status changes
    if (m_useMockSerial) {
        connect(m_mockSerialComm, &MockSerialCommunication::portStatusChanged, this, &MainWindow::onPortStatusChanged);
    } else {
        connect(m_serialComm, &SerialCommunication::portStatusChanged, this, &MainWindow::onPortStatusChanged);
    }

    // Add these new connections
    if (!m_useMockSerial) {
        connect(m_serialComm, &SerialCommunication::keepaliveMessage, 
                this, &MainWindow::onKeepaliveMessage);
        connect(m_serialComm, &SerialCommunication::normalMessage, 
                this, &MainWindow::onNormalMessage);
    }
}

void MainWindow::onToggleKeepaliveLogs(bool show)
{
    m_showKeepaliveLogs = show;
}

void MainWindow::onKeepaliveMessage(const QString &message)
{
    if (m_showKeepaliveLogs) {
        logAction(message);
    }
}

void MainWindow::onNormalMessage(const QString &message)
{
    logAction(message);  // Always show normal messages
}

void MainWindow::onToggleKeepalive(bool enable)
{
    if (!m_useMockSerial) {
        m_serialComm->enableKeepalive(enable);
        m_showKeepaliveLogsAction->setEnabled(enable);  // Enable/disable logs option
        if (!enable) {
            m_showKeepaliveLogsAction->setChecked(false);  // Uncheck logs when disabling keepalive
        }
    }
}

void MainWindow::messageHandler(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
    QString txt;
    switch (type) {
    case QtDebugMsg:
        txt = QString("Debug: %1").arg(msg);
        break;
    case QtWarningMsg:
        txt = QString("Warning: %1").arg(msg);
        break;
    case QtCriticalMsg:
        txt = QString("Critical: %1").arg(msg);
        break;
    case QtFatalMsg:
        txt = QString("Fatal: %1").arg(msg);
        break;
    }

    // Use a safer way to post the message
    QMetaObject::invokeMethod(qApp, [txt]() {
        for (QWidget *widget : QApplication::topLevelWidgets()) {
            if (MainWindow *mainWindow = qobject_cast<MainWindow*>(widget)) {
                mainWindow->appendToConsole(txt);
                break;
            }
        }
    }, Qt::QueuedConnection);
}

void MainWindow::appendToConsole(const QString &text)
{
    m_consoleOutput->append(text);
}