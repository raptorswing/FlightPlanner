#ifndef SUBFLIGHTNODE_H
#define SUBFLIGHTNODE_H

#include <QSharedPointer>
#include <QList>

#include "Position.h"
#include "UAVOrientation.h"

class SubFlightNode
{
public:
    SubFlightNode(const Position& pos,
                  const UAVOrientation& pose,
                  const QSharedPointer<SubFlightNode>& parent = QSharedPointer<SubFlightNode>());

    const Position& position() const;

    const UAVOrientation& orientation() const;

    QSharedPointer<SubFlightNode> parent() const;
    void setParent(const QSharedPointer<SubFlightNode>& parent);

    const QList<Position>& path() const;

private:
    Position _position;
    UAVOrientation _orientation;
    QSharedPointer<SubFlightNode> _parent;

    QList<Position> _path;
};

#endif // SUBFLIGHTNODE_H
