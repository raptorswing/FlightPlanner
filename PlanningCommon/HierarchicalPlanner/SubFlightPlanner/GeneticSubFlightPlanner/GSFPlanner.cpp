#include "GSFPlanner.h"

#include <QMutableMapIterator>

const qreal ANGLE_INTERVAL = 3.14159265 / 8.0;
const int MIN_GENERATIONS = 3;

GSFPlanner::GSFPlanner(const UAVParameters &uavParams,
                       const QSharedPointer<FlightTask> &task,
                       const QSharedPointer<FlightTaskArea> &area,
                       const Position &startPos,
                       const UAVOrientation &startPose) :
    SubFlightPlanner(uavParams, task, area, startPos, startPose)
{
}

//virtual from SubFlightPlanner
bool GSFPlanner::plan()
{
    //Locations for the genomes
    const QList<Position> bins = this->task()->bins(this->area()->geoPoly());
    const QList<Position> locations = this->task()->transformedBins(this->area()->geoPoly());

    //Orientations for the genomes
    QList<UAVOrientation> orientations;

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

    qreal bestScore = 0;
    Wayset bestFlight;
    QMultiMap<qreal, Wayset> individuals;
    int generationCount = 0;
    while (bestScore < this->task()->maxTaskPerformance() || generationCount++ < MIN_GENERATIONS)
    {
        qDebug() << "Best:" << bestScore;

        //Create random individuals until there are at least twenty
        while (individuals.size() < 20)
        {
            Wayset newGuy;
            newGuy.append(this->startPos(), this->startPose());
            for (int i = 0; i < locations.size(); i++)
            {
                newGuy.append(locations.at(qrand() % locations.size()),
                              orientations.at(qrand() % orientations.size()));
            }

            qreal newGuyScore = this->task()->calculateFlightPerformance(newGuy,
                                                                         this->area()->geoPoly(),
                                                                         this->uavParams(),
                                                                         true);
            newGuyScore += newGuyScore / (newGuy.lengthMeters(this->uavParams()) + 1.0);
            individuals.insert(newGuyScore, newGuy);
        }

        bestScore = individuals.keys().last();
        bestFlight = individuals.values().last();

        //Kill the worst ten individuals
        QMutableMapIterator<qreal, Wayset> iter(individuals);
        for (int i = 0; i < 10; i++)
        {
            iter.next();
            iter.remove();
        }

        //Breed eight new individuals
        const QList<qreal> keys = individuals.keys();
        for (int i = 0; i < 8; i++)
        {
            const Wayset A = individuals.value(keys.at(qrand() % keys.size()));
            const Wayset B = individuals.value(keys.at(qrand() % keys.size()));

            const int startA = qrand() % A.size();
            const int endA = startA + (qrand() % (A.size() - startA));

            const int startB = qrand() % B.size();
            const int endB = startB + (qrand() % (B.size() - startB));

            Wayset newGuy;
            newGuy.append(this->startPos(), this->startPose());
            for (int j = startA; j < endA; j++)
                newGuy.append(A.at(j));
            for (int j = startB; j < endB; j++)
                newGuy.append(B.at(j));

            qreal newGuyScore = this->task()->calculateFlightPerformance(newGuy,
                                                                         this->area()->geoPoly(),
                                                                         this->uavParams(),
                                                                         true);
            newGuyScore += newGuyScore / (newGuy.lengthMeters(this->uavParams()) + 1.0);
            individuals.insert(newGuyScore, newGuy);
        }
    }

    UAVParameters fudgeParams = this->uavParams();
    fudgeParams.setMinTurningRadius(fudgeParams.minTurningRadius() * 1.02);

    this->setResults(bestFlight.resample(this->uavParams().waypointInterval(), fudgeParams));

    return true;
}
