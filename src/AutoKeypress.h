#ifndef AUTOKEYPRESS_H
#define AUTOKEYPRESS_H

#include <QObject>
#include <QTimer>
#include <QVector>
#include "KeypressCommands.h"

class AutoKeypress : public QObject
{
    Q_OBJECT

public:
    explicit AutoKeypress(KeypressCommands *keypressCommands, QObject *parent = nullptr);

    void startSequence();
    void stopSequence();
    bool isRunning() const;

signals:
    void sequenceCompleted();
    void keyPressed(const QString &key);

private slots:
    void pressNextKey();

private:
    KeypressCommands *m_keypressCommands;
    QTimer m_timer;
    QVector<QString> m_sequence;
    int m_currentIndex;
    bool m_isRunning;

    void initializeSequence();
};

#endif // AUTOKEYPRESS_H