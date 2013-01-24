#include "SubFlightPlanner.h"

#include <QMap>
#include <QMutableMapIterator>
#include <QtDebug>

#include "SubFlightNode.h"
#include "FlightTasks/FlightTask.h"
#include "FlightTasks/CoverageTask.h"

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
        const QList<qreal> scores = frontier.uniqueKeys();
        const qreal score = scores.last();
        QList<QSharedPointer<SubFlightNode> > nodes = frontier.values(score);

        QSharedPointer<SubFlightNode> node = nodes[qrand() % nodes.size()];
        frontier.remove(score, node);

        //qDebug() << "Chose" << node->position() << "with score " << score;

        //qDebug() << score;
        if (score >= _task->maxTaskPerformance())
        {
            qDebug() << "Done";
            qDebug() << score;
            _results = node->path();
            break;
        }
        else if (node->path().size() >= 500)
        {
            qDebug() << "Failed";
            qDebug() << score;

            QSharedPointer<CoverageTask> ct = _task.dynamicCast<CoverageTask>();
            if (!ct.isNull())
            {
                //ct->_debug = true;
                _task->calculateFlightPerformance(node->path(), _area->geoPoly());
            }

            _results = node->path();
            qDebug() << _results.last();
            break;
        }

        for (int i = -1; i <= 1; i++)
        {
            qreal successorRadians = node->orientation().radians() + i*(PI / 4.0);
            QVector3D successorENU(cos(successorRadians), sin(successorRadians), 0);
            successorENU.normalize();
            successorENU *= 30.0;
            Position successorPos = Position::fromENU(node->position(), successorENU);

            UAVOrientation successorPose(successorRadians);
            QSharedPointer<SubFlightNode> successor(new SubFlightNode(successorPos, successorPose, node));

            qreal successorScore = _task->calculateFlightPerformance(successor->path(), _area->geoPoly());
            frontier.insert(successorScore, successor);
            //qDebug() << successorPos << " " << successorRadians << " " << successorScore;
        }
    }
}


