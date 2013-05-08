#ifndef USERSTUDYEVENTLOGGER_H
#define USERSTUDYEVENTLOGGER_H

#include <QObject>
#include <QHash>
#include <QList>

#include "SimulatedFlier.h"

#include "PlanningCommon_global.h"

class PLANNINGCOMMONSHARED_EXPORT UserStudyEventLogger : public QObject
{
    Q_OBJECT
signals:

public:
    explicit UserStudyEventLogger(const QString& prefix,
                                  QObject *parent = 0);
    ~UserStudyEventLogger();

    const QString& prefix() const;
    void setPrefix(const QString& nPrefix);


    static void logFlightPerformance(UserStudyEventLogger * logger,
                                     const QString& log,
                                     const SimulatedFlierResults& results,
                                     qreal flightLengthMeters);
    
public slots:
    void addLine(const QString& log, const QString& line);
    void addTimestampedLine(const QString& log, const QString& line);

    void addCSVLine(const QString& log, const QStringList& csvLine);
    void addTimestampedCSVLine(const QString& log, const QStringList& csvLine);

    void flush();
    

private:
    QString _prefix;

    QHash<QString, QList<QString> > _logs;
};

#endif // USERSTUDYEVENTLOGGER_H
