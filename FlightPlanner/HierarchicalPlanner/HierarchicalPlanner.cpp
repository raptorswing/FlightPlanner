#include "HierarchicalPlanner.h"

#include "guts/Conversions.h"

#include "QVectorND.h"
#include "SubFlightPlanner.h"
#include "SubFlightNode.h"
#include "RRTIntermediatePlanner/RRTIntermediatePlanner.h"

#include <QMap>
#include <cmath>

const qreal EVERY_X_METERS = 50.0;
const qreal AIRSPEED = 14.0; //meters per second
const qreal TIMESLICE = 15.0; //seconds
const qreal MAX_TURN_ANGLE = 3.14159265 / 4.0;

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
     * Also calculate sub-flights from each task's end point to every other tasks' start point.
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
    _areaStartOrientations.clear();
    _areaEndOrientations.clear();
    _taskSubFlights.clear();
    _startTransitionSubFlights.clear();
    _obstacles.clear();

    if (this->problem().isNull())
        return;

    //Fill in list of tasks and mapping of tasks to areas
    foreach(const QSharedPointer<FlightTaskArea>& area, this->problem()->areas())
    {
        foreach(const QSharedPointer<FlightTask>& task, area->tasks())
        {
            //We treat obstacles separately in the hierarchical planner. Not as tasks.
            if (task->taskType() == "No-Fly Zone")
                _obstacles.append(area->geoPoly());
            else
            {
                _tasks.append(task);
                _tasks2areas.insert(task, area);
            }
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

        qreal angleRads = atan2(end.latitude() - start.latitude(),
                                end.longitude() - start.longitude());
        UAVOrientation orientation(angleRads);
        _areaStartOrientations.insert(area, orientation);
        _areaEndOrientations.insert(area, orientation);
    }
}

