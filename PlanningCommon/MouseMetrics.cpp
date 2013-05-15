#include "MouseMetrics.h"

#include <QEvent>
#include <QtDebug>

MouseMetrics * MouseMetrics::_instance = 0;

//private
MouseMetrics::MouseMetrics()
{
    _press = 0;
    _release = 0;
    _doubleClick = 0;
}

void MouseMetrics::addPress()
{
    ++_press;
}

void MouseMetrics::addRelease()
{
    qDebug() << ++_release;
}

void MouseMetrics::addDoubleClick()
{
    ++_doubleClick;
}


quint64 MouseMetrics::presses() const
{
    return _press;
}

quint64 MouseMetrics::releases() const
{
    return _release;
}

quint64 MouseMetrics::doubleClicks() const
{
    return _doubleClick;
}

//static
MouseMetrics *MouseMetrics::instance()
{
    return MouseMetrics::_instance;
}

//static
void MouseMetrics::buildInstance()
{
    if (MouseMetrics::instance() != 0)
        MouseMetrics::destroyInstance();

    MouseMetrics::_instance = new MouseMetrics();
}

//static
void MouseMetrics::destroyInstance()
{
    if (MouseMetrics::instance() != 0)
    {
        delete MouseMetrics::_instance;
        MouseMetrics::_instance = 0;
    }
}

//protcted
bool MouseMetrics::eventFilter(QObject *obj, QEvent *event)
{
    Q_UNUSED(obj);

    if (event->type() == QEvent::MouseButtonDblClick)
    {
        MouseMetrics::addDoubleClick();
    }
    else if (event->type() == QEvent::MouseButtonPress)
    {
        MouseMetrics::addPress();
    }
    else if (event->type() == QEvent::MouseButtonRelease)
    {
        MouseMetrics::addRelease();
    }
    else if (event->type() == QEvent::ChildAdded)
    {
        QChildEvent * childEvent = (QChildEvent*) event;
        childEvent->child()->installEventFilter(this);
    }

    return false;
}
