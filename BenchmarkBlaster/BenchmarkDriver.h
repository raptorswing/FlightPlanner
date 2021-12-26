#ifndef BENCHMARKDRIVER_H
#define BENCHMARKDRIVER_H

#include <QObject>

#include "UserStudyEventLogger.h"

class BenchmarkDriver : public QObject
{
    Q_OBJECT
public:
    explicit BenchmarkDriver(QObject *parent = 0);
    
signals:
    
public slots:

private slots:
    void start();
    void doTrial(int numTasks, int trialNum);

private:
    UserStudyEventLogger * _logger;
};

#endif // BENCHMARKDRIVER_H