//private
void HierarchicalPlanner::_buildStartTransitions()
{
    const Position& globalStartPos = this->problem()->startingPosition();
    const UAVOrientation& globalStartPose = this->problem()->startingOrientation();

    foreach(const QSharedPointer<FlightTask>& task, _tasks)
    {
        const QSharedPointer<FlightTaskArea>& area = _tasks2areas.value(task);
        const Position& taskStartPos = _areaStartPositions.value(area);
        const UAVOrientation& taskStartPose = _areaStartOrientations.value(area);

        QList<Position> subFlight = _generateTransitionFlight(globalStartPos,
                                                              globalStartPose,
                                                              taskStartPos,
                                                              taskStartPose);

        /*
        const QVector3D startXYZ = Conversions::lla2xyz(globalStartPos);
        const QVector3D endXYZ = Conversions::lla2xyz(taskStartPos);

        QVector3D direction = endXYZ - startXYZ;
        const qreal distance = direction.length();
        direction.normalize();

        int numPoints = distance / EVERY_X_METERS;

        for (int i = 0; i < numPoints; i++)
        {
            QVector3D vec = startXYZ + direction * i * EVERY_X_METERS;
            subFlight.append(Conversions::xyz2lla(vec));
        }
        */

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

//private
bool HierarchicalPlanner::_interpolatePath(const QList<Position> &path,
                                           const UAVOrientation &startingOrientation,
                                           qreal time,
                                           Position *outPosition,
                                           UAVOrientation *outOrientation)
{
    if (outPosition == 0 || outOrientation == 0)
        return false;
    else if (time < 0.0)
        return false;
    else if (path.isEmpty())
        return false;
    else if (path.size() == 1)
    {
        *outPosition = path[0];
        *outOrientation = startingOrientation;
        return true;
    }

    qreal distanceSoFar = 0.0;
    qreal timeSoFar = 0.0;

    for (int i = 1; i < path.size(); i++)
    {
        const Position& pos = path[i];
        const Position& lastPos = path[i-1];
        const qreal distance = (Conversions::lla2xyz(pos) - Conversions::lla2xyz(lastPos)).length();
        distanceSoFar += distance;
        timeSoFar = distanceSoFar / AIRSPEED;

        if (timeSoFar >= time)
        {
            const qreal timeFirst = timeSoFar - distance / AIRSPEED;
            const qreal ratio = (time - timeFirst) / (timeSoFar - timeFirst);
            const qreal longitude = lastPos.longitude() + ratio * (pos.longitude() - lastPos.longitude());
            const qreal latitude = lastPos.latitude() + ratio * (pos.latitude() - lastPos.latitude());
            *outPosition = Position(longitude, latitude);
            *outOrientation = UAVOrientation(atan2(pos.latitude() - lastPos.latitude(),
                                                   pos.longitude() - lastPos.longitude()));
        }
    }

    if (timeSoFar < time)
        return false;
    return true;
}

//private
QList<Position> HierarchicalPlanner::_generateTransitionFlight(const Position &startPos,
                                                               const UAVOrientation &startPose,
                                                               const Position &endPos,
                                                               const UAVOrientation &endPose)
{
    IntermediatePlanner * intermed = new RRTIntermediatePlanner(startPos,
                                                                startPose,
                                                                endPos,
                                                                endPose,
                                                                _obstacles);
    intermed->plan();
    QList<Position> toRet = intermed->results();
    delete intermed;

    return toRet;

    /*
    QList<Position> toRet;

    const QVector3D endPosXYZ = Conversions::lla2xyz(endPos);

    Position currentPos = startPos;
    UAVOrientation currentPose = startPose;

    //Simple greedy or A* planner to get in the ballpark
    QMultiMap<qreal, QSharedPointer<SubFlightNode> > frontier;
    {
        QSharedPointer<SubFlightNode> startNode(new SubFlightNode(startPos,startPose));
        const qreal dist = (startNode->xyz() - endPosXYZ).length();
        frontier.insert(dist, startNode);
    }

    const qreal lonPerMeter = Conversions::degreesLonPerMeter(currentPos.latitude());
    const qreal latPerMeter = Conversions::degreesLatPerMeter(currentPos.latitude());

    int count = 0;
    while (!frontier.isEmpty())
    {
        QList<qreal> keys = frontier.keys();
        qreal bestFScore = keys[0];
        QList<QSharedPointer<SubFlightNode> > values = frontier.values(bestFScore);
        QSharedPointer<SubFlightNode> node = values[qrand() % values.size()];
        frontier.remove(bestFScore,node);
        const QVector3D& XYZ = node->xyz();

        const qreal distToGoal = (XYZ - endPosXYZ).length();

        if (distToGoal < 5.0 * AIRSPEED)
        {
            toRet = node->path();
            currentPos = node->position();
            currentPose = node->orientation();
            frontier.clear();
            break;
        }
        else if (count++ > 10000)
        {
            qDebug() << "Intermediate failed";
            toRet = node->path();
            break;
        }

        //closedSet.append(XYZ);

        //generate successors
        const int branches = 1;
        for (int i = -branches; i <= branches; i++)
        {
            const qreal angleMod = MAX_TURN_ANGLE * ((qreal)i / (qreal)branches);
            const qreal successorRadians = node->orientation().radians() + angleMod;
            QVector3D successorVec(cos(successorRadians), sin(successorRadians), 0);
            successorVec.normalize();
            successorVec *= EVERY_X_METERS;
            const Position successorPos(node->position().longitude() + lonPerMeter * successorVec.x(),
                                        node->position().latitude() + latPerMeter * successorVec.y());

            //No flying through obstacles!
            if (_collidesWithObstacle(successorPos))
                continue;

            const QVector3D successorPosXYZ = Conversions::lla2xyz(successorPos);

            UAVOrientation successorPose(successorRadians);
            QSharedPointer<SubFlightNode> successor(new SubFlightNode(successorPos, successorPose, node));

            const qreal heuristic = (successorPosXYZ - endPosXYZ).length();
            const qreal costSoFar = node->path().length() * EVERY_X_METERS;
            const qreal costToMove = EVERY_X_METERS;

            frontier.insert(heuristic + costSoFar + costToMove, successor);
        }
    }

    //RRT Planner for remainder?
    return toRet;
    */
}

//private
bool HierarchicalPlanner::_collidesWithObstacle(const Position &pos)
{
    foreach(const QPolygonF& obstPoly, _obstacles)
    {
        if (obstPoly.containsPoint(pos.lonLat(), Qt::OddEvenFill))
            return true;
    }
    return false;
}
