/*
 * ProtoNode.h
 *
 *  Created on: Apr 2, 2011
 *      Author: oneamtu
 */

#ifndef ProtoNode_H_
#define ProtoNode_H_

#include <QList>
#include <google/protobuf/message.h>
#include <google/protobuf/descriptor.h>

#include "Node.h"

namespace ProtoView {

namespace proto = google::protobuf;

class ProtoNode : public TreeModel::Node {



public:
    ProtoNode(Node* parent,
              const proto::FieldDescriptor* fd,
              const proto::Message* m = NULL);
    virtual ~ProtoNode();

    void constructTree();

    const proto::FieldDescriptor *getFieldDescriptor() const;
    const proto::Message *getMessage() const;

private:
    const proto::FieldDescriptor* fieldDescriptor;
    const proto::Message* message;
};

}
#endif /* ProtoNode_H_ */
