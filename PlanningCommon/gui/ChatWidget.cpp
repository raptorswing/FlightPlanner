#include "ChatWidget.h"
#include "ui_ChatWidget.h"

#include <QTimer>

const qreal RATIO_CRIT = 50.0;  //Good response rate of 50% or lower is critically bad
const qreal RATIO_OKAY = 75.0;  //Good response rate of 50% - 75% is okay. Above is good.

const qreal TIME_CRIT = 10.0;   //Average response time of 10.0 seconds or above is critically bad
const qreal TIME_OKAY = 5.0;    //Average response time of 5.0 - 10.0 seconds is okay. Below 5.0 is good.

const QColor COLOR_CRIT = Qt::red;
const QColor COLOR_OKAY = Qt::yellow;
const QColor COLOR_GOOD = Qt::green;

const QString SMILE_CRIT = ":/smiles/sad.png";
const QString SMILE_OKAY = ":/smiles/plain.png";
const QString SMILE_GOOD = ":/smiles/smile.png";

const int FLASH_MAX = 11;
const int FLASH_INTERVAL = 200;


ChatWidget::ChatWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ChatWidget)
{
    ui->setupUi(this);
    this->on_chatLineEntry_textChanged("");

    _flashTimer = new QTimer(this);
    connect(_flashTimer,
            SIGNAL(timeout()),
            this,
            SLOT(doFlashes()));

    _missedFlashCount = FLASH_MAX;
    _goodFlashCount = FLASH_MAX;
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

//public slot
void ChatWidget::addGoodResponse(qint64 latencyMS)
{
    this->ui->goodCountBox->setValue(this->ui->goodCountBox->value() + 1);
    this->updateGoodRate();

    const qreal newLatency = latencyMS / 1000.0;
    const qreal lastAvg = this->ui->averageTimeBox->value();
    const int newCount = this->ui->goodCountBox->value();

    const qreal newAvg = lastAvg + (newLatency - lastAvg) / newCount;
    this->ui->averageTimeBox->setValue(newAvg);


    QString style = "background-color: rgb(%1, %2, %3);";
    QColor color = COLOR_GOOD;
    QString icon = SMILE_GOOD;
    if (newAvg > TIME_OKAY)
    {
        color = COLOR_OKAY;
        icon = SMILE_OKAY;
    }
    else if (newAvg > TIME_CRIT)
    {
        color = COLOR_CRIT;
        icon = SMILE_CRIT;
    }
    this->ui->averageTimeBox->setStyleSheet(style.arg(color.red()).arg(color.green()).arg(color.blue()));
    this->ui->averageTimeIcon->setPixmap(QPixmap(icon));

    _goodFlashCount = 0;
    _flashTimer->start(FLASH_INTERVAL);
}

//public slot
void ChatWidget::addMissedResponse()
{
    this->ui->missedCountBox->setValue(this->ui->missedCountBox->value() + 1);
    this->updateGoodRate();

    _missedFlashCount = 0;
    _flashTimer->start(FLASH_INTERVAL);
}

//private slot
void ChatWidget::doFlashes()
{
    const QString style = "background-color: rgb(%1, %2, %3);";
    if (_missedFlashCount++ < FLASH_MAX)
    {
        const QColor color = COLOR_CRIT;
        if (_missedFlashCount % 2 == 0)
            this->ui->missedCountBox->setStyleSheet(style.arg(color.red()).arg(color.green()).arg(color.blue()));
        else
            this->ui->missedCountBox->setStyleSheet("");
    }
    else if (_goodFlashCount++ < FLASH_MAX)
    {
        const QColor color = COLOR_GOOD;
        if (_goodFlashCount % 2 == 0)
            this->ui->goodCountBox->setStyleSheet(style.arg(color.red()).arg(color.green()).arg(color.blue()));
        else
            this->ui->goodCountBox->setStyleSheet("");
    }
    else
        _flashTimer->stop();

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

//private
void ChatWidget::updateGoodRate()
{
    qreal total = this->ui->goodCountBox->value() + this->ui->missedCountBox->value();

    qreal rate = 0.0;
    if (total > 0.0)
        rate = this->ui->goodCountBox->value() / total * 100.0;


    this->ui->goodRateBox->setValue(rate);

    QString style = "background-color: rgb(%1, %2, %3);";
    QColor color = COLOR_GOOD;
    QString icon = SMILE_GOOD;
    if (rate < RATIO_CRIT)
    {
        color = COLOR_CRIT;
        icon = SMILE_CRIT;
    }
    else if (rate < RATIO_OKAY)
    {
        color = COLOR_OKAY;
        icon = SMILE_OKAY;
    }

    this->ui->goodRateBox->setStyleSheet(style.arg(color.red()).arg(color.green()).arg(color.blue()));
    this->ui->goodRateIcon->setPixmap(QPixmap(icon));
}
