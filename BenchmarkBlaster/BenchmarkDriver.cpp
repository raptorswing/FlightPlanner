#include "BenchmarkDriver.h"

#include <QtDebug>
#include <QTimer>
#include <QCoreApplication>
#include <QSharedPointer>
#include <QStringBuilder>
#include <QFile>
#include <QDataStream>
#include <QPolygonF>
#include <QDateTime>
#include <QStringList>
#include <QThread>

#include "PlanningProblem.h"
#include "Position.h"

#include "FlightTasks/FlyThroughTask.h"
#include "FlightTasks/NoFlyFlightTask.h"
#include "FlightTasks/SamplingTask.h"
#include "FlightTasks/CoverageTask.h"

#include "HierarchicalPlanner/HierarchicalPlanner.h"

#include "gui/CommonFileHandling.h"

const int MIN_TASKS = 1;
const int MAX_TASKS = 15;
const int TRIALS_PER_LEVEL = 5;

const int MAX_DISTANCE_METERS = 1500;

const int MIN_WIDTH_METERS = 300;
const int MIN_HEIGHT_METERS = 300;

const int MAX_MORE_WIDTH_METERS = 150;
const int MAX_MORE_HEIGHT_METERS = 150;

const QString LOGNAME = "Benchmarks.csv";


const Position START_POS(-111.658684, 40.233722);

BenchmarkDriver::BenchmarkDriver(QObject *parent) :
    QObject(parent)
{
    _logger = new UserStudyEventLogger("Benchmark", this);

    qsrand(663458226535);

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

            if (meterRect.contains(QPointF(0.0, 0.0)))
                continue;

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

        //Assign a task to each task area.
        foreach(const QPolygonF& geoPoly, geoPolys)
        {
            QSharedPointer<FlightTaskArea> area(new FlightTaskArea(geoPoly));
            QSharedPointer<FlightTask> task;

            const int taskNum = (qrand() % 100) + 1;
            if (taskNum <= 10 && problem->areas().size() > 0 && false)
                task = QSharedPointer<FlightTask>(new NoFlyFlightTask());
            else if (taskNum <= 33)
                task = QSharedPointer<FlightTask>(new CoverageTask());
            else if (taskNum <= 66)
                task = QSharedPointer<FlightTask>(new SamplingTask(100.0));
            else
                task = QSharedPointer<FlightTask>(new FlyThroughTask());

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

    //Try to build a flight
    {
        const QTime startTime = QTime::currentTime();

        QThread * thread = new QThread(this);

        QSharedPointer<HierarchicalPlanner> planner(new HierarchicalPlanner(problem));
        planner->moveToThread(thread);
        connect(thread,
                SIGNAL(started()),
                planner.data(),
                SLOT(startPlanning()));
        connect(planner.data(),
                SIGNAL(plannerPaused()),
                thread,
                SLOT(quit()));

        //planner->startPlanning();
        thread->start();
        while (thread->isRunning())
        {
            thread->wait(50);
            QCoreApplication::processEvents();
        }

        const QTime stopTime = QTime::currentTime();
        const qreal msecs = startTime.msecsTo(stopTime);

        QStringList csvParts;
        csvParts.append(QString::number(numTasks));
        csvParts.append(QString::number(trialNum));
        if (planner->bestFlightSoFar().isEmpty())
        {
            csvParts.append("FALSE");
            csvParts.append("");
        }
        else
        {
            csvParts.append("TRUE");
            csvParts.append(QString::number(msecs/1000.0));

            const QString filename = QString::number(numTasks).rightJustified(4, '0')
                    % " trial "
                    % QString::number(trialNum).rightJustified(4, '0')
                    % ".wst";
            CommonFileHandling::doExport(planner->bestFlightSoFar(), filename);
        }

        _logger->addCSVLine(LOGNAME, csvParts);
        _logger->flush();
    }
}
