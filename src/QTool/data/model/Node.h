/*
 * Node.h
 *
 *  Created on: Apr 2, 2011
 *      Author: oneamtu
 */

#ifndef Node_H_
#define Node_H_

#include <QList>

#include "TreeModel.h"

namespace ProtoView {

class TreeModel::Node {
public:
    Node(const Node* parent);
    virtual ~Node();

    virtual void constructTree() = 0;
    virtual QVariant getData(int row, int column) const = 0;
    virtual int getNumColumns() const = 0;

    void addChild(const Node* child);
    const Node* getChild(int index) const;
    virtual int childCount() const;

    const Node *getParent() const;

    int indexInParentList() const;
    int indexOfChild(const Node* child) const;

private:
    QList <const Node* > children;
    const Node* parent;
};

}
#endif /* Node_H_ */
