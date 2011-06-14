/*
 * ProtoNode.cpp
 *
 * @class ProtoNode
 *
 * A ProtoNode is either:
 * * a repeated field node
 * * a message node
 * * a repeated field leaf
 * * a singular field leaf
 *
 *  Created on: Apr 2, 2011
 *      Author: oneamtu
 */

#include <iostream>
#include "ProtoNode.h"

namespace ProtoView {

using namespace google::protobuf;

ProtoNode::ProtoNode(ProtoNode* _parent,
          const FieldDescriptor* _fieldDescriptor,
          const Message* _message) :
    Node(_parent), fieldDescriptor(_fieldDescriptor), message(_message) {
    if (isMessage() && message == NULL && !isRepeated()) {

        const Message* parentM = _parent->getMessage();
        message = &(parentM->GetReflection()->GetMessage(*parentM, fieldDescriptor));
    }
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

        if (parent->isRepeated()) {
            nodes.append(parent->constructRepeatedChildren());
        } else {
            if (parent->isMessage()) {
                nodes.append(parent->constructMessageChildren());
            }
        }
    }
}

QList<ProtoNode*> ProtoNode::constructMessageChildren() {

    QList<ProtoNode*> nodes;
    const Descriptor* d = this->getMessage()->GetDescriptor();
    for (int i = 0; i < d->field_count(); i++) {

        const FieldDescriptor* childFD = d->field(i);
        ProtoNode* childNode = new ProtoNode(this, childFD);
        this->addChild(childNode);

        if (childNode->isMessage() || childNode->isRepeated()) {
            nodes.push_back(childNode);
        }
    }
    return nodes;
}

QList<ProtoNode*> ProtoNode::constructRepeatedChildren() {
    QList<ProtoNode*> nodes;
    for (int i = 0; i < this->getSizeOfField(); i++) {
        ProtoNode* childNode;
        if (this->isMessage()) {
            const Message* message = this->getRepeatedMessageAt(i);
            childNode = new ProtoNode(this, NULL, message);
            nodes.append(childNode);
        } else {
            childNode = new ProtoNode(this, NULL);
        }
        this->addChild(childNode);
    }
    return nodes;
}

int ProtoNode::getNumColumns() const {
    return NUM_DATA_COLUMNS;
}

int ProtoNode::childCount() const {
    if (isRepeated()) {
        return getSizeOfField();
    } else {
        return Node::childCount();
    }
}

bool ProtoNode::isRepeated() const {
    return fieldDescriptor && fieldDescriptor->is_repeated();
}

bool ProtoNode::isMessage() const {
    return (fieldDescriptor &&
           fieldDescriptor->type() == FieldDescriptor::TYPE_MESSAGE)
           || message != NULL;
}

int ProtoNode::getSizeOfField() const {
    const ProtoNode* parent =
            static_cast<const ProtoNode *> (this->getParent());
    const Message* parentM = parent->getMessage();
    if (!parentM) {
        return 0;
    }

    const Reflection* reflection = parentM->GetReflection();
    return reflection->FieldSize(*parentM, fieldDescriptor);

}

QVariant ProtoNode::getData(int row, int column) const {
    switch(column) {
    case 0 :
        return getName(row);
    case 1 :
        return getValue(row);
    default :
        return QVariant();
    }
}

QVariant ProtoNode::getName(int index) const {
    if (fieldDescriptor != NULL) {
        return QVariant(fieldDescriptor->name().data());
    } else {
        return index;
    }
}

QVariant ProtoNode::getValue(int index) const {

    if (fieldDescriptor == NULL) {
        return getRepeatedChildValue(index);
    }
    if (!isRepeated()) {
        return getSingleValue();
    }
    return QVariant();
}

QVariant ProtoNode::getRepeatedChildValue(int index) const {
    const ProtoNode* parent =
                    static_cast<const ProtoNode *>(this->getParent());
            return parent->getSingleValueAt(index);
}

const Message* ProtoNode::getRepeatedMessageAt(int index) const {
    if (!this->isRepeated()) {
        return NULL;
    }

    const ProtoNode* parent =
            static_cast<const ProtoNode *> (this->getParent());
    const Message* parentM = parent->getMessage();
    if (!parentM) {
        return NULL;
    }

    const Reflection* reflection = parentM->GetReflection();
    return &(reflection->GetRepeatedMessage(*parentM, fieldDescriptor, index));
}

QVariant ProtoNode::getSingleValueAt(int index) const {

    if (!this->isRepeated()) {
        return QVariant();
    }

    const ProtoNode* parent = static_cast<const ProtoNode *>(this->getParent());
    const Message* parentM = parent->getMessage();
    if (!parentM) {
        return QVariant();
    }

    const Reflection* reflection = parentM->GetReflection();
    switch (fieldDescriptor->cpp_type()) {
    case FieldDescriptor::CPPTYPE_BOOL:
        return QVariant(reflection->GetRepeatedBool(*parentM, fieldDescriptor,
                index));
    case FieldDescriptor::CPPTYPE_DOUBLE:
        return QVariant(
                reflection->GetRepeatedDouble(*parentM, fieldDescriptor, index));
    case FieldDescriptor::CPPTYPE_FLOAT:
        return QVariant(
                reflection->GetRepeatedFloat(*parentM, fieldDescriptor, index));
    case FieldDescriptor::CPPTYPE_INT32:
        return QVariant(
                reflection->GetRepeatedInt32(*parentM, fieldDescriptor, index));
    case FieldDescriptor::CPPTYPE_INT64:
        return QVariant(
                reflection->GetRepeatedInt64(*parentM, fieldDescriptor, index));
    case FieldDescriptor::CPPTYPE_MESSAGE:
        return QVariant();
    case FieldDescriptor::CPPTYPE_STRING:
        //TODO: use the GetRepeatedStringReference
        return QVariant(
                reflection->GetRepeatedString(*parentM, fieldDescriptor,
                index).data());
    case FieldDescriptor::CPPTYPE_UINT32:
        return QVariant(
                reflection->GetRepeatedUInt32(*parentM, fieldDescriptor, index));
    case FieldDescriptor::CPPTYPE_UINT64:
        return QVariant(
                reflection->GetRepeatedUInt64(*parentM, fieldDescriptor, index));
    default:
        return QVariant();
    }

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
        //TODO: use GetStringReference
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
