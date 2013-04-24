#ifndef USERSTUDYCHATHANDLER_H
#define USERSTUDYCHATHANDLER_H

#include "gui/ChatWidget.h"
#include "PlanningCommon_global.h"

#include <QObject>
#include <QPointer>
#include <QMap>
#include <QDateTime>

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
    void possiblyGenerateChatEvent();
    void generateBackgroundEvent();
    void generateForegroundEvent();

private:
    QPointer<ChatWidget> _chatWidget;

    QStringList _backgroundMessages;
    QStringList _naturalFeatures;
    QStringList _names;

    QDateTime _lastForegroundTime;
    int _lastForegroundCode;
    QMap<QDateTime, QDateTime> _response2Stimulus;
    
};

#endif // USERSTUDYCHATHANDLER_H
