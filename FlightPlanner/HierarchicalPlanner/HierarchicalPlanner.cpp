#include "HierarchicalPlanner.h"

#include "guts/Conversions.h"

#include "QVectorND.h"
#include "SubFlightPlanner.h"

#include <QMap>

const qreal EVERY_X_METERS = 50.0;
const qreal AIRSPEED = 14.0; //meters per second
const qreal TIMESLICE = 15.0; //seconds

HierarchicalPlanner::HierarchicalPlanner(QSharedPointer<PlanningProblem> prob,
                                         QObject *parent) :
    FlightPlanner(prob, parent)
{
    this->doReset();
}

//protected
//pure-virtual from FlightPlanner
void HierarchicalPlanner::doStart()
{
}

//protected
//pure-virtual from FlightPlanner
void HierarchicalPlanner::doIteration()
{
    /*
     * Decide on arbitrary start and end points for each task (except no-fly).
     * They should be on edges of the polygon.
    */
    _buildStartAndEndPositions();

    /*
     * Calculate sub-flights from the global start point to each of the tasks' start points.
     * Also calculate sub-flights from each tasks' end point to every other tasks' start point.
    */
    _buildStartTransitions();

    /*
     * Calculate ideal sub-flights for each task (except no-fly).
     * These sub-flights start and end at the arbitrary start/end points of the tasks.
    */
    _buildSubFlights();


    /*
     * Build and solve scheduling problem.
    */
    //_buildSchedule();


    /*
     * Temporary testing stuff / store results
    */
    QList<Position> toSet;

    /*
    foreach(const QSharedPointer<FlightTaskArea>& area, _tasks2areas.values())
    {
        toSet.append(_areaStartPositions[area]);
        toSet.append(_areaEndPositions[area]);
    }
    */

    foreach(const QSharedPointer<FlightTask>& task, _tasks)
    {
        const QList<Position>& startTransitionFlight = _startTransitionSubFlights.value(task);
        foreach(const Position& pos, startTransitionFlight)
            toSet.append(pos);

        const QList<Position>& subFlight = _taskSubFlights.value(task);
        foreach(const Position& pos, subFlight)
            toSet.append(pos);
    }

    this->setBestFlightSoFar(toSet);

    this->pausePlanning();
}

//protected
//pure-virtual from FlightPlanner
void HierarchicalPlanner::doReset()
{
    _tasks.clear();
    _tasks2areas.clear();
    _areaStartPositions.clear();
    _areaEndPositions.clear();
    _taskSubFlights.clear();
    _startTransitionSubFlights.clear();

    if (this->problem().isNull())
        return;

    //Fill in list of tasks and mapping of tasks to areas
    foreach(const QSharedPointer<FlightTaskArea>& area, this->problem()->areas())
    {
        foreach(const QSharedPointer<FlightTask>& task, area->tasks())
        {
            _tasks.append(task);
            _tasks2areas.insert(task, area);
        }
    }
}

//private
void HierarchicalPlanner::_buildStartAndEndPositions()
{
    const qreal divisions = 100;
    foreach(const QSharedPointer<FlightTaskArea>& area, _tasks2areas.values())
    {
        QRectF boundingRect = area->geoPoly().boundingRect();

        Position start;
        Position end;

        //We'll lay the start/end out on the axis that intersects the most of the bounding rect.
        //This could be improved later.

        bool wasInside = false;
        bool gotStart = false;
        qreal stepSize = qMax<qreal>(boundingRect.width() / divisions,
                                     boundingRect.height() / divisions);
        if (boundingRect.width() > boundingRect.height())
        {
            qreal y = boundingRect.top() + boundingRect.height() / 2.0;
            for (int i = 0; i < divisions + 1; i++)
            {
                QPointF p(boundingRect.left() + stepSize * i, y);
                bool amInside = area->geoPoly().containsPoint(p, Qt::OddEvenFill);

                if (!wasInside && amInside && !gotStart)
                {
                    start = Position(p.x(),y);
                    gotStart = true;
                }
                else if (wasInside && !amInside)
                    end = Position(p.x(),y);

                wasInside = amInside;
            }
        }
        else
        {
            qreal x = boundingRect.left() + boundingRect.width() / 2.0;
            for (int i = 0; i < divisions + 1; i++)
            {
                QPointF p(x, boundingRect.top() + stepSize * i);
                bool amInside = area->geoPoly().containsPoint(p, Qt::OddEvenFill);

                if (!wasInside && amInside && !gotStart)
                {
                    start = Position(x,p.y());
                    gotStart = true;
                }
                else if (wasInside && !amInside)
                    end = Position(x,p.y());

                wasInside = amInside;
            }
        }

        _areaStartPositions.insert(area, start);
        _areaEndPositions.insert(area, end);
    }
}

