#include "ChatWidget.h"
#include "ui_ChatWidget.h"

ChatWidget::ChatWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ChatWidget)
{
    ui->setupUi(this);
    this->on_chatLineEntry_textChanged("");
}

ChatWidget::~ChatWidget()
{
    delete ui;
}

//public slot
void ChatWidget::sendMessage(const QString &toSend)
{
    //this->ui->chatDisplay->appendPlainText("You: " + toSend);
    this->ui->chatDisplay->appendHtml("<b>You: </b>" + toSend);
    this->messageSent(toSend);
}

//public slot
void ChatWidget::receiveMessage(const QString &sender, const QString &toReceive)
{
    //this->ui->chatDisplay->appendPlainText(sender + ": " + toReceive);
    this->ui->chatDisplay->appendHtml("<b>" + sender + ": </b>" + toReceive);
}

//private slot
void ChatWidget::on_sendButton_clicked()
{
    const QString toSend = this->ui->chatLineEntry->text();
    this->ui->chatLineEntry->setText("");
    this->sendMessage(toSend);
}

//private slot
void ChatWidget::on_chatLineEntry_textChanged(const QString &arg1)
{
    this->ui->sendButton->setDisabled(arg1.isEmpty());
}
