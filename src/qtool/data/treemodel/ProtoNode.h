/*
 * @class ProtoNode
 *
 * A class that provides a Node-type structure that can be used to represent
 * the structure of a protocol message in a tree-like way
 *
 * Any field of type message is a node which has all the fields of that message
 * as children nodes
 *
 * Any primitive (non-message) field is a leaf
 *
 * Any repeated field has all of its values represented as dummy leaf nodes;
 * These dummy leaf nodes contain no information; instead when they are accessed
 * they query their parent for a value
 *
 * Any repeated message has all of the messages it has as children (untested)
 *
 *
 *  Created on: Apr 2, 2011
 *      Author: oneamtu
 */

#ifndef ProtoNode_H_
#define ProtoNode_H_

#include <QList>
#include <string>
#include <google/protobuf/message.h>
#include <google/protobuf/descriptor.h>

#include "Node.h"

namespace qtool {
namespace data {
namespace treemodel {

namespace proto = google::protobuf;

class ProtoNode : public TreeModel::Node {

public:
    ProtoNode(ProtoNode* _parent,
              const proto::FieldDescriptor* _fieldDescriptor,
              const proto::Message* _message = NULL);
    virtual ~ProtoNode();

    ProtoNode* createNewSingleValueNode(const proto::FieldDescriptor* childFD);
    ProtoNode* createNewMessageNodeFromRepeatedAt(int index);
    ProtoNode* createNewSingularNodeFromRepeated();

    void constructTree();

    QVariant getData(int row, int column) const;
    int getNumColumns() const;
    bool isRepeated() const;
    bool isOfTypeMessage() const;
    int getSizeOfField() const;
    QVariant getName(int index) const;
    QVariant getValue(int index) const;
    //repeated fields can grow or shrink, so this method should
    //make sure that each node that represents a repeated field
    //has the valid number of children
    //it also recurses on children
    void revalidate();

    const proto::FieldDescriptor *getFieldDescriptor() const;
    const proto::Message *getMessage() const;

private:
    //constructs children from a message node
    void constructMessageChildren();
    //constructs children of a repeated field node;
    void constructRepeatedChildren();

    QVariant getRepeatedChildValue(int index) const;
    const proto::Message* getRepeatedMessageAt(int index) const;
    QVariant getSingleValueAt(int index) const;
    QVariant getSingleValue() const;

    //adds or substracts the necessary number of children to keep
    //the number of children the same as the number of values
    //in the repeated field the node represents
    void fixRepeatedSingularChildren();
    //clears this nodes' children and re-builds the subtree rooted
    //at this node
    void reconstructTree();

private:
    const static int NUM_DATA_COLUMNS = 2;

    const proto::FieldDescriptor* fieldDescriptor;
    const proto::Message* message;
};

}
}
}
#endif /* ProtoNode_H_ */
