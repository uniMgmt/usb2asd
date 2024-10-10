#include "AutoKeypress.h"

AutoKeypress::AutoKeypress(KeypressCommands *keypressCommands, QObject *parent)
    : QObject(parent), m_keypressCommands(keypressCommands), m_currentIndex(0), m_isRunning(false)
{
    connect(&m_timer, &QTimer::timeout, this, &AutoKeypress::pressNextKey);
    initializeSequence();
}

void AutoKeypress::startSequence()
{
    if (!m_isRunning) {
        m_currentIndex = 0;
        m_isRunning = true;
        m_timer.start(1000); // 1 second delay between keypresses
        pressNextKey();
    }
}

void AutoKeypress::stopSequence()
{
    m_isRunning = false;
    m_timer.stop();
}

bool AutoKeypress::isRunning() const
{
    return m_isRunning;
}

void AutoKeypress::pressNextKey()
{
    if (m_currentIndex < m_sequence.size()) {
        QString key = m_sequence[m_currentIndex];
        emit keyPressed(key);

        if (key == "1") m_keypressCommands->sendKeypress1();
        else if (key == "2") m_keypressCommands->sendKeypress2();
        else if (key == "3") m_keypressCommands->sendKeypress3();
        else if (key == "4") m_keypressCommands->sendKeypress4();
        else if (key == "5") m_keypressCommands->sendKeypress5();
        else if (key == "6") m_keypressCommands->sendKeypress6();
        else if (key == "7") m_keypressCommands->sendKeypress7();
        else if (key == "8") m_keypressCommands->sendKeypress8();
        else if (key == "9") m_keypressCommands->sendKeypress9();
        else if (key == "0") m_keypressCommands->sendKeypress0();
        else if (key == "*") m_keypressCommands->sendKeypressStar();
        else if (key == "#") m_keypressCommands->sendKeypressHash();

        m_currentIndex++;
    } else {
        stopSequence();
        emit sequenceCompleted();
    }
}

void AutoKeypress::initializeSequence()
{
    // Define your sequence here
    m_sequence = {"1", "2", "3", "4", "5", "*", "0", "#"};
}