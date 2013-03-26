#include "CommonWindowHandling.h"

#include <QSettings>
#include <QMainWindow>
#include <QDesktopWidget>
#include <QApplication>
#include <QtDebug>

const QString SETTINGS_GEOMETRY = "lastGeometry";
const QString SETTINGS_WINDOWSTATE = "lastWindowState";

//static
void CommonWindowHandling::storeGeometry(QMainWindow *windowIn)
{
    if (windowIn == 0)
        return;

    QSettings settings;

    const QRect rect = windowIn->geometry();
    settings.setValue(SETTINGS_GEOMETRY, rect);

    const Qt::WindowStates wState = windowIn->windowState();
    const int wStateInt = wState;
    settings.setValue(SETTINGS_WINDOWSTATE, wStateInt);
}

//static
void CommonWindowHandling::restoreGeometry(QMainWindow *windowIn)
{
    if (windowIn == 0)
        return;

    QSettings settings;

    if (!settings.contains(SETTINGS_GEOMETRY))
        return;

    const QRect rect = settings.value(SETTINGS_GEOMETRY).toRect();

    QDesktopWidget * desktop = QApplication::desktop();
    bool onScreen = false;
    for (int i = 0; i < desktop->numScreens() && !onScreen; i++)
    {
        if (desktop->availableGeometry(i).contains(rect))
            onScreen = true;
    }
    if (onScreen)
        windowIn->setGeometry(rect);
    else
        qDebug() << "Can't restore geometry off-screen";

    if (!settings.contains(SETTINGS_WINDOWSTATE))
        return;

    bool ok;
    const int wStateInt = settings.value(SETTINGS_WINDOWSTATE).toInt(&ok);
    const Qt::WindowStates wState(wStateInt);
    if (ok)
        windowIn->setWindowState(wState);
}
