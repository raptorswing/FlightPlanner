#include "GreedySubFlightNode.h"

GreedySubFlightNode::GreedySubFlightNode(const Position &pos,
                             const UAVOrientation &pose,
                             const QSharedPointer<GreedySubFlightNode> &parent) :
    _position(pos), _orientation(pose)
{
    this->setParent(parent);
}

const Position &GreedySubFlightNode::position() const
{
    return _position;
}

const UAVOrientation &GreedySubFlightNode::orientation() const
{
    return _orientation;
}

QSharedPointer<GreedySubFlightNode> GreedySubFlightNode::parent() const
{
    return _parent;
}

void GreedySubFlightNode::setParent(const QSharedPointer<GreedySubFlightNode> &parent)
{
    _parent = parent;

    _path.clear();

    if (!_parent.isNull())
        _path = _parent->path();

    _path.append(this->position(),
                 this->orientation());
}

const Wayset &GreedySubFlightNode::path() const
{
    return _path;
}

const QVector3D &GreedySubFlightNode::xyz()
{
    if (_xyz.isNull())
        _xyz = Conversions::lla2xyz(_position);
    return _xyz;
}
