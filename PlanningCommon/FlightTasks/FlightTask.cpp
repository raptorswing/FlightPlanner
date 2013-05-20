#include "FlightTask.h"

#include <cmath>
#include <QtDebug>
#include <QMutableListIterator>
#include <QMutableSetIterator>

const qreal PI = 3.1415926535897932384626433;
const qreal SQRT2PI = sqrt(2.0*PI);

//static public
QHash<quint64, QWeakPointer<FlightTask> > FlightTask::_uuidToWeakTask = QHash<quint64, QWeakPointer<FlightTask> >();

FlightTask::FlightTask(SensorType sensorType)
{
    _taskName = "Untitled";
    this->addTimingConstraint(TimingConstraint(0, 3600));
    this->setSensorType(sensorType);
    _validSensorAngleRange = AngleRange(UAVOrientation(), PI * 2.0);
    this->setMinSensingDistance(0);
    this->setMaxSensingDistance(100.0);

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

    QString sensorTypeString;
    stream >> sensorTypeString;
    _sensorType = FlightTask::string2SensorType(sensorTypeString);
    _validSensorAngleRange = AngleRange(stream);
    stream >> _minDistMeters;
    stream >> _maxDistMeters;

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
    stream << FlightTask::sensorType2String(_sensorType);
    _validSensorAngleRange.serialize(stream);
    stream << _minDistMeters;
    stream << _maxDistMeters;

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

FlightTask::SensorType FlightTask::sensorType() const
{
    return _sensorType;
}

void FlightTask::setSensorType(FlightTask::SensorType nType)
{
    _sensorType = nType;
}

const AngleRange &FlightTask::validSensorAngleRange() const
{
    return _validSensorAngleRange;
}

void FlightTask::setValidSensorAngleRange(const AngleRange &vRange)
{
    _validSensorAngleRange = vRange;
    _clearBins();
}

qreal FlightTask::minSensingDistance() const
{
    return _minDistMeters;
}

void FlightTask::setMinSensingDistance(qreal minDist)
{
    _minDistMeters = qBound<qreal>(0.0, minDist, 100000);
    _clearBins();
}

qreal FlightTask::maxSensingDistance() const
{
    return _maxDistMeters;
}

void FlightTask::setMaxSensingDistance(qreal maxDist)
{
    _maxDistMeters = qBound<qreal>(0, maxDist, 100000);
    _clearBins();
}

void FlightTask::setSensingRange(qreal min, qreal max)
{
    this->setMinSensingDistance(min);
    this->setMaxSensingDistance(max);
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

//static
QString FlightTask::sensorType2String(FlightTask::SensorType type)
{
    QString toRet;
    if (type == DirectionalSensor)
        toRet = "Directional";
    else if (type == OmnidirectionalSensor)
        toRet = "Omnidirectional";
    else
        toRet = "Error";

    return toRet;
}

//static
FlightTask::SensorType FlightTask::string2SensorType(const QString &typeString)
{
    FlightTask::SensorType toRet;
    if (typeString.toLower() == "omnidirectional")
        toRet = OmnidirectionalSensor;
    else
        toRet = DirectionalSensor;

    return toRet;
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


void FlightTask::_calculateBins(const QPolygonF &geoPoly)
{
    Q_UNUSED(geoPoly)
}

void FlightTask::_clearBins()
{
    _bins.clear();
}
