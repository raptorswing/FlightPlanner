#ifndef SERIALIZABLE_H
#define SERIALIZABLE_H

#include <QString>
#include <QDataStream>

class Serializable
{
public:
    Serializable();
    virtual ~Serializable();

    virtual QString serializationType() const=0;
    virtual void serialize(QDataStream& stream) const=0;
};

#endif // SERIALIZABLE_H
