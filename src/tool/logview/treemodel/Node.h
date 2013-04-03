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

namespace tool {
namespace unlog {
namespace view {

class TreeModel::Node {

public:
    typedef QList <Node* > NodeList;

public:
    Node(const Node* parent);
    virtual ~Node();

    virtual void constructTree() = 0;
    virtual QVariant getData(int row, int column) const = 0;
    virtual int getNumColumns() const = 0;

    void addChild(Node* child);
    void popChild();
    void clearChildren();
    const Node* getChild(int index) const;
    virtual int childCount() const;

    const Node *getParent() const;

    int indexInParentList() const;
    int indexOfChild(const Node* child) const;

    //checks if anything has changed and tries to correct based on that
    virtual void revalidate() {}

protected:
    NodeList children;
    const Node* parent;
};

}
}
}

#endif /* Node_H_ */
