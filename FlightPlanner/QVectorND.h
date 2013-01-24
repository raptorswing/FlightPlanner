#ifndef QVECTORND_H
#define QVECTORND_H

#include <QList>
#include <QPoint>
#include <QPointF>
#include <QVector2D>
#include <QVector3D>
#include <QVector4D>
#include <QtDebug>

class QVectorND
{
public:
    QVectorND(int dimensions);
    QVectorND(const QList<qreal>& values);
    QVectorND(const QPoint& point);
    QVectorND(const QPointF& point);
    QVectorND(const QVector2D& vec);
    QVectorND(const QVector3D& vec);
    QVectorND(const QVector4D& vec);
    QVectorND(const QVectorND& other);

    int dimension() const;

    bool isNull() const;
    qreal length() const;
    qreal lengthSquared() const;

    void normalize();
    QVectorND normalized() const;

    void setVal(int index, qreal value);
    qreal val(int index) const;
    const QList<qreal>& values() const;


    QVectorND& operator*= (qreal factor);
    QVectorND& operator*= (const QVectorND& other);
    QVectorND& operator+= (const QVectorND& other);
    QVectorND& operator-= (const QVectorND& other);
    QVectorND& operator/= (qreal divisor);

    bool operator==(const QVectorND& other) const;
    bool operator!=(const QVectorND& other) const;

    qreal& operator[](int index);
    qreal operator[](int index) const;

private:
    int _dimensions;
    QList<qreal> _data;
};

//non-members
uint qHash(const QVectorND& vec);
QDebug operator<<(QDebug dbg, const QVectorND& vec);
const QVectorND operator-(const QVectorND& v1, const QVectorND& v2);
const QVectorND operator-(const QVectorND& v);

#endif // QVECTORND_H
