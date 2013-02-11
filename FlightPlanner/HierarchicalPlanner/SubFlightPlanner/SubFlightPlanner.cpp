#include "SubFlightPlanner.h"

#include <QMap>
#include <QMutableMapIterator>
#include <QtDebug>

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

const QList<Position>& SubFlightPlanner::results() const
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
        const QList<qreal> scores = frontier.uniqueKeys();
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

        const qreal lonPerMeter = Conversions::degreesLonPerMeter(node->position().latitude());
        const qreal latPerMeter = Conversions::degreesLatPerMeter(node->position().latitude());

        //Build successors to the current node. Add them to frontier.
        const int branches = 4;
        for (int i = -branches; i <= branches; i++)
        {
            qreal successorRadians = node->orientation().radians() + _uavParams.maxTurnAngle() * ((qreal)i / (qreal) branches);
            QVector3D successorVec(cos(successorRadians), sin(successorRadians), 0);
            successorVec.normalize();
            successorVec *= _uavParams.airspeed();
            Position successorPos(node->position().longitude() + lonPerMeter * successorVec.x(),
                                  node->position().latitude() + latPerMeter * successorVec.y());

            UAVOrientation successorPose(successorRadians);
            QSharedPointer<SubFlightNode> successor(new SubFlightNode(successorPos, successorPose, node));

            const qreal successorScore = _task->calculateFlightPerformance(successor->path(), _area->geoPoly());
            frontier.insert(successorScore, successor);
        }
    }
}


