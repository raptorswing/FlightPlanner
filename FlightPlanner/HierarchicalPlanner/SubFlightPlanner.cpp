#include "SubFlightPlanner.h"

#include <QMap>
#include <QMutableMapIterator>
#include <QtDebug>

#include "SubFlightNode.h"
#include "FlightTasks/FlightTask.h"
#include "FlightTasks/CoverageTask.h"

#include "guts/Conversions.h"

const qreal PI = 3.1415926535;

SubFlightPlanner::SubFlightPlanner(const QSharedPointer<FlightTask> &task,
                                   const QSharedPointer<FlightTaskArea> &area,
                                   const Position &startPos,
                                   const Position &endPos,
                                   const UAVOrientation &startPose,
                                   const UAVOrientation &endPose) :
    _task(task), _area(area), _startPos(startPos), _endPos(endPos), _startPose(startPose), _endPose(endPose)
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
            qDebug() << "Done";
            qDebug() << score;
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
        for (int i = -4; i <= 4; i++)
        {
            qreal successorRadians = node->orientation().radians() + i*(PI / 16.0);
            QVector3D successorVec(cos(successorRadians), sin(successorRadians), 0);
            successorVec.normalize();
            successorVec *= 30.0;
            Position successorPos(node->position().longitude() + Conversions::degreesLonPerMeter(node->position().latitude()) * successorVec.x(),
                                  node->position().latitude() + Conversions::degreesLatPerMeter(node->position().latitude()) * successorVec.y());

            UAVOrientation successorPose(successorRadians);
            QSharedPointer<SubFlightNode> successor(new SubFlightNode(successorPos, successorPose, node));

            qreal successorScore = _task->calculateFlightPerformance(successor->path(), _area->geoPoly());
            frontier.insert(successorScore, successor);
        }
    }
}


