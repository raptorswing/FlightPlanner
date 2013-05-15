#ifndef MOUSEMETRICS_H
#define MOUSEMETRICS_H

#include <QtGlobal>
#include <QObject>

#include "PlanningCommon_global.h"

class PLANNINGCOMMONSHARED_EXPORT MouseMetrics : public QObject
{
    Q_OBJECT
public:
    void addPress();
    void addRelease();
    void addDoubleClick();

    quint64 presses() const;
    quint64 releases() const;
    quint64 doubleClicks() const;

    static MouseMetrics* instance();
    static void buildInstance();
    static void destroyInstance();

protected:
    bool eventFilter(QObject * obj, QEvent* event);

private:
    MouseMetrics();
    quint64 _press;
    quint64 _release;
    quint64 _doubleClick;

    static MouseMetrics * _instance;
};

#endif // MOUSEMETRICS_H
