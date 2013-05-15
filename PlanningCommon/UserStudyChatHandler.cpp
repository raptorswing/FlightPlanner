#include "UserStudyChatHandler.h"

#include <QTimer>
#include <QtCore>
#include <QStringBuilder>
#include <QtDebug>
#include <QMapIterator>

#include <random>

const qreal FOREGROUND_MEAN = 15000;
const qreal FOREGROUND_STDEV = 13000;

const qreal BACKGROUND_MEAN = 7500;
const qreal BACKGROUND_STDEV = 5500;

UserStudyChatHandler::UserStudyChatHandler(ChatWidget *widget,
                                           QObject *parent) :
    QObject(parent), _chatWidget(widget), _lastForegroundCode(-50)
{

    _foregroundTimer = new QTimer(this);
    _backgroundTimer = new QTimer(this);

    connect(_foregroundTimer,
            SIGNAL(timeout()),
            this,
            SLOT(generateForegroundEvent()));
    connect(_backgroundTimer,
            SIGNAL(timeout()),
            this,
            SLOT(generateBackgroundEvent()));

    this->changeTimerInterval(_foregroundTimer, FOREGROUND_MEAN, FOREGROUND_STDEV);
    this->changeTimerInterval(_backgroundTimer, BACKGROUND_MEAN, BACKGROUND_STDEV);

    connect(this,
            SIGNAL(messageGenerated(QString,QString)),
            widget,
            SLOT(receiveMessage(QString,QString)));

    connect(widget,
            SIGNAL(messageSent(QString)),
            this,
            SLOT(handleUserMessage(QString)));

    _backgroundMessages.append("Roger that.");
    _backgroundMessages.append("Sasquatch detected in sector %1!");
    _backgroundMessages.append("%1, what's your twenty?");
    _backgroundMessages.append("Standing by in sector %2");
    _backgroundMessages.append("10-4");
    _backgroundMessages.append("10-4, good buddy!");
    _backgroundMessages.append("%1, please report to sector %2. Code %3.");
    _backgroundMessages.append("%1, report to sector %2. Code orange %3.");
    _backgroundMessages.append("Commander, we're finished searching sector %2. Requesting orders.");
    _backgroundMessages.append("Negative.");
    _backgroundMessages.append("Negative. I'm on break!");
    _backgroundMessages.append("Requesting confirmation. Sector %1 code %2?");
    _backgroundMessages.append("Fred, can you bring us some Mtn. Dew?");
    _backgroundMessages.append("We're searching the $feature now");
    _backgroundMessages.append("Ok.");
    _backgroundMessages.append("Standing by");
    _backgroundMessages.append("$name, what's your twenty?");
    _backgroundMessages.append("Roger that, $name.");

    _naturalFeatures.append("gorge");
    _naturalFeatures.append("ravine");
    _naturalFeatures.append("hill");
    _naturalFeatures.append("hillside");
    _naturalFeatures.append("trail");
    _naturalFeatures.append("stream");
    _naturalFeatures.append("riverbank");
    _naturalFeatures.append("road");

    _names.append("Fred");
    _names.append("George");
    _names.append("Miriam");
    _names.append("Martha");
    _names.append("Alice");
    _names.append("Bob");
    _names.append("Tom");
    _names.append("Lucy");
    _names.append("Robert");
    _names.append("Alan");
    _names.append("Mike");
    _names.append("Susan");
}

QString UserStudyChatHandler::toString() const
{
    QString toRet;

    QListIterator<QPair<QDateTime, QDateTime >> iter(_response2Stimulus);
    while (iter.hasNext())
    {
        const QPair<QDateTime, QDateTime>& pair = iter.next();
        const QDateTime& responseTime = pair.first;
        const QDateTime& stimulusTime = pair.second;

        QString responseString;
        if (_responses.contains(responseTime))
        {
            responseString = _responses.value(responseTime);
            responseString.replace(",","");
        }

        QString stimulusString;
        if (_stimulii.contains(stimulusTime))
        {
            stimulusString = _stimulii.value(stimulusTime);
            stimulusString.replace(",","");
        }

        QString diff = "0";
        if (!responseTime.isNull() && !stimulusTime.isNull())
            diff = QString::number(stimulusTime.msecsTo(responseTime) / 1000.0);


        toRet += QString::number(responseTime.toMSecsSinceEpoch())
                % ", " % responseString
                % ", " % QString::number(stimulusTime.toMSecsSinceEpoch())
                % ", " % stimulusString
                % ", " % diff
                % "\n";
    }

    return toRet;
}

//private slot
void UserStudyChatHandler::handleUserMessage(const QString &msg)
{
    const QString searchString = QString::number(_lastForegroundCode);
    const QDateTime current = QDateTime::currentDateTimeUtc();

    if (msg.contains(searchString) && _lastForegroundCode >= 0)
    {
        const QDateTime mostRecent = _lastForegroundTime;
        const int offset = mostRecent.msecsTo(current);

        _response2Stimulus.append(QPair<QDateTime, QDateTime>(current, mostRecent));

        qDebug() << "User responded" << offset << "ms later";
        _lastForegroundCode = -50;
    }
    else
    {
        _response2Stimulus.append(QPair<QDateTime, QDateTime>(current, QDateTime()));
        qDebug() << "Unmatched user response";
    }

    _responses.insert(current, msg);
}

//private slot
void UserStudyChatHandler::generateBackgroundEvent()
{
    const int teammateNumber = (qrand() % 300) + 1;
    const QString sender = "Searcher #" % QString::number(teammateNumber);

    const QString& baseMessage = _backgroundMessages.at(qrand() % _backgroundMessages.size());
    QString message = baseMessage;
    while (message.contains("%"))
        message = message.arg((qrand() % 300) + 1);

    while (message.contains("$feature"))
    {
        const int index = message.indexOf("$feature");
        message = message.replace(index, 8, _naturalFeatures.at(qrand() % _naturalFeatures.size()));
    }

    while (message.contains("$name"))
    {
        const int index = message.indexOf("$name");
        message = message.replace(index, 5, _names.at(qrand() % _names.size()));
    }

    this->messageGenerated(sender, message);

    this->changeTimerInterval(_backgroundTimer, BACKGROUND_MEAN, BACKGROUND_STDEV);
}

//private slot
void UserStudyChatHandler::generateForegroundEvent()
{
    const int codeNumber = (qrand() % 99) + 1;
    const QString sender = "Incident Commander";
    const QString message = "Attention all units: please acknowledge with code " % QString::number(codeNumber);

    this->messageGenerated(sender, message);

    /*
     *if the user didn't reply to a foreground event before a new one is generated we make a note of it
     *as being missed.
    */
    if (_lastForegroundCode >= 0)
        _response2Stimulus.append(QPair<QDateTime,QDateTime>(QDateTime(), _lastForegroundTime));

    _lastForegroundTime = QDateTime::currentDateTimeUtc();
    _lastForegroundCode = codeNumber;
    _stimulii.insert(_lastForegroundTime, QString::number(_lastForegroundCode));

    this->changeTimerInterval(_foregroundTimer, FOREGROUND_MEAN, FOREGROUND_STDEV);
}

//private
void UserStudyChatHandler::changeTimerInterval(QTimer *timer,
                                               qreal meanMS,
                                               qreal stdDev)
{
    std::default_random_engine generator;
    generator.seed(qrand());
    std::normal_distribution<qreal> normal(meanMS, stdDev);

    qreal nextMS = -50;
    while (nextMS < 0)
        nextMS = normal(generator);

    if (timer->isActive())
        timer->setInterval(nextMS);
    else
        timer->start(nextMS);
}
