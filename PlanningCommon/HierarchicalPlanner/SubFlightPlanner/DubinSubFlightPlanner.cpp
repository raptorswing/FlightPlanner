#include "DubinSubFlightPlanner.h"

#include <QtDebug>

DubinSubFlightPlanner::DubinSubFlightPlanner(const UAVParameters &uavParams,
                                             const QSharedPointer<FlightTask> &task,
                                             const QSharedPointer<FlightTaskArea> &area,
                                             const Position &startPos,
                                             const UAVOrientation &startPose) :
    SubFlightPlanner(uavParams, task, area, startPos, startPose)
{
}

bool DubinSubFlightPlanner::plan()
{
    this->setResults(Wayset());

    const QList<Position> bins = this->task()->bins(this->area()->geoPoly());
    const QList<Position> tBins = this->task()->transformedBins(this->area()->geoPoly());

    QList<Position> binsToUse = tBins;
    if (binsToUse.isEmpty())
        binsToUse = bins;

    UAVOrientation approachAngle(this->task()->validSensorAngleRange().center().degrees() + 180.0, false);

    UAVParameters fudgeParams = this->uavParams();
    fudgeParams.setMinTurningRadius(fudgeParams.minTurningRadius() * 1.02);

    Wayset current;
    current.append(this->startPos(), this->startPose());

    qreal bestScore = -500;

    while (!binsToUse.isEmpty() && bestScore < this->task()->maxTaskPerformance())
    {
        bestScore = -500;
        int bestBinIndex = -1;

        for (int i = 0; i < binsToUse.size(); i++)
        {
            const Position& binPos = binsToUse.at(i);
            Wayset toTest = current;
            toTest.append(binPos, approachAngle);

            //toTest = toTest.resample(3.0, fudgeParams);

            const qreal score = this->task()->calculateFlightPerformance(toTest,
                                                                         this->area()->geoPoly(),
                                                                         this->uavParams(),
                                                                         false);
            if (score > bestScore)
            {
                bestScore = score;
                bestBinIndex = i;
            }
        }

        //qDebug() << bestScore << "/" << _task->maxTaskPerformance();

        if (bestBinIndex < 0 || bestBinIndex >= binsToUse.size())
        {
            qWarning() << "best bin index is out of range";
            return false;
        }

        const Position toAdd = binsToUse.at(bestBinIndex);
        if (this->task()->taskType() != "Sampling")
            binsToUse.removeAt(bestBinIndex);
        current.append(toAdd, approachAngle);
    }

    qDebug() << bestScore;

    this->setResults(current.resample(this->uavParams().waypointInterval(), fudgeParams));
    //_toRet = current;


    return true;
}
