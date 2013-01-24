#ifndef RRTNODE_H
#define RRTNODE_H

#include <QList>

#include "QVectorND.h"

class RRTNode
{
public:
    RRTNode(const QVectorND& pos,
            RRTNode * parent=0);

    const QVectorND& position() const;

    RRTNode * parent() const;

    const QList<RRTNode*>& children() const;
    void addChild(RRTNode* c);


private:
    QVectorND _pos;

    RRTNode * _parent;

    QList<RRTNode*> _children;
};

#endif // RRTNODE_H
