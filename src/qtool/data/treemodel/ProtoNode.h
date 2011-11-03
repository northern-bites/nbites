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
    ProtoNode* createNewMessageNodeFromRepeated(const proto::Message* message);
    ProtoNode* createNewSingularNodeFromRepeated();

    void constructTree();

    QVariant getData(int row, int column) const;
    int getNumColumns() const;
    bool isRepeated() const;
    bool isOfTypeMessage() const;
    int getSizeOfField() const;
    QVariant getName(int index) const;
    QVariant getValue(int index) const;
    //checks if repeated fields can grow or shrink, so this method should
    //make sure that everything is valid
    //Note: does NOT deal with repeated messages
    void revalidate();

    const proto::FieldDescriptor *getFieldDescriptor() const;
    const proto::Message *getMessage() const;

private:
    QList<ProtoNode*> constructMessageChildren();
    QList<ProtoNode*> constructRepeatedChildren();
    QVariant getRepeatedChildValue(int index) const;
    const proto::Message* getRepeatedMessageAt(int index) const;
    QVariant getSingleValueAt(int index) const;
    QVariant getSingleValue() const;

private:
    const static int NUM_DATA_COLUMNS = 2;

    const proto::FieldDescriptor* fieldDescriptor;
    const proto::Message* message;
};

}
}
}
#endif /* ProtoNode_H_ */
