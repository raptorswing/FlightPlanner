#ifndef USERSTUDYCHATHANDLER_H
#define USERSTUDYCHATHANDLER_H

#include "gui/ChatWidget.h"
#include "PlanningCommon_global.h"

#include <QObject>
#include <QPointer>
#include <QMap>
#include <QDateTime>
#include <QTimer>
#include <QPair>

class PLANNINGCOMMONSHARED_EXPORT UserStudyChatHandler : public QObject
{
    Q_OBJECT
signals:
    void messageGenerated(const QString& sender, const QString& message);

public:
    explicit UserStudyChatHandler(ChatWidget * widget,
                                  QObject *parent = 0);

    QString toString() const;
    
public slots:

private slots:
    void handleUserMessage(const QString& msg);
    void generateBackgroundEvent();
    void generateForegroundEvent();

private:
    void changeTimerInterval(QTimer * timer, qreal meanMS, qreal stdDev);
    QPointer<ChatWidget> _chatWidget;

    QTimer * _foregroundTimer;
    QTimer * _backgroundTimer;

    QStringList _backgroundMessages;
    QStringList _naturalFeatures;
    QStringList _names;

    QDateTime _lastForegroundTime;
    int _lastForegroundCode;
    QList<QPair<QDateTime, QDateTime> > _response2Stimulus;
    
};

#endif // USERSTUDYCHATHANDLER_H
