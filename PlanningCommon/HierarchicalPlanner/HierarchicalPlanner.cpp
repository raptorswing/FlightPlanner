#include "HierarchicalPlanner.h"

#include "guts/Conversions.h"

#include "QVectorND.h"
#include "SubFlightPlanner/SubFlightPlanner.h"
#include "SubFlightPlanner/SubFlightNode.h"
#include "AstarPRMIntermediatePlanner/AstarPRMIntermediatePlanner.h"
#include "DubinsIntermediate/DubinsIntermediatePlanner.h"

#include <QMap>
#include <QtCore>
#include <cmath>
#include <limits>

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
    if (!_buildSchedule())
        qDebug() << "Scheduling failed";
    this->pausePlanning();
}

//protected
//pure-virtual from FlightPlanner
void HierarchicalPlanner::doReset()
{
    _tasks.clear();
    _tasks2areas.clear();
    _areaStartPositions.clear();
    _areaStartOrientations.clear();
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
    //First calculate the average of all of the task area's midpoints
    //(Or at least an approximation based on their bounding rectangles...)
    QPointF avgLonLat(0.0,0.0);
    foreach(const QSharedPointer<FlightTaskArea>& area, _tasks2areas.values())
        avgLonLat += area->geoPoly().boundingRect().center();
    avgLonLat += this->problem()->startingPosition().lonLat();
    if (_tasks2areas.values().size() > 0)
        avgLonLat /= (_tasks2areas.values().size() + 1);

    const Position avgPos(avgLonLat);

    //Then loop through all of the areas and find good points that could be start or end
    //Make the start point the one that is closest to the average computed above
    const qreal stepSize = 20.0; //meters
    foreach(const QSharedPointer<FlightTaskArea>& area, _tasks2areas.values())
    {
        const QRectF boundingRect = area->geoPoly().boundingRect();
        const Position centerPos(boundingRect.center());

        const qreal lonPerMeter = Conversions::degreesLonPerMeter(centerPos.longitude());
        const qreal latPerMeter = Conversions::degreesLatPerMeter(centerPos.latitude());
        const qreal bbWidthMeters = boundingRect.width() / lonPerMeter;
        const qreal bbHeightMeters = boundingRect.height() / latPerMeter;

        qreal mostDistance = -500000;
        Position bestPoint1;
        Position bestPoint2;
        for (int angleDeg = 0; angleDeg < 179; angleDeg++)
        {
            bool gotPos = false;
            bool gotNeg = false;

            const QVector2D dirVec(cos(angleDeg * 180.0 / 3.14159265),
                                   sin(angleDeg * 180.0 / 3.14159265));

            int count = 0;
            const int maxCount = qCeil(sqrt(pow(bbWidthMeters / 2.0, 2.0) + pow(bbHeightMeters / 2.0, 2.0)) / stepSize) + 1;
            Position pos;
            Position neg;
            bool lastPosSense = false;
            bool lastNegSense = false;
            while (count <= maxCount)
            {
                const QPointF posOffset = count * stepSize * dirVec.toPointF();
                const QPointF negOffset = count * stepSize * -1 * dirVec.toPointF();
                const QPointF trialPointPos = centerPos.flatOffsetToPosition(posOffset).lonLat();
                const QPointF trialPointNeg = centerPos.flatOffsetToPosition(negOffset).lonLat();

                const bool posSense = area->geoPoly().containsPoint(trialPointPos, Qt::OddEvenFill);
                const bool negSense = area->geoPoly().containsPoint(trialPointNeg, Qt::OddEvenFill);
                if (!posSense && lastPosSense)
                {
                    pos = trialPointPos;
                    gotPos = true;
                }

                if (!negSense && lastNegSense)
                {
                    neg = trialPointNeg;
                    gotNeg = true;
                }

                lastPosSense = posSense;
                lastNegSense = negSense;
                count++;
            }

            if (!gotPos || !gotNeg)
                continue;

            const qreal distance = pos.flatDistanceEstimate(neg);
            if (distance > mostDistance)
            {
                mostDistance = distance;
                bestPoint1 = pos;
                bestPoint2 = neg;
            }
        }

        Position start;
        Position end;

        //The point closest to all the other areas will be the start
        start = bestPoint2;
        end = bestPoint1;
        if (end.flatDistanceEstimate(avgPos) < start.flatDistanceEstimate(avgPos))
        {
            start = bestPoint1;
            end = bestPoint2;
        }

        _areaStartPositions.insert(area, start);

        const qreal angleRads = start.angleTo(end);
        const UAVOrientation orientation(angleRads);
        _areaStartOrientations.insert(area, orientation);
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
        if (_startTransitionSubFlights.contains(area))
            continue;
        const Position& taskStartPos = _areaStartPositions.value(area);
        const UAVOrientation& taskStartPose = _areaStartOrientations.value(area);

        Wayset subFlight = _generateTransitionFlight(globalStartPos,
                                                              globalStartPose,
                                                              taskStartPos,
                                                              taskStartPose);

        _startTransitionSubFlights.insert(area, subFlight);
    }
}

