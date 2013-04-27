#include "SubFlightPlanner.h"

#include <QMap>
#include <QMutableMapIterator>
#include <QtDebug>
#include <cmath>

#include "SubFlightNode.h"
#include "FlightTasks/FlightTask.h"
#include "FlightTasks/CoverageTask.h"

#include "guts/Conversions.h"

const qreal PI = 3.1415926535;

SubFlightPlanner::SubFlightPlanner(const UAVParameters &uavParams,
                                   const QSharedPointer<FlightTask> &task,
                                   const QSharedPointer<FlightTaskArea> &area,
                                   const Position &startPos,
                                   const UAVOrientation &startPose) :
    _uavParams(uavParams), _task(task), _area(area), _startPos(startPos), _startPose(startPose)
{
}

void SubFlightPlanner::plan()
{
    _results.clear();

    _greedyPlan();
}

const Wayset& SubFlightPlanner::results() const
{
    return _results;
}

//private
void SubFlightPlanner::_greedyPlan()
{
    QMultiMap<qreal, QSharedPointer<SubFlightNode> > frontier;

    QSharedPointer<SubFlightNode> rootNode(new SubFlightNode(_startPos, _startPose));
    frontier.insert(0.0, rootNode);

    while (!frontier.isEmpty())
    {
        //Find the node in the frontier with the best fitness. Grab it and remove it
        const QList<qreal> scores = frontier.keys();
        const qreal score = scores.last();
        QList<QSharedPointer<SubFlightNode> > nodes = frontier.values(score);

        //If there are several nodes with the same fitness, choose one randomly
        QSharedPointer<SubFlightNode> node = nodes[qrand() % nodes.size()];
        frontier.remove(score, node);

        //If we've accomplished our task we can quit
        if (score >= _task->maxTaskPerformance())
        {
            qDebug() << "Done. Performance of" << score << "on sub flight";
            _results = node->path();
            break;
        }
        //If our task gets to long we give up
        else if (node->path().size() >= 1000)
        {
            qDebug() << "Failed";
            qDebug() << score;
            _results = node->path();
            qDebug() << _results.last();
            break;
        }

        //Build successors to the current node. Add them to frontier.
        for (qreal i = 1.2; i <= 6.0 * 1.5; i += 1.5)
        {
            const qreal radius = _uavParams.minTurningRadius() * i;
            const qreal arc = _uavParams.waypointInterval() / radius;
            const qreal leftCircleAngle = node->orientation().radians() + PI / 2.0;
            const QVector2D leftCenterPos(radius * cos(leftCircleAngle),
                                          radius * sin(leftCircleAngle));
            const qreal newAngle = leftCircleAngle - PI + arc;
            const QVector2D pos(leftCenterPos.x() + radius * cos(newAngle),
                                leftCenterPos.y() + radius * sin(newAngle));
            const qreal forwardAngle = newAngle + PI / 2.0;

            const Position posLatLon = node->position().flatOffsetToPosition(pos.toPointF());
            QSharedPointer<SubFlightNode> s(new SubFlightNode(posLatLon,
                                                              UAVOrientation(forwardAngle),
                                                              node));
            const qreal sScore = _task->calculateFlightPerformance(s->path(), _area->geoPoly(), _uavParams);
            frontier.insert(sScore, s);
        }

        for (qreal i = 1.2; i <= 6.0 * 1.5; i += 1.5)
        {
            const qreal radius = _uavParams.minTurningRadius() * i;
            const qreal arc = _uavParams.waypointInterval() / radius;
            const qreal rightCircleAngle = node->orientation().radians() - PI / 2.0;
            const QVector2D rightCenterPos(radius * cos(rightCircleAngle),
                                           radius * sin(rightCircleAngle));
            const qreal newAngle = rightCircleAngle + PI - arc;
            const QVector2D pos(rightCenterPos.x() + radius * cos(newAngle),
                                rightCenterPos.y() + radius * sin(newAngle));
            const qreal forwardAngle = newAngle - PI / 2.0;

            const Position posLatLon = node->position().flatOffsetToPosition(pos.toPointF());
            QSharedPointer<SubFlightNode> s(new SubFlightNode(posLatLon,
                                                              UAVOrientation(forwardAngle),
                                                              node));
            const qreal sScore = _task->calculateFlightPerformance(s->path(), _area->geoPoly(), _uavParams);
            frontier.insert(sScore, s);
        }
    }
}


