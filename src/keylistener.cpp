#include "keylistener.h"

#include <QEvent>
#include <QKeyEvent>
#include <QLoggingCategory>

namespace {
Q_LOGGING_CATEGORY(self, "keylistener");
}

KeyListener::KeyListener(QObject *parent)
    : QObject{parent}
{
    
}

bool KeyListener::eventFilter(QObject *obj, QEvent *event)
{
    if (event->type() == QEvent::KeyPress) {
        QKeyEvent *keyEvent = static_cast<QKeyEvent *>(event);
        Qt::Key key = static_cast<Qt::Key>(keyEvent->key());
        qCDebug(self) << "key event:" << key << static_cast<QEvent::Type>(keyEvent->type());

        QTime now = QTime::currentTime();
        if (!_lastSignalEmit.contains(key) || _lastSignalEmit[key].addMSecs(50) < now) {
            _lastSignalEmit[key] = now;
            emit keyPressed(key);
        }
    }
    // standard event processing
    return QObject::eventFilter(obj, event);
}
