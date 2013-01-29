#ifndef QKDTREE_H
#define QKDTREE_H

#include "QKDTree_global.h"

#include "QKDTreeNode.h"
#include "QKDTreeDistanceMetric.h"
#include "QVectorND.h"

class QKDTREESHARED_EXPORT QKDTree
{
public:
    /**
     * @brief QKDTree constructs a kd-tree that takes positions of the given dimension.
     * e.g., to store 2d points, set dimension = 2.
     * @param dimension
     */
    QKDTree(uint dimension);
    ~QKDTree();

    /**
     * @brief dimension Returns the dimensionality of the positions stored by this tree.
     * @return
     */
    uint dimension() const;

    /**
     * @brief size Returns the number of key/value pairs in the tree
     * @return
     */
    qint64 size() const;

    bool add(QKDTreeNode * node, QString * resultOut = 0);
    bool add(const QVectorND& position, const QVariant& value, QString * resultOut = 0);
    bool add(const QPointF& position, const QVariant& value, QString * resultOut = 0);

    bool nearest(const QVectorND& position, QKDTreeNode * output, QString * resultOut = 0);
    bool nearest(QKDTreeNode * node, QKDTreeNode * output, QString * resultOut = 0);

    bool nearest(const QVectorND& position, QVectorND * output, QString * resultOut = 0);

    bool contains(const QVectorND& position);
    bool contains(QKDTreeNode * node);

    bool setDistanceMetric(QKDTreeDistanceMetric * nMetric, QString * resultOut = 0);
    QKDTreeDistanceMetric * distanceMetric() const;

    /**
     * @brief debugPrint does a breadth-first search of the tree, printing values as it goes.
     */
    void debugPrint();

private:
    uint _dimension;
    qint64 _size;

    QKDTreeNode * _root;

    QKDTreeDistanceMetric * _distanceMetric;
};

#endif // QKDTREE_H