//private
void HierarchicalPlanner::_buildSubFlights()
{
    foreach(const QSharedPointer<FlightTask>& task, _tasks)
    {
        const QSharedPointer<FlightTaskArea>& area = _tasks2areas.value(task);
        const Position& start = _areaStartPositions.value(area);
        const UAVOrientation& startPose = _areaStartOrientations.value(area);

        qDebug() << "Build sub-flight for" << task.data() << area.data() << start << startPose;

        SubFlightPlanner planner(this->problem()->uavParameters(), task, area, start, startPose);
        planner.plan();

        _taskSubFlights.insert(task, planner.results());
    }
}

//private
bool HierarchicalPlanner::_buildSchedule()
{
    const UAVParameters& params = this->problem()->uavParameters();

    //First we need to know how long each of our sub-flights takes
    QList<qreal> taskTimes;
    foreach(const QSharedPointer<FlightTask>& task, _tasks)
    {
        const Wayset& subFlight = _taskSubFlights.value(task);

        //Time required is estimated to be the length of the path in meters divided by airspeed
        const qreal timeRequired = subFlight.timeToFly(params);
        taskTimes.append(timeRequired);
    }

    //start and end states
    const QVectorND startState(_tasks.size());
    const QVectorND endState(taskTimes);

    qDebug() << "Schedule from" << startState << "to" << endState;

    //This hash stores child:parent relationships
    QHash<QVectorND, QVectorND> parents;

    //This hash stores node:(index of last task) relationships
    QHash<QVectorND, int> lastTasks;

    //This hash stores node:(transition flight to reach node) relationships
    QHash<QVectorND, Wayset > transitionFlights;

    QHash<QVectorND, qreal> actualCosts;

    QMultiMap<qreal, QVectorND> worklist;
    QSet<QVectorND> closedSet;
    worklist.insert((startState - endState).manhattanDistance(), startState);
    actualCosts.insert(startState, 0);

    QList<QVectorND> schedule;

    bool solutionFound = false;
    while (!worklist.isEmpty())
    {
        const qreal costKey = worklist.keys().first();
        const QVectorND state = worklist.value(costKey);
        worklist.remove(costKey, state);
        closedSet.insert(state);

        //qDebug() << "At:" << state << "with cost" << costKey;

        if (state == endState)
        {
            qDebug() << "Done scheduling - traceback.";
            solutionFound = true;
            QVectorND current = state;
            while (true)
            {
                qDebug() << current << actualCosts.value(current);
                schedule.prepend(current);
                if (!parents.contains(current))
                    break;
                current = parents.value(current);
            }
            break;
        }

        //Generate possible transitions
        for (int i = 0; i < state.dimension(); i++)
        {
            QVectorND newState = state;
            newState[i] = qMin<qreal>(taskTimes[i], newState[i] + TIMESLICE);
            if (newState[i] == state[i])
                continue;
            if (closedSet.contains(newState))
                continue;

            const QSharedPointer<FlightTask>& task = _tasks.value(i);
            const QSharedPointer<FlightTaskArea>& area = _tasks2areas.value(task);

            //The time at which the subflight flying begins and ends.
            qreal startTime;
            qreal endTime;

            Wayset transitionFlight;

            /*
             * The heuristic is the amount of time to fly all remaining tasks assuming no-cost
             * transitions and no obstacles.
             */
            qreal heuristic = (endState - newState).manhattanDistance();
            if (!lastTasks.contains(state))
                transitionFlight = _startTransitionSubFlights.value(area);
            else if (lastTasks.value(state) == i)
            {
                //Nothing to do here?
            }
            else
            {
                //The task we're coming from and the task we're going to
                const QSharedPointer<FlightTask>& prevTask = _tasks.value(lastTasks.value(state));
                const QSharedPointer<FlightTaskArea>& prevArea = _tasks2areas.value(prevTask);

                //Get current position and pose
                Position startPos;
                UAVOrientation startPose;
                _interpolatePath(_taskSubFlights.value(prevTask),
                                 _areaStartOrientations.value(prevArea),
                                 state[_tasks.indexOf(prevTask)],
                        &startPos,
                        &startPose);

                //Get position/pose of context switch destination
                Position endPos;
                UAVOrientation endPose;
                _interpolatePath(_taskSubFlights.value(task),
                                 _areaStartOrientations.value(area),
                                 state[i],
                                 &endPos,
                                 &endPose);

                //Plan intermediate flight
                transitionFlight = _generateTransitionFlight(startPos, startPose,
                                                             endPos, endPose);
            }

            //The time (if any) needed to fly the transition flight to this task
            const qreal transitionTime = transitionFlight.timeToFly(params);
            startTime = actualCosts.value(state) + transitionTime;
            endTime = startTime + newState[i] - state[i];
            const qreal tentativeCostToMove = actualCosts.value(state) + endTime - actualCosts.value(state);

            //Check dependency constraints
            bool dependencyViolation = false;
            foreach(const QWeakPointer<FlightTask> wConstraint, task->dependencyConstraints())
            {
                QSharedPointer<FlightTask> constraint = wConstraint.toStrongRef();
                if (constraint.isNull())
                    continue;
                int index = _tasks.indexOf(constraint);
                if (index == -1)
                    continue;
                if (state.val(index) < taskTimes.at(index))
                {
                    dependencyViolation = true;
                    break;
                }
            }
            if (dependencyViolation)
                continue;

            //If the newstate violates timing constraints then we won't generate it
            bool timingSatisfied = false;
            foreach(const TimingConstraint& constraint, task->timingConstraints())
            {
                if (startTime < constraint.start() || startTime > constraint.end())
                    continue;
                else if (endTime < constraint.start() || endTime > constraint.end())
                    continue;
                else
                {
                    timingSatisfied = true;
                    break;
                }
            }
            if (!timingSatisfied)
                continue;

            //If we have found a better way to reach a state then we'll replace the current information
            if (!actualCosts.contains(newState) || actualCosts.value(newState) > tentativeCostToMove)
            {
                //newState's parent is state
                parents.insert(newState, state);

                actualCosts.insert(newState, tentativeCostToMove);
                lastTasks.insert(newState, i);

                transitionFlights.insert(newState, transitionFlight);

                worklist.insert(tentativeCostToMove + heuristic, newState);
            }
        } // Done generating transitions
    } // Done building schedule

    if (!solutionFound)
        return false;

    QVectorND prevInterval = schedule[0];
    schedule.removeFirst();

    Wayset path;
    foreach(const QVectorND& interval, schedule)
    {
        const int taskIndex = lastTasks.value(interval);
        const QSharedPointer<FlightTask> task = _tasks.value(taskIndex);
        const QSharedPointer<FlightTaskArea> area = _tasks2areas.value(task);

        if (prevInterval == startState)
            path.append(_startTransitionSubFlights.value(area));
        else if (prevInterval == startState
                 || lastTasks.value(prevInterval) != taskIndex)
            path.append(transitionFlights.value(interval));

        //Add the portion of the sub-flight that we care about
        const Wayset& subFlight = _taskSubFlights.value(task);
        const qreal startTime = prevInterval.val(taskIndex);
        const qreal endTime = interval.val(taskIndex);
        path.append(_getPathPortion(subFlight, startTime, endTime));


        prevInterval = interval;
    }

    this->setBestFlightSoFar(path);
    return true;
}

