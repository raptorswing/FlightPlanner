#include "DubinSubFlightPlanner.h"

#include <QtDebug>

const qreal ANGLE_INTERVAL = 3.14159265 / 8.0;

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

    QList<Position> bins = this->task()->bins(this->area()->geoPoly());
    QList<Position> locations = this->task()->transformedBins(this->area()->geoPoly());


    //Get a list of orientations that we should try in our search
    QList<UAVOrientation> orientations;

    {
        qreal widthRads = 2.0 * 3.14159265;
        if (this->task()->sensorType() == FlightTask::DirectionalSensor)
            widthRads = this->uavParams().directionalSensorViewAngleRadians();

        const qreal centerAngleRads = locations.first().angleTo(bins.first());
        const AngleRange angleRange(UAVOrientation(centerAngleRads),widthRads, true);

        UAVOrientation toAdd(centerAngleRads);
        for (int j = 0; j < (2.0 * 3.14159265) / ANGLE_INTERVAL; j++)
        {
            if (angleRange.withinRange(toAdd))
                orientations.append(toAdd);

            toAdd = UAVOrientation(toAdd.radians() + ANGLE_INTERVAL);
        }
    }

    Wayset current;
    current.append(this->startPos(), this->startPose());
    qreal bestScore = -500;

    while (!locations.isEmpty() && bestScore < this->task()->maxTaskPerformance())
    {
        bestScore = -500;
        int bestBinIndex = -1;
        UAVOrientation bestOrientation;

        for (int i = 0; i < locations.size(); i++)
        {
            const Position& binPos = locations.at(i);
            Wayset toTest = current;

            foreach(const UAVOrientation& approachAngle, orientations)
            {
                toTest.append(binPos, approachAngle);

                const qreal score = this->task()->calculateFlightPerformance(toTest,
                                                                             this->area()->geoPoly(),
                                                                             this->uavParams(),
                                                                             false);
                if (score > bestScore)
                {
                    bestScore = score;
                    bestOrientation = approachAngle;
                    bestBinIndex = i;
                }
            }
        }

        //qDebug() << bestScore << "/" << _task->maxTaskPerformance();

        if (bestBinIndex < 0 || bestBinIndex >= locations.size())
        {
            qWarning() << "best bin index is out of range";
            return false;
        }

        const Position toAdd = locations.at(bestBinIndex);
        if (this->task()->taskType() != "Sampling")
            locations.removeAt(bestBinIndex);
        current.append(toAdd, bestOrientation);
    }

    qDebug() << bestScore;

    UAVParameters fudgeParams = this->uavParams();
    fudgeParams.setMinTurningRadius(fudgeParams.minTurningRadius() * 1.02);
    this->setResults(current.resample(this->uavParams().waypointInterval(), fudgeParams));

    return true;
}
