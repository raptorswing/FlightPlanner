#include "BenchmarkDriver.h"

#include <QtDebug>
#include <QTimer>
#include <QCoreApplication>
#include <QSharedPointer>
#include <QStringBuilder>
#include <QFile>
#include <QDataStream>
#include <QPolygonF>

#include "PlanningProblem.h"
#include "Position.h"

#include "FlightTasks/FlyThroughTask.h"

const int MIN_TASKS = 1;
const int MAX_TASKS = 20;
const int TRIALS_PER_LEVEL = 5;

const int MAX_DISTANCE_METERS = 1000;

const int MIN_WIDTH_METERS = 150;
const int MIN_HEIGHT_METERS = 150;

const int MAX_MORE_WIDTH_METERS = 150;
const int MAX_MORE_HEIGHT_METERS = 150;


const Position START_POS(-111.658684, 40.233722);

BenchmarkDriver::BenchmarkDriver(QObject *parent) :
    QObject(parent)
{
    _logger = new UserStudyEventLogger("Benchmark", this);

    QTimer::singleShot(1,this, SLOT(start()));
}

//private slot
void BenchmarkDriver::start()
{
    qDebug() << "START!!!!";

    for (int numTasks = MIN_TASKS; numTasks <= MAX_TASKS; numTasks++)
    {
        for (int k = 0; k < TRIALS_PER_LEVEL; k++)
        {
            this->doTrial(numTasks,k);
        }
    }

    qDebug() << "DONE!!!!";
    QCoreApplication::quit();
}

//private slot
void BenchmarkDriver::doTrial(int numTasks, int trialNum)
{
    QSharedPointer<PlanningProblem> problem(new PlanningProblem());
    problem->setStartingPosition(START_POS);

    //Generate random orientation
    {
        const int randomDegrees = qrand() % 360;
        problem->setStartingOrientation(UAVOrientation(randomDegrees,false));
    }

    //Generate task areas
    {
        //First we need to make some non-overlapping rectangles
        QList<QRectF> rects;
        while (rects.size() < numTasks)
        {
            //Generate a topleft offset, width, and height in meters
            const qreal widthMeters = (qrand() % MAX_MORE_WIDTH_METERS) + MIN_WIDTH_METERS;
            const qreal heightMeters = (qrand() % MAX_MORE_HEIGHT_METERS) + MIN_HEIGHT_METERS;

            const QPointF topLeftOffsetMeters((qrand() % (2 * MAX_DISTANCE_METERS)) - MAX_DISTANCE_METERS - widthMeters/2.0,
                                              (qrand() % (2 * MAX_DISTANCE_METERS)) - MAX_DISTANCE_METERS - heightMeters/2.0);
            const QRectF meterRect(topLeftOffsetMeters, QSize(widthMeters, heightMeters));

            bool intersection = false;
            foreach(const QRectF& other, rects)
            {
                if (other.intersects(meterRect))
                {
                    intersection = true;
                    break;
                }
            }
            if (!intersection)
                rects.append(meterRect);
        }

        //Next we want to convert the rectangles from meter offsets into "geo polygons"
        QList<QPolygonF> geoPolys;
        foreach(const QRectF& offsetRect, rects)
        {
            QPolygonF geoPoly;
            geoPoly << START_POS.flatOffsetToPosition(offsetRect.topLeft()).lonLat();
            geoPoly << START_POS.flatOffsetToPosition(offsetRect.topRight()).lonLat();
            geoPoly << START_POS.flatOffsetToPosition(offsetRect.bottomRight()).lonLat();
            geoPoly << START_POS.flatOffsetToPosition(offsetRect.bottomLeft()).lonLat();
            geoPolys.append(geoPoly);
        }

        foreach(const QPolygonF& geoPoly, geoPolys)
        {
            QSharedPointer<FlightTaskArea> area(new FlightTaskArea(geoPoly));
            QSharedPointer<FlightTask> task(new FlyThroughTask());
            area->addTask(task);
            problem->addTaskArea(area);
        }
    }

    //Write generated scenario to disk
    {
        const QString filename = QString::number(numTasks).rightJustified(4, '0')
                % " trial "
                % QString::number(trialNum).rightJustified(4, '0')
                % ".prb";
        QFile fp(filename);
        if (!fp.open(QFile::WriteOnly | QFile::Truncate))
        {
            qWarning() << "Failed to open" << filename;
        }
        else
        {
            QDataStream stream(&fp);
            problem->serialize(stream);
            qDebug() << "Wrote" << filename;
        }
    }
}
