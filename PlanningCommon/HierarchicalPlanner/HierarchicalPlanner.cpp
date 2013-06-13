#include "HierarchicalPlanner.h"

#include "guts/Conversions.h"

#include "QVectorND.h"
#include "SubFlightPlanner/SubFlightPlanner.h"
#include "SmartIntermediatePlanner.h"

#include <QMap>
#include <QtCore>
#include <cmath>
#include <limits>

const qreal TIMESLICE = 15.0; //seconds
const int SCHEDULING_TIMEOUT_SECONDS = 900; //15 minutes

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
    _startTime = QTime::currentTime();
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

    if (_startTime.secsTo(QTime::currentTime()) >= SCHEDULING_TIMEOUT_SECONDS)
    {
        this->pausePlanning();
        return;
    }


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
    const qreal stepSize = 5.0; //meters
    foreach(const QSharedPointer<FlightTaskArea>& area, _tasks2areas.values())
    {
        const QRectF boundingRect = area->geoPoly().boundingRect();
        const Position centerPos(boundingRect.center());

        const qreal lonPerMeter = Conversions::degreesLonPerMeter(centerPos.longitude());
        const qreal latPerMeter = Conversions::degreesLatPerMeter(centerPos.latitude());
        const qreal bbWidthMeters = boundingRect.width() / lonPerMeter;
        const qreal bbHeightMeters = boundingRect.height() / latPerMeter;

        qreal mostDistance = -500000;
        bool gotBetter = false;
        Position bestPoint1(area->geoPoly().first());
        Position bestPoint2(area->geoPoly().first());
        for (int angleDeg = 0; angleDeg < 179; angleDeg++)
        {
            const QVector2D dirVec(cos(angleDeg * 180.0 / 3.14159265),
                                   sin(angleDeg * 180.0 / 3.14159265));

            int count = 0;
            const int maxCount = qCeil(sqrt(pow(bbWidthMeters / 2.0, 2.0) + pow(bbHeightMeters / 2.0, 2.0)) / stepSize) + 1;
            bool lastPosSense = false;
            bool lastNegSense = false;
            QList<Position> posChanges;
            QList<Position> negChanges;
            while (count <= maxCount)
            {
                const QPointF posOffset = count * stepSize * dirVec.toPointF();
                const QPointF negOffset = count * stepSize * -1 * dirVec.toPointF();
                const QPointF trialPointPos = centerPos.flatOffsetToPosition(posOffset).lonLat();
                const QPointF trialPointNeg = centerPos.flatOffsetToPosition(negOffset).lonLat();

                const bool posSense = area->geoPoly().containsPoint(trialPointPos, Qt::OddEvenFill);
                const bool negSense = area->geoPoly().containsPoint(trialPointNeg, Qt::OddEvenFill);
                if (posSense != lastPosSense)
                    posChanges.append(trialPointPos);

                if (negSense != lastNegSense)
                    negChanges.append(trialPointNeg);

                lastPosSense = posSense;
                lastNegSense = negSense;
                count++;
            }

            Position candidateA, candidateB;
            if (posChanges.size() + negChanges.size() <= 1)
                continue;
            else if (posChanges.isEmpty())
            {
                candidateA = negChanges.first();
                candidateB = negChanges.last();
            }
            else if (negChanges.isEmpty())
            {
                candidateA = posChanges.first();
                candidateB = posChanges.last();
            }
            else
            {
                candidateA = posChanges.last();
                candidateB = negChanges.last();
            }


            const qreal distance = candidateA.flatDistanceEstimate(candidateB);
            if (distance > mostDistance)
            {
                mostDistance = distance;
                bestPoint1 = candidateA;
                bestPoint2 = candidateB;
                gotBetter = true;
            }
        }

        //The point closest to all the other areas will be the start
        Position start = bestPoint2;
        Position end = bestPoint1;
        if (end.flatDistanceEstimate(avgPos) < start.flatDistanceEstimate(avgPos))
        {
            start = bestPoint1;
            end = bestPoint2;
        }
        if (!gotBetter)
            qWarning() << "Start pos/pose heuristic failed for area" << area->areaName() << "!";

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
    qDebug() << "Building sub flights";
    foreach(const QSharedPointer<FlightTask>& task, _tasks)
    {
        const QSharedPointer<FlightTaskArea>& area = _tasks2areas.value(task);
        const Position& start = _areaStartPositions.value(area);
        const UAVOrientation& startPose = _areaStartOrientations.value(area);

        qDebug() << "Build sub-flight for" << task->taskName() << area->areaName() << start << startPose;

        SubFlightPlanner planner(this->problem()->uavParameters(), task, area, start, startPose);
        if (!planner.plan())
        {
            qWarning() << "Sub flight planning failed";
            this->resetPlanning();
            return;
        }

        _taskSubFlights.insert(task, planner.results());
        qDebug() << "Sub-flight should take" << planner.results().timeToFly(this->problem()->uavParameters()) << " seconds";
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

    //This hash stores the actual A* "cost so far" to reach a node.
    QHash<QVectorND, qreal> actualCosts;

    QMultiMap<qreal, QVectorND> worklist;
    QSet<QVectorND> closedSet;
    worklist.insert((startState - endState).manhattanDistance(), startState);
    actualCosts.insert(startState, 0);

    QList<QVectorND> schedule;

    bool allowInterTaskTransitions = false;
    if (_tasks.size() >= 2)
    {
        for (int i = 0; i < _tasks.size() - 1; i++)
        {
            QList<TimingConstraint> a = _tasks.at(i)->timingConstraints();
            QList<TimingConstraint> b = _tasks.at(i+1)->timingConstraints();

            if (a.size() != 1 || b.size() != 1)
            {
                allowInterTaskTransitions = true;
                break;
            }
            else if (a.first() != b.first())
            {
                allowInterTaskTransitions = true;
                break;
            }
        }
    }

    qDebug() << "Allow inter-task transitions? " << allowInterTaskTransitions;


    bool solutionFound = false;
    while (!worklist.isEmpty())
    {
        const QTime currentTime = QTime::currentTime();
        if (_startTime.secsTo(currentTime) >= SCHEDULING_TIMEOUT_SECONDS)
            break;
        else
            qDebug() << _startTime.secsTo(currentTime) << "seconds elapsed";

        const qreal costKey = worklist.keys().first();
        const QVectorND state = worklist.value(costKey);
        worklist.remove(costKey, state);
        closedSet.insert(state);

        qDebug() << "At:" << state << "with cost" << costKey;

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

            if (lastTasks.contains(state) && !allowInterTaskTransitions)
            {
                const int lastTaskIndex = lastTasks.value(state);
                if (state[lastTaskIndex] < endState[lastTaskIndex]
                        && i != lastTaskIndex)
                    continue;
            }



            //The time at which the subflight flying begins and ends.
            qreal startTime;
            qreal endTime;

            Wayset transitionFlight;

            //If this is the first step from start then we use one of our start transition flights
            if (!lastTasks.contains(state))
                transitionFlight = _startTransitionSubFlights.value(area);
            //If we're going to keep working on the same task as before there is not transition
            else if (lastTasks.value(state) == i)
            {
                //Nothing to do here?
            }
            else
            {
                //The task we're coming from and the task we're going to
                const QSharedPointer<FlightTask>& prevTask = _tasks.value(lastTasks.value(state));
                //const QSharedPointer<FlightTaskArea>& prevArea = _tasks2areas.value(prevTask);

                //Get current position and pose
                const UAVPose startPose = _taskSubFlights.value(prevTask).sampleAtTime(state[_tasks.indexOf(prevTask)],
                        params);
                const Position startPos = startPose.pos();
                const UAVOrientation startAngle = startPose.angle();

                //Get position/pose of context switch destination
                const UAVPose endPose = _taskSubFlights.value(task).sampleAtTime(state[i], params);
                const Position endPos = endPose.pos();
                const UAVOrientation endAngle = endPose.angle();

                //Plan intermediate flight
                transitionFlight = _generateTransitionFlight(startPos, startAngle,
                                                             endPos, endAngle);
            }

            //The time (if any) needed to fly the transition flight to this task
            const qreal transitionTime = transitionFlight.timeToFly(params);

            /*
             *The time (in total mission time) at which the next chunk of the next task would be started.
             *(Note that "next task" could be the same as previous task and transitionTime could be zero)
             */
            startTime = actualCosts.value(state) + transitionTime;

            /*
             *The time (in total mission time) at which the next chunk of the next task would be finished
             *("Next task" could be same as previous task.)
             *Generally this will just be startTime + TIMESLICE, unless there is less than TIMESLICE of
             *the task remaining to be done.
             */
            endTime = startTime + newState[i] - state[i];

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

            //The total cost of everything up to and including the move we'll make
            const qreal tentativeCostToMove = actualCosts.value(state) + endTime - actualCosts.value(state);

            //If we have found a better way to reach a state then we'll replace the current information
            if (!actualCosts.contains(newState) || actualCosts.value(newState) > tentativeCostToMove)
            {
                /*
                 * The heuristic is the estimated amount of time to fly all remaining tasks assuming no-cost
                 * transitions and no obstacles.
                 */
                const qreal heuristic = (endState - newState).manhattanDistance();

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
        const Wayset portion = subFlight.discretePortionByTime(startTime, endTime, params);
        path.append(portion);


        prevInterval = interval;
    }

    qDebug() << "Path should take" << path.timeToFly(params) << "seconds";

    this->setBestFlightSoFar(path);
    return true;
}

//private
Wayset HierarchicalPlanner::_generateTransitionFlight(const Position &startPos,
                                                      const UAVOrientation &startPose,
                                                      const Position &endPos,
                                                      const UAVOrientation &endPose)
{
    SmartIntermediatePlanner * intermed = new SmartIntermediatePlanner(this->problem()->uavParameters(),
                                                                       startPos, startPose,
                                                                       endPos, endPose,
                                                                       _obstacles);
    intermed->plan();
    Wayset toRet = intermed->results();

    /*
     *This will probably cause duplicate points in the wayset, but the Wayset class should take
     *care of removing duplicates later when sub-flights and transitions are combined. For now, we
     *need this here to make the path length measurements in the scheduler match up with
     *the results from the simulated flier!
    */
    toRet.append(endPos, endPose);
    delete intermed;

    return toRet;
}
