#ifndef CHATWIDGET_H
#define CHATWIDGET_H

#include <QWidget>

#include "PlanningCommon_global.h"

namespace Ui {
class ChatWidget;
}

class PLANNINGCOMMONSHARED_EXPORT ChatWidget : public QWidget
{
    Q_OBJECT

signals:
    void messageSent(const QString& message);
    
public:
    explicit ChatWidget(QWidget *parent = 0);
    ~ChatWidget();

public slots:
    void sendMessage(const QString& toSend);
    void receiveMessage(const QString& sender, const QString& toReceive);
    
private slots:
    void on_sendButton_clicked();

    void on_chatLineEntry_textChanged(const QString &arg1);

private:
    Ui::ChatWidget *ui;
};

#endif // CHATWIDGET_H
