#include "GreedyBacktrackDubinsSFP.h"

#include <QMultiMap>

const qreal ANGLE_INTERVAL = 3.14159265 / 8.0;

GreedyBacktrackDubinsSFP::GreedyBacktrackDubinsSFP(const UAVParameters &uavParams,
                                                   const QSharedPointer<FlightTask> &task,
                                                   const QSharedPointer<FlightTaskArea> &area,
                                                   const Position &startPos,
                                                   const UAVOrientation &startPose) :
    SubFlightPlanner(uavParams, task, area, startPos, startPose)
{
}

//virtual from SubFlightPlanner
bool GreedyBacktrackDubinsSFP::plan()
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

    QMultiMap<qreal, Wayset> frontier;
    {
        Wayset startWayset;
        startWayset.append(this->startPos(), this->startPose());
        frontier.insert(0.0, startWayset);
    }

    qreal bestScoreSoFar = 0.0;
    Wayset bestSoFar;

    const qreal stopScore = this->task()->maxTaskPerformance();

    while (!frontier.isEmpty() && bestScoreSoFar < stopScore)
    {
        bestScoreSoFar = frontier.keys().last();
        bestSoFar = frontier.take(bestScoreSoFar);

        qDebug() << (bestScoreSoFar / stopScore) << bestSoFar.lengthMeters(this->uavParams());

        foreach(const Position& enticePos, locations)
        {
            if (bestSoFar.contains(enticePos))
                continue;

            foreach(const UAVOrientation& enticePose, orientations)
            {
                Wayset newGuy = bestSoFar;
                newGuy.append(enticePos, enticePose);

                qreal score = this->task()->calculateFlightPerformance(newGuy, this->area()->geoPoly(),
                                                                             this->uavParams(), true);
                score += (score / newGuy.lengthMeters(this->uavParams()));
                frontier.insert(score, newGuy);
            }
        }
    }

    this->setResults(bestSoFar);

    return true;
}
