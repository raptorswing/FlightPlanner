#include "DubinSubFlightPlanner.h"

#include <QtDebug>

DubinSubFlightPlanner::DubinSubFlightPlanner(const UAVParameters &uavParams,
                                             const QSharedPointer<FlightTask> &task,
                                             const QSharedPointer<FlightTaskArea> &area,
                                             const Position &startPos,
                                             const UAVOrientation &startPose) :
    _uavParams(uavParams), _task(task), _area(area), _startPos(startPos), _startPose(startPose)
{
}

bool DubinSubFlightPlanner::plan()
{
    _toRet.clear();

    QList<Position> bins = _task->bins(_area->geoPoly());
    UAVOrientation approachAngle(_task->validSensorAngleRange().center().degrees() + 180.0, false);

    UAVParameters fudgeParams = _uavParams;
    fudgeParams.setMinTurningRadius(fudgeParams.minTurningRadius() * 1.02);

    Wayset current;
    current.append(_startPos, _startPose);

    qreal bestScore = -500;

    while (!bins.isEmpty() && bestScore < _task->maxTaskPerformance())
    {
        bestScore = -500;
        int bestBinIndex = -1;

        for (int i = 0; i < bins.size(); i++)
        {
            const Position& binPos = bins.at(i);
            Wayset toTest = current;
            toTest.append(binPos, approachAngle);

            toTest = toTest.resample(_uavParams.waypointInterval(), fudgeParams);

            const qreal score = _task->calculateFlightPerformance(toTest, _area->geoPoly(), _uavParams, false);
            if (score > bestScore)
            {
                bestScore = score;
                bestBinIndex = i;
            }
        }

        //qDebug() << bestScore << "/" << _task->maxTaskPerformance();

        if (bestBinIndex < 0 || bestBinIndex >= bins.size())
        {
            qWarning() << "best bin index is out of range";
            return false;
        }

        const Position toAdd = bins.at(bestBinIndex);
        if (_task->taskType() != "Sampling")
            bins.removeAt(bestBinIndex);
        current.append(toAdd, approachAngle);
    }

    _toRet = current.resample(_uavParams.waypointInterval(), fudgeParams);
    //_toRet = current;


    return true;
}

const Wayset &DubinSubFlightPlanner::results() const
{
    return _toRet;
}
