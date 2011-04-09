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

ProtoNode::ProtoNode(Node* _parent,
          const FieldDescriptor* _fieldDescriptor,
          const Message* _message) :
    Node(_parent), fieldDescriptor(_fieldDescriptor), message(_message) {
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

int ProtoNode::getNumColumns() const {
    return NUM_DATA_COLUMNS;
}

QVariant ProtoNode::getData(int column) const {
    switch(column) {
    case 0 :
        return getName();
    case 1 :
        return getValue();
    default :
        return QVariant();
    }
}

QVariant ProtoNode::getName() const {
    return QVariant(fieldDescriptor->name().data());
}

QVariant ProtoNode::getValue() const {

    if (!fieldDescriptor->is_repeated()) {
        return getSingleValue();
    }
    return QVariant();
}

QVariant ProtoNode::getSingleValue() const {
    const ProtoNode* parent = static_cast<const ProtoNode *>(this->getParent());
    const Message* parentM = parent->getMessage();
    if (!parentM) {
        return QVariant();
    }

    const Reflection* reflection = parentM->GetReflection();
    switch (fieldDescriptor->cpp_type()) {
    case FieldDescriptor::CPPTYPE_BOOL:
        return QVariant(reflection->GetBool(*parentM, fieldDescriptor));
    case FieldDescriptor::CPPTYPE_DOUBLE:
        return QVariant(reflection->GetDouble(*parentM, fieldDescriptor));
    case FieldDescriptor::CPPTYPE_FLOAT:
        return QVariant(reflection->GetFloat(*parentM, fieldDescriptor));
    case FieldDescriptor::CPPTYPE_INT32:
        return QVariant(reflection->GetInt32(*parentM, fieldDescriptor));
    case FieldDescriptor::CPPTYPE_INT64:
        return QVariant(reflection->GetInt64(*parentM, fieldDescriptor));
    case FieldDescriptor::CPPTYPE_MESSAGE:
        return QVariant();
    case FieldDescriptor::CPPTYPE_STRING:
        return QVariant(reflection->GetString(*parentM, fieldDescriptor).data());
    case FieldDescriptor::CPPTYPE_UINT32:
        return QVariant(reflection->GetUInt32(*parentM, fieldDescriptor));
    case FieldDescriptor::CPPTYPE_UINT64:
        return QVariant(reflection->GetUInt64(*parentM, fieldDescriptor));
    default:
        return QVariant();
    }
}

const FieldDescriptor* ProtoNode::getFieldDescriptor() const {
    return fieldDescriptor;
}

const Message* ProtoNode::getMessage() const {
    return message;
}

}
