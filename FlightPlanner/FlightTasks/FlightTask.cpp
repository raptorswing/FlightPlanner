#include "FlightTask.h"

#include <cmath>
#include <QMutableListIterator>

const qreal PI = 3.1415926535897932384626433;
const qreal SQRT2PI = sqrt(2.0*PI);

FlightTask::FlightTask()
{
    _taskName = "Untitled";
    this->addTimingConstraint(TimingConstraint(0, 3600));
}

FlightTask::~FlightTask()
{
}

//for de-serializing
FlightTask::FlightTask(QDataStream &stream)
{
    stream >> _taskName;
    stream >> _timingConstraints;
}

void FlightTask::serialize(QDataStream &stream) const
{
    stream << _taskName;
    stream << _timingConstraints;
}

//virtual
bool FlightTask::shortnessRewardApplies() const
{
    return true;
}

qreal FlightTask::priority() const
{
    return 1.0;
}

qreal FlightTask::maxTaskPerformance() const
{
    return 100.0;
}

const QList<TimingConstraint> &FlightTask::timingConstraints() const
{
    return _timingConstraints;
}

void FlightTask::setTimingConstraints(const QList<TimingConstraint> &nConstraints)
{
    _timingConstraints = nConstraints;
    this->flightTaskChanged();
}

void FlightTask::addTimingConstraint(const TimingConstraint &nConstraint)
{
    _timingConstraints.append(nConstraint);
    this->flightTaskChanged();
}

void FlightTask::removeTimingConstraint(const TimingConstraint &nConstraint)
{
    _timingConstraints.removeAll(nConstraint);
    this->flightTaskChanged();
}

void FlightTask::removeTimingConstraintAt(int index)
{
    _timingConstraints.removeAt(index);
    this->flightTaskChanged();
}

const QString &FlightTask::taskName() const
{
    return _taskName;
}

void FlightTask::setTaskName(const QString &nName)
{
    _taskName = nName;
    this->flightTaskChanged();
}

const QList<QWeakPointer<FlightTask> > &FlightTask::dependencyConstraints() const
{
    return _dependencyConstraints;
}

void FlightTask::setDependencyConstraints(const QList<QWeakPointer<FlightTask> > &nConstraints)
{
    _dependencyConstraints = nConstraints;

    foreach(const QWeakPointer<FlightTask>& task, _dependencyConstraints)
    {
        QSharedPointer<FlightTask> strong = task.toStrongRef();
        if (strong.isNull())
            continue;

        //We want to know when our dependencies blow up
        connect(strong.data(),
                SIGNAL(destroyed()),
                this,
                SLOT(handleDependencyDeleted()));
    }

    this->flightTaskChanged();
}

//private slot
void FlightTask::handleDependencyDeleted()
{
    /*
     * Some sucka-punk dependency has gone and gotten themselves deleted.
     * For good measure, loop through and remove all references to deleted dependencies.
     * Yes, I am lazy.
    */
    QMutableListIterator<QWeakPointer<FlightTask> > iter(_dependencyConstraints);

    while (iter.hasNext())
    {
        const QWeakPointer<FlightTask>& dependency = iter.next();
        if (dependency.isNull())
            iter.remove();
    }

    //Be sure to let everyone know that we've changed.
    this->flightTaskChanged();
}

//protected static
qreal FlightTask::normal(qreal x, qreal stdDev, qreal scaleFactor)
{
    qreal expPart = exp(-0.5 * pow(x / stdDev, 2.0) / scaleFactor);
    qreal otherPart = stdDev * SQRT2PI;
    return (1.0 / otherPart) * expPart;
}