//private
bool HierarchicalPlanner::_interpolatePath(const Wayset &path,
                                           const UAVOrientation &startingOrientation,
                                           qreal goalTime,
                                           Position *outPosition,
                                           UAVOrientation *outOrientation) const
{
    if (outPosition == 0 || outOrientation == 0)
    {
        qDebug() << "Can't interpolate: bad output position/orientation pointer(s).";
        return false;
    }
    else if (goalTime < 0.0)
    {
        qDebug() << "Can't interpolate: bad time.";
        return false;
    }
    else if (path.isEmpty())
    {
        qDebug() << "Can't interpolate: empty path.";
        return false;
    }
    else if (path.size() == 1)
    {
        *outPosition = path.at(0).pos();
        *outOrientation = startingOrientation;
        return true;
    }

    const UAVParameters& params = this->problem()->uavParameters();

    //qDebug() << "Interpolate path of size" << path.size() << "to time" << goalTime;

    qreal distanceSoFar = 0.0;
    qreal timeSoFar = 0.0;

    for (int i = 1; i < path.size(); i++)
    {
        const Position& pos = path.at(i).pos();
        const Position& lastPos = path.at(i-1).pos();
        //const qreal distance = (Conversions::lla2xyz(pos) - Conversions::lla2xyz(lastPos)).length();
        const qreal intervalDistance = params.waypointInterval();
        distanceSoFar += intervalDistance;
        timeSoFar = distanceSoFar / params.airspeed();


        if (timeSoFar >= goalTime || i == path.size() - 1)
        {
            const qreal lonPerMeter = Conversions::degreesLonPerMeter(pos.latitude());
            const qreal latPerMeter = Conversions::degreesLatPerMeter(pos.latitude());
            const qreal lastTime = timeSoFar - intervalDistance / params.airspeed();
            const qreal ratio = (goalTime - lastTime) / (timeSoFar - lastTime);
            QVector2D dirVecMeters((pos.longitude() - lastPos.longitude()) / lonPerMeter,
                                   (pos.latitude() - lastPos.latitude()) / latPerMeter);
            const qreal distToGo = params.waypointInterval() * ratio;
            dirVecMeters.normalize();
            const qreal longitude = lastPos.longitude() + distToGo * dirVecMeters.x() * lonPerMeter;
            const qreal latitude = lastPos.latitude() + distToGo * dirVecMeters.y() * latPerMeter;
            *outPosition = Position(longitude, latitude);
            *outOrientation = UAVOrientation(atan2(dirVecMeters.y(),
                                                   dirVecMeters.x()));
            break;
        }
    }

    if (timeSoFar < goalTime)
    {
        //qDebug() << "Can't interpolate into future. Goal time" << goalTime << "but only reached" << timeSoFar;
        //return false;
    }
    return true;
}

