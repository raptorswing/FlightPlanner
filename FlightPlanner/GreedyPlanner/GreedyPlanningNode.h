#ifndef GREEDYPLANNINGNODE_H
#define GREEDYPLANNINGNODE_H

#include <QSharedPointer>
#include <QVector>
#include <QList>

#include "Position.h"
#include "UAVOrientation.h"

class GreedyPlanningNode
{
public:
    GreedyPlanningNode(const Position& pos = Position(),
                       const UAVOrientation& orientation = UAVOrientation(),
                       int depth=0,
                       int maxDepth = 100,
                       QSharedPointer<GreedyPlanningNode> parent = QSharedPointer<GreedyPlanningNode>());
    ~GreedyPlanningNode();

    const Position& position() const;
    const UAVOrientation& orientation() const;

    int depth() const;
    int maxDepth() const;

    bool isVisited() const;
    QVector<QSharedPointer<GreedyPlanningNode> > visit();

    const QSharedPointer<GreedyPlanningNode>& parent() const;
    void setParent(const QSharedPointer<GreedyPlanningNode>& nParent);

    const QList<Position>& flightPath();
    void setFlighPath(const QList<Position>& flightPath);
private:
    Position _pos;
    UAVOrientation _orientation;
    int _depth;
    int _maxDepth;

    QSharedPointer<GreedyPlanningNode> _parent;
    bool _visited;

    QList<Position> _flightPath;
};

#endif // GREEDYPLANNINGNODE_H
