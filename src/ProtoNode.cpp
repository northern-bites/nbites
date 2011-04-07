/*
 * MessageNode.cpp
 *
 *  Created on: Apr 2, 2011
 *      Author: oneamtu
 */

#include <iostream>
#include "ProtoNode.h"

namespace ProtoView {

using namespace google::protobuf;

//TODO: rename
ProtoNode::ProtoNode(Node* parent, const FieldDescriptor* fd, const Message* m) :
    Node(parent), fieldDescriptor(fd), message(m) {
}

ProtoNode::~ProtoNode() {
}

void ProtoNode::constructTree() {

    //do a breadth-first search through all the descriptors
    QList<ProtoNode*> nodes;
    //add the root
    nodes.push_back(this);

    while (!nodes.empty()) {
        ProtoNode* parent = nodes.front();
        nodes.pop_front();

        const Descriptor* d = parent->getMessage()->GetDescriptor();
        for (int i = 0; i < d->field_count(); i++) {
            const FieldDescriptor* childFD = d->field(i);
            if (childFD->type() == FieldDescriptor::TYPE_MESSAGE) {

                const Message* parentM = parent->getMessage();
                const Message* childM =
                        &(parentM->GetReflection()->
                                GetMessage(*parentM, childFD));

                ProtoNode* childNode = new ProtoNode(parent, childFD, childM);
                parent->addChild(childNode);
                nodes.push_back(childNode);

            } else {
                ProtoNode* childNode = new ProtoNode(parent, childFD);
                parent->addChild(childNode);
            }
        }
    }
}

const FieldDescriptor* ProtoNode::getFieldDescriptor() const {
    return fieldDescriptor;
}

const Message* ProtoNode::getMessage() const {
    return message;
}

}