//private
Wayset HierarchicalPlanner::_generateTransitionFlight(const Position &startPos,
                                                               const UAVOrientation &startPose,
                                                               const Position &endPos,
                                                               const UAVOrientation &endPose)
{
    //qDebug() << "Intermediate from" << startPos << startPose.radians() << "to" << endPos << endPose.radians();
    //Adjust the positions backwards a little bit along their angles?

    //    DubinsIntermediatePlanner * intermed = new DubinsIntermediatePlanner(this->problem()->uavParameters(),
    //                                                                         startPos, startPose,
    //                                                                         endPos, endPose,
    //                                                                         _obstacles);
    AstarPRMIntermediatePlanner * intermed = new AstarPRMIntermediatePlanner(this->problem()->uavParameters(),
                                                                             startPos, startPose,
                                                                             endPos, endPose,
                                                                             _obstacles);
    intermed->plan();
    Wayset toRet = intermed->results();
    delete intermed;

    return toRet;
}

//private
Wayset HierarchicalPlanner::_getPathPortion(const Wayset &path,
                                                     qreal portionStartTime,
                                                     qreal portionEndTime) const
{
    Wayset toRet;

    const UAVParameters& params = this->problem()->uavParameters();
    const int startingIndex = portionStartTime * params.airspeed() / params.waypointInterval();
    const int endingIndex = qMin<int>(portionEndTime * params.airspeed() / params.waypointInterval(),
                                      path.size() - 1);
    //const int count = endingIndex - startingIndex;

    for (int i = startingIndex; i < endingIndex; i++)
        toRet.append(path.at(i));


    return toRet;
}
