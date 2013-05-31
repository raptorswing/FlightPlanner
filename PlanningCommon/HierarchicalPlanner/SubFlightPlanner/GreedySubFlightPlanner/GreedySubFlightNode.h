#ifndef SUBFLIGHTNODE_H
#define SUBFLIGHTNODE_H

#include <QSharedPointer>

#include "Wayset.h"
#include "UAVOrientation.h"
#include "guts/Conversions.h"

class GreedySubFlightNode
{
public:
    GreedySubFlightNode(const Position& pos,
                  const UAVOrientation& pose,
                  const QSharedPointer<GreedySubFlightNode>& parent = QSharedPointer<GreedySubFlightNode>());

    const Position& position() const;

    const UAVOrientation& orientation() const;

    QSharedPointer<GreedySubFlightNode> parent() const;
    void setParent(const QSharedPointer<GreedySubFlightNode>& parent);

    const Wayset& path() const;

    const QVector3D& xyz();

private:
    Position _position;
    UAVOrientation _orientation;
    QSharedPointer<GreedySubFlightNode> _parent;

    Wayset _path;

    QVector3D _xyz;
};

#endif // SUBFLIGHTNODE_H
