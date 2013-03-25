#include "SubFlightNode.h"

SubFlightNode::SubFlightNode(const Position &pos,
                             const UAVOrientation &pose,
                             const QSharedPointer<SubFlightNode> &parent) :
    _position(pos), _orientation(pose)
{
    this->setParent(parent);
}

const Position &SubFlightNode::position() const
{
    return _position;
}

const UAVOrientation &SubFlightNode::orientation() const
{
    return _orientation;
}

QSharedPointer<SubFlightNode> SubFlightNode::parent() const
{
    return _parent;
}

void SubFlightNode::setParent(const QSharedPointer<SubFlightNode> &parent)
{
    _parent = parent;

    _path.clear();

    if (!_parent.isNull())
        _path = _parent->path();

    _path.append(this->position());
}

const Wayset &SubFlightNode::path() const
{
    return _path;
}

const QVector3D &SubFlightNode::xyz()
{
    if (_xyz.isNull())
        _xyz = Conversions::lla2xyz(_position);
    return _xyz;
}
