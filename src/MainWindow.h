#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPushButton>
#include <QLineEdit>
#include <QComboBox>
#include <QLabel>
#include <QListWidget>
#include "SerialCommunication.h"
#include "KeypressCommands.h"
#include "MockSerialCommunication.h"
#include "AutoKeypress.h"
#include <QMenuBar>
#include <QTextEdit>
#include <QSplitter>

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void onDigitClicked();
    void onClearClicked();
    void onEnterClicked();
    void onConnectClicked();
    void onPortStatusChanged(bool isOpen);
    void onClearLogClicked();
    void onChangeDefaultPortClicked();
    void onExitClicked();
    void onAboutClicked();
    void onClearVMCErrorClicked();
    void onSetVMCPricesClicked();
    void onAutoKeypressClicked();
    void onToggleKeepaliveLogs(bool show);
    void onKeepaliveMessage(const QString &message);
    void onNormalMessage(const QString &message);
    void onToggleKeepalive(bool enable);

private:
    void setupUi();
    void setupMenuBar();
    void logAction(const QString &action);
    void errorLog(const QString &error);
    void connectSignalsAndSlots();
    Q_INVOKABLE void appendToConsole(const QString &text);

    QLineEdit *m_display;
    QPushButton *m_buttons[12];
    QPushButton *m_clearButton;
    QPushButton *m_enterButton;
    QPushButton *m_connectButton;
    QComboBox *m_portComboBox;
    QLabel *m_portStatusLabel;
    SerialCommunication *m_serialComm;
    MockSerialCommunication *m_mockSerialComm;
    KeypressCommands *m_keypressCommands;
    bool m_useMockSerial;
    AutoKeypress *m_autoKeypress;
    QPushButton *m_autoKeypressButton;

    void setupAutoKeypress();
    void onAutoKeypressToggled(bool checked);
    void onAutoKeypressCompleted();
    void onAutoKeypressKeyPressed(const QString &key);

    QMenuBar *m_menuBar;

    void showSetPriceDialog();

    bool m_showKeepaliveLogs;
    QAction* m_toggleKeepaliveAction;
    QAction* m_showKeepaliveLogsAction;

    QTextEdit *m_consoleOutput;
    QSplitter *m_mainSplitter;

    static void messageHandler(QtMsgType type, const QMessageLogContext &context, const QString &msg);
};

#endif // MAINWINDOW_H