//private
void HierarchicalPlanner::_buildStartTransitions()
{
    const Position& globalStartPos = this->problem()->startingPosition();

    foreach(const QSharedPointer<FlightTask>& task, _tasks)
    {
        const QSharedPointer<FlightTaskArea>& area = _tasks2areas.value(task);
        const Position& taskStartPos = _areaStartPositions.value(area);

        const QVector3D startXYZ = Conversions::lla2xyz(globalStartPos);
        const QVector3D endXYZ = Conversions::lla2xyz(taskStartPos);

        QVector3D direction = endXYZ - startXYZ;
        const qreal distance = direction.length();
        direction.normalize();

        int numPoints = distance / EVERY_X_METERS;

        QList<Position> subFlight;

        for (int i = 0; i < numPoints; i++)
        {
            QVector3D vec = startXYZ + direction * i * EVERY_X_METERS;
            subFlight.append(Conversions::xyz2lla(vec));
        }

        _startTransitionSubFlights.insert(task, subFlight);
    }
}

//private
void HierarchicalPlanner::_buildSubFlights()
{
    foreach(const QSharedPointer<FlightTask>& task, _tasks)
    {
        const QSharedPointer<FlightTaskArea>& area = _tasks2areas.value(task);
        const Position& start = _areaStartPositions.value(area);
        const Position& end = _areaEndPositions.value(area);

        UAVOrientation tempPose(0);

        SubFlightPlanner planner(task, area, start, end, tempPose, tempPose);
        planner.plan();

        _taskSubFlights.insert(task, planner.results());
    }
}

//private
void HierarchicalPlanner::_buildSchedule()
{
    QList<qreal> taskTimes;
    foreach(const QSharedPointer<FlightTask>& task, _tasks)
    {
        const QList<Position>& subFlight = _taskSubFlights.value(task);

        //Time required is estimated to be the length of the path in meters divided by airspeed
        qreal timeRequired = subFlight.length() * EVERY_X_METERS / AIRSPEED;
        taskTimes.append(timeRequired);
    }

    QVectorND startState(_tasks.size());
    QVectorND endState(taskTimes);

    qDebug() << "Schedule from" << startState << "to" << endState;

    QHash<QVectorND, QVectorND> parents;
    QHash<QVectorND, int> lastTasks;

    QMultiMap<qreal, QVectorND> worklist;
    QSet<QVectorND> closedList;
    worklist.insert(0, startState);
    closedList.insert(startState);

    while (!worklist.isEmpty())
    {
        QMutableMapIterator<qreal, QVectorND> iter(worklist);
        iter.next();
        QVectorND state = iter.value();
        iter.remove();

        qDebug() << state;

        if (state == endState)
        {
            qDebug() << "Done";
            //do traceback
            return;
        }

        //Generate possible transitions
        for (int i = 0; i < state.dimension(); i++)
        {
            QVectorND newState = state;
            newState[i] = qMin<qreal>(taskTimes[i], newState[i] + TIMESLICE);
            if (closedList.contains(newState))
                continue;

            //Add newState to closed list so it is never regenerated
            closedList.insert(newState);

            //newState's parent is state
            parents.insert(newState, state);
            lastTasks.insert(newState, i);

            /*
             * The cost is the distance in the state space (draw us toward end node)
             * plus transition penalties ("context switching")
             */
            qreal cost = (endState - state).length();
            if (!lastTasks.contains(state))
                cost += _startTransitionSubFlights.value(_tasks[i]).length();
            else if (lastTasks.value(state) == i)
                cost += 0.0;
            else
            {

            }

            worklist.insert(cost, newState);
        }
    }
}
