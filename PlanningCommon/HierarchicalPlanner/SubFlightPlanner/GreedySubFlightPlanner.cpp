#include "GreedySubFlightPlanner.h"

#include <QMap>
#include <QMutableMapIterator>
#include <QtDebug>
#include <cmath>

#include "GreedySubFlightNode.h"
#include "FlightTasks/FlightTask.h"
#include "FlightTasks/CoverageTask.h"

#include "guts/Conversions.h"

const qreal PI = 3.1415926535;

GreedySubFlightPlanner::GreedySubFlightPlanner(const UAVParameters &uavParams,
                                   const QSharedPointer<FlightTask> &task,
                                   const QSharedPointer<FlightTaskArea> &area,
                                   const Position &startPos,
                                   const UAVOrientation &startPose) :
    SubFlightPlanner(uavParams, task, area, startPos, startPose)
{
}

bool GreedySubFlightPlanner::plan()
{
    this->setResults(Wayset());

    return _greedyPlan();
}

//private
bool GreedySubFlightPlanner::_greedyPlan()
{
    QMultiMap<qreal, QSharedPointer<GreedySubFlightNode> > frontier;

    QSharedPointer<GreedySubFlightNode> rootNode(new GreedySubFlightNode(this->startPos(),
                                                                         this->startPose()));
    frontier.insert(0.0, rootNode);

    while (!frontier.isEmpty())
    {
        //Find the node in the frontier with the best fitness. Grab it and remove it
        const QList<qreal> scores = frontier.keys();
        const qreal score = scores.last();
        QList<QSharedPointer<GreedySubFlightNode> > nodes = frontier.values(score);

        //If there are several nodes with the same fitness, choose one randomly
        QSharedPointer<GreedySubFlightNode> node = nodes[qrand() % nodes.size()];
        frontier.remove(score, node);

        //If we've accomplished our task we can quit
        if (score >= this->task()->maxTaskPerformance())
        {
            qDebug() << "Done. Performance of" << score << "on sub flight";
            this->setResults(node->path());
            break;
        }
        //If our task gets to long we give up
        else if (node->path().size() >= 1000)
        {
            qDebug() << "Failed";
            qDebug() << score;
            this->setResults(node->path());
            qDebug() << this->results().last();
            return false;
        }

        //Build successors to the current node. Add them to frontier.
        for (qreal i = 1.1; i <= 6.0 * 1.5; i += 1.5)
        {
            const qreal radius = this->uavParams().minTurningRadius() * i;
            const qreal arc = this->uavParams().waypointInterval() / radius;
            const qreal leftCircleAngle = node->orientation().radians() + PI / 2.0;
            const QVector2D leftCenterPos(radius * cos(leftCircleAngle),
                                          radius * sin(leftCircleAngle));
            const qreal newAngle = leftCircleAngle - PI + arc;
            const QVector2D pos(leftCenterPos.x() + radius * cos(newAngle),
                                leftCenterPos.y() + radius * sin(newAngle));
            const qreal forwardAngle = newAngle + PI / 2.0;

            const Position posLatLon = node->position().flatOffsetToPosition(pos.toPointF());
            QSharedPointer<GreedySubFlightNode> s(new GreedySubFlightNode(posLatLon,
                                                              UAVOrientation(forwardAngle),
                                                              node));
            const qreal sScore = this->task()->calculateFlightPerformance(s->path(),
                                                                   this->area()->geoPoly(),
                                                                   this->uavParams());
            frontier.insert(sScore, s);
        }

        for (qreal i = 1.1; i <= 6.0 * 1.5; i += 1.5)
        {
            const qreal radius = this->uavParams().minTurningRadius() * i;
            const qreal arc = this->uavParams().waypointInterval() / radius;
            const qreal rightCircleAngle = node->orientation().radians() - PI / 2.0;
            const QVector2D rightCenterPos(radius * cos(rightCircleAngle),
                                           radius * sin(rightCircleAngle));
            const qreal newAngle = rightCircleAngle + PI - arc;
            const QVector2D pos(rightCenterPos.x() + radius * cos(newAngle),
                                rightCenterPos.y() + radius * sin(newAngle));
            const qreal forwardAngle = newAngle - PI / 2.0;

            const Position posLatLon = node->position().flatOffsetToPosition(pos.toPointF());
            QSharedPointer<GreedySubFlightNode> s(new GreedySubFlightNode(posLatLon,
                                                              UAVOrientation(forwardAngle),
                                                              node));
            const qreal sScore = this->task()->calculateFlightPerformance(s->path(),
                                                                          this->area()->geoPoly(),
                                                                          this->uavParams());
            frontier.insert(sScore, s);
        }
    }
    return true;
}


