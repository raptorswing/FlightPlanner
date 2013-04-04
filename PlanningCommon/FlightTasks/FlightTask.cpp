#include "FlightTask.h"

#include <cmath>
#include <QtDebug>
#include <QMutableListIterator>
#include <QMutableSetIterator>

const qreal PI = 3.1415926535897932384626433;
const qreal SQRT2PI = sqrt(2.0*PI);

//static public
QHash<quint64, QWeakPointer<FlightTask> > FlightTask::_uuidToWeakTask = QHash<quint64, QWeakPointer<FlightTask> >();

FlightTask::FlightTask()
{
    _taskName = "Untitled";
    this->addTimingConstraint(TimingConstraint(0, 3600));

    const quint32 r1 = qrand();
    const quint32 r2 = qrand();
    quint64 uuid = r1;
    uuid = uuid << 32;
    uuid = uuid | r2;
    _uuid = uuid;
}

FlightTask::~FlightTask()
{
}

//for de-serializing
FlightTask::FlightTask(QDataStream &stream)
{
    stream >> _taskName;
    stream >> _timingConstraints;

    int numDependencies;
    stream >> numDependencies;
    for (int i = 0; i < numDependencies; i++)
    {
        quint64 depUUID;
        stream >> depUUID;
        _unresolvedDependencies.insert(depUUID);
    }

    stream >> _uuid;
}

void FlightTask::serialize(QDataStream &stream) const
{
    stream << _taskName;
    stream << _timingConstraints;

    stream << this->dependencyConstraints().size();
    foreach(const QWeakPointer<FlightTask>& wTask, this->dependencyConstraints())
    {
        QSharedPointer<FlightTask> strong = wTask.toStrongRef();
        if (strong.isNull())
        {
            qWarning() << "Failed to serialize flight task dependency uuid. (Strong pointer is null)";
            stream << (quint64)0;
            continue;
        }
        stream << strong->uuid();
    }

    stream << _uuid;
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
    _dependencyConstraints.clear();

    foreach(const QWeakPointer<FlightTask>& task, nConstraints)
    {
        QSharedPointer<FlightTask> strong = task.toStrongRef();
        this->addDependencyContraint(strong);
    }

    this->flightTaskChanged();
}

void FlightTask::addDependencyContraint(const QSharedPointer<FlightTask> &other)
{
    if (other.isNull())
        return;

    connect(other.data(),
            SIGNAL(destroyed()),
            this,
            SLOT(handleDependencyDeleted()));
    _dependencyConstraints.append(other);
}

quint64 FlightTask::uuid() const
{
    return _uuid;
}

void FlightTask::resolveDependencies()
{
    QMutableSetIterator<quint64> iter(_unresolvedDependencies);

    while (iter.hasNext())
    {
        quint64 depUUID = iter.next();
        if (FlightTask::_uuidToWeakTask.contains(depUUID))
        {
            this->addDependencyContraint(FlightTask::_uuidToWeakTask.value(depUUID));
            iter.remove();
        }
        else
            qWarning() << "Unresolved task dependency" << depUUID;
    }
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
