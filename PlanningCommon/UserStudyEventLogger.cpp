#include "UserStudyEventLogger.h"

#include <QFile>
#include <QStringBuilder>
#include <QtDebug>
#include <QDateTime>
#include <QStringList>

UserStudyEventLogger::UserStudyEventLogger(const QString &prefix, QObject *parent) :
    QObject(parent)
{
    this->setPrefix(prefix);
}

UserStudyEventLogger::~UserStudyEventLogger()
{
    this->flush();
}

const QString &UserStudyEventLogger::prefix() const
{
    return _prefix;
}

void UserStudyEventLogger::setPrefix(const QString &nPrefix)
{
    _prefix = nPrefix;
}

void UserStudyEventLogger::logFlightPerformance(UserStudyEventLogger *logger,
                                                const QString &log,
                                                const SimulatedFlierResults &results,
                                                qreal flightLengthMeters)
{
    if (logger == 0)
        return;

    QStringList parts;
    parts.append(QString::number(flightLengthMeters));
    parts.append(QString::number(results.points));
    parts.append(QString::number(results.pointsPossible));
    parts.append(QString::number(results.timingViolations.size()));
    parts.append(QString::number(results.dependencyViolations.size()));
    parts.append(QString::number(results.noFlyViolations.size()));
    logger->addTimestampedCSVLine(log, parts);
}

//public slot
void UserStudyEventLogger::addLine(const QString &log, const QString &line)
{
    //the QHash [] operator will add a new empty log for us automatically if needed
    _logs[log].append(line);
}

//public slot
void UserStudyEventLogger::addTimestampedLine(const QString &log, const QString &line)
{
    QDateTime current = QDateTime::currentDateTime();
    const QString dateline = QString::number(current.toMSecsSinceEpoch());

    this->addLine(log, dateline % " - " % line);
}

void UserStudyEventLogger::addCSVLine(const QString &log, const QStringList &csvLine)
{
    QString toAdd = "";

    for (int i = 0; i < csvLine.size(); i++)
    {
        QString cleaned = csvLine.at(i);
        cleaned.replace(",","");
        toAdd.append(cleaned);

        if (i < csvLine.size() - 1)
            toAdd.append(",");
    }

    this->addLine(log, toAdd);
}

void UserStudyEventLogger::addTimestampedCSVLine(const QString &log, const QStringList &csvLine)
{
    QDateTime current = QDateTime::currentDateTime();
    const QString dateline = QString::number(current.toMSecsSinceEpoch());

    QStringList parts = csvLine;
    parts.prepend(dateline);

    this->addCSVLine(log, parts);
}

void UserStudyEventLogger::flush()
{
    foreach(const QString& logname, _logs.keys())
    {
        const QList<QString>& lines = _logs[logname];

        const QString filename = this->prefix() % " " % logname;
        QFile fp(filename);
        if (!fp.open(QFile::Append | QFile::Text))
        {
            qWarning() << "Failed to open log file" << filename;
            continue;
        }

        QByteArray toWrite;
        foreach(const QString& line, lines)
        {
            toWrite.append(line);
            if (!line.endsWith("\n"))
                toWrite.append("\n");
        }
        fp.write(toWrite);
    }

    _logs.clear();
}
