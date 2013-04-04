#include "CommonWindowHandling.h"

#include <QSettings>
#include <QMainWindow>
#include <QDesktopWidget>
#include <QApplication>
#include <QtDebug>
#include <QMessageBox>
#include <QWidget>
#include <QFileDialog>

const QString SETTINGS_GEOMETRY = "lastGeometry";
const QString SETTINGS_WINDOWSTATE = "lastWindowState";
const QString SETTINGS_LAST_PROBLEM_DIR = "lastProblemDirectory";

const QString PROBLEM_FILE_FILTER = "*.prb";

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

//static
void CommonWindowHandling::showFlightTestResults(QWidget *parent,
                                                 bool success, qreal score)
{
    QString message;
    if (success)
        message = "Flight satisfies all constraints.";
    else message = "Flight does NOT satisfy all constraints.";

    message += "\n\nFlight Score: " + QString::number(score);


    QMessageBox::information(parent, "Flight Test Results", message);
}

//static
QString CommonWindowHandling::getOpenProblemFilename(QWidget *parent)
{
    QString lastDir = QString();
    QSettings settings;
    if (settings.contains(SETTINGS_LAST_PROBLEM_DIR))
        lastDir = settings.value(SETTINGS_LAST_PROBLEM_DIR).toString();

    const QString toRet = QFileDialog::getOpenFileName(parent,
                                                       "Select Problem",
                                                       lastDir,
                                                       PROBLEM_FILE_FILTER);

    if (!toRet.isEmpty())
        settings.setValue(SETTINGS_LAST_PROBLEM_DIR, toRet);
    return toRet;
}

//static
QString CommonWindowHandling::getSaveProblemFilename(QWidget *parent)
{
    QString lastDir = QString();
    QSettings settings;
    if (settings.contains(SETTINGS_LAST_PROBLEM_DIR))
        lastDir = settings.value(SETTINGS_LAST_PROBLEM_DIR).toString();

    const QString toRet = QFileDialog::getSaveFileName(parent,
                                                       "Select Problem Destination",
                                                       lastDir,
                                                       PROBLEM_FILE_FILTER);

    if (!toRet.isEmpty())
        settings.setValue(SETTINGS_LAST_PROBLEM_DIR, toRet);
    return toRet;
}
