/*
 * ProtoNode.h
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

    void constructTree();

    QVariant getData(int row, int column) const;
    int getNumColumns() const;
    int childCount() const;
    bool isRepeated() const;
    bool isOfTypeMessage() const;
    int getSizeOfField() const;
    QVariant getName(int index) const;
    QVariant getValue(int index) const;

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
