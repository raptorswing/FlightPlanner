#ifndef SUBFLIGHTNODE_H
#define SUBFLIGHTNODE_H

#include <QSharedPointer>

#include "Wayset.h"
#include "UAVOrientation.h"
#include "guts/Conversions.h"

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

    const Wayset& path() const;

    const QVector3D& xyz();

private:
    Position _position;
    UAVOrientation _orientation;
    QSharedPointer<SubFlightNode> _parent;

    Wayset _path;

    QVector3D _xyz;
};

#endif // SUBFLIGHTNODE_H
