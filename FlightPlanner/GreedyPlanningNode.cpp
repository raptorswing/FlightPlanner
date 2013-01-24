#include "GreedyPlanningNode.h"

#include <QScopedPointer>
#include <cmath>

#include "guts/Conversions.h"

const qreal minTurningRadius = 100.0;
const qreal speedMetersPerSecond = 15.0;
const qreal secondsPerStep = 5;
const qreal branchFactor = 3;

const qreal PI = 3.1415926535897932384626433;

GreedyPlanningNode::GreedyPlanningNode(const Position &pos,
                                       const UAVOrientation &orientation,
                                       int depth,
                                       int maxDepth,
                                       QSharedPointer<GreedyPlanningNode> parent) :
    _pos(pos), _orientation(orientation), _depth(depth), _maxDepth(maxDepth), _parent(parent), _visited(false)
{
}

GreedyPlanningNode::~GreedyPlanningNode()
{
}

const Position &GreedyPlanningNode::position() const
{
    return _pos;
}

const UAVOrientation &GreedyPlanningNode::orientation() const
{
    return _orientation;
}

int GreedyPlanningNode::depth() const
{
    return _depth;
}

int GreedyPlanningNode::maxDepth() const
{
    return _maxDepth;
}

bool GreedyPlanningNode::isVisited() const
{
    return _visited;
}

QVector<QSharedPointer<GreedyPlanningNode> > GreedyPlanningNode::visit()
{
    _visited = true;

    QVector<QSharedPointer<GreedyPlanningNode> > toRet;

    if (this->depth() >= this->maxDepth())
        return toRet;

    /*
    QVector<qreal> radii;
    radii.reserve(branchFactor+1);

    const int branchesPerSide = (branchFactor-1)/2;

    //left branches
    for (int i = 0; i < branchesPerSide; i++)
        radii.append(-1 * minTurningRadius - (i+1));

    //right branches
    for (int i = 0 ; i < branchesPerSide; i++)
        radii.append(minTurningRadius + (i + 1));

    //The ENU position of the UAV relative to the UAV. Always 0,0,0
    QVector3D enuPos(0,0,0);

    //distance (in meters) we can travel in one tick (along a circle or straight)
    const qreal distance = secondsPerStep * speedMetersPerSecond;

    foreach(qreal radius, radii)
    {
        //Negative radius is a sign for left vs. right turn
        qreal opSign = 1;
        if (radius < 0)
            opSign = -1;

        //Make radius positive in case it was negative
        radius = qAbs<qreal>(radius);

        //Figure out the angle in the circle that points to our position on the circle
        qreal angleToUs = this->orientation().radians() + opSign*PI/2;

        QVector3D ourENU(0,0,0);

        qreal circleX = ourENU.x() - radius*cos(angleToUs);
        qreal circleY = ourENU.y() - radius*sin(angleToUs);

        qreal angularDistance = distance / radius;
        qreal newAngleToUs = angleToUs + -1*opSign*angularDistance;

        qreal newX = circleX + radius*cos(newAngleToUs);
        qreal newY = circleY + radius*sin(newAngleToUs);
        QVector3D newENU(newX,newY,ourENU.z());

        Position newPos = Conversions::enu2lla(newENU,this->position());
        qreal newOrientation = newAngleToUs - opSign*PI/2;

        QSharedPointer<GreedyPlanningNode> node(new GreedyPlanningNode(newPos,
                                                                       newOrientation,
                                                                       this->depth() + 1,
                                                                       this->maxDepth()));
        toRet.append(node);
    }
    */

    for (int i = -1; i <= 1; i++)
    {
        qreal successorRadians = this->orientation().radians() + i*(PI / 4.0);
        QVector3D successorENU(cos(successorRadians), sin(successorRadians), 0);
        successorENU *= 45.0;
        Position successorPos = Position::fromENU(this->position(), successorENU);
        UAVOrientation successorPose(successorRadians);
        toRet.append(QSharedPointer<GreedyPlanningNode>(new GreedyPlanningNode(successorPos,
                                                                               successorPose,
                                                                               this->depth() + 1,
                                                                               this->maxDepth())));
    }

    return toRet;
}

const QSharedPointer<GreedyPlanningNode> &GreedyPlanningNode::parent() const
{
    return _parent;
}

void GreedyPlanningNode::setParent(const QSharedPointer<GreedyPlanningNode> &nParent)
{
    _parent = nParent;
}

const QList<Position> &GreedyPlanningNode::flightPath()
{
    if (_flightPath.isEmpty())
    {
        if (!this->parent().isNull())
            _flightPath = this->parent()->flightPath();
        _flightPath.append(this->position());
    }
    return _flightPath;
}

void GreedyPlanningNode::setFlighPath(const QList<Position> &flightPath)
{
    _flightPath = flightPath;
}
