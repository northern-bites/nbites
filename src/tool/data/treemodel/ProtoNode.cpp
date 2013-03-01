/*
 * ProtoNode.cpp
 *
 * @class ProtoNode
 *
 * A ProtoNode is either:
 * * a message node (usually root)
 * * a repeated field node
 * * a repeated field leaf
 * * a singular field leaf
 *
 *  Created on: Apr 2, 2011
 *      Author: oneamtu
 */

#include <iostream>
#include "ProtoNode.h"

namespace qtool {
namespace data {
namespace treemodel {

using namespace google::protobuf;

//TODO: split this into two constructors

ProtoNode::ProtoNode(ProtoNode* _parent,
          const FieldDescriptor* _fieldDescriptor,
          const Message* _message) :
    Node(_parent), fieldDescriptor(_fieldDescriptor), message(_message) {
    if (isOfTypeMessage() && message == NULL && !isRepeated()) {

        message = getMessageFromParentDescriptor();
    }
}

ProtoNode::~ProtoNode() {
}

ProtoNode* ProtoNode::createNewSingleValueNode(const FieldDescriptor* childFD) {
    return new ProtoNode(this, childFD);
}

ProtoNode* ProtoNode::createNewMessageNodeFromRepeatedAt(int index) {
    const Message* message = this->getRepeatedMessageAt(index);
    return new ProtoNode(this, NULL, message);
}

ProtoNode* ProtoNode::createNewSingularNodeFromRepeated() {
    return new ProtoNode(this, NULL);
}

void ProtoNode::constructTree() {

    if (this->isRepeated()) {
        this->constructRepeatedChildren();
    } else {
        if (this->isOfTypeMessage()) {
            this->constructMessageChildren();
        }
    }
}

void ProtoNode::constructMessageChildren() {

    const Descriptor* d = this->getMessage()->GetDescriptor();
    for (int i = 0; i < d->field_count(); i++) {

        const FieldDescriptor* childFD = d->field(i);
        ProtoNode* childNode = createNewSingleValueNode(childFD);
        this->addChild(childNode);

        if (childNode->isOfTypeMessage() || childNode->isRepeated()) {
            childNode->constructTree();
        }
    }
}

void ProtoNode::constructRepeatedChildren() {

    for (int i = 0; i < this->getSizeOfField(); i++) {
        ProtoNode* childNode;
        if (this->isOfTypeMessage()) {
            childNode = createNewMessageNodeFromRepeatedAt(i);
            childNode->constructTree();
        } else {
            childNode = createNewSingularNodeFromRepeated();
        }
        this->addChild(childNode);
    }
}

void ProtoNode::reconstructTree() {
    this->clearChildren();
    this->constructTree();
}

void ProtoNode::fixRepeatedSingularChildren() {
    int sizeChange = getSizeOfField() - children.size();
    if (sizeChange > 0) {
        for (int i = 0; i < sizeChange; i++) {
            this->addChild(createNewSingularNodeFromRepeated());
        }
    } else {
        for (int i = sizeChange; i < 0; i++) {
            this->popChild();
        }
    }
}

void ProtoNode::fixMessageField() {
    if (this->isOfTypeMessageField()) {
        message = getMessageFromParentDescriptor();
    }
}

void ProtoNode::revalidate() {
    if (this->isOfTypeRepeatedMessage()) {
        this->reconstructTree();
    }
    if (this->isOfTypeRepeatedSingular()) {
        this->fixRepeatedSingularChildren();
    }
    if (this->isOfTypeMessageField() && !this->isRepeated()){
        this->fixMessageField();
    }

    for (NodeList::iterator i = children.begin(); i != children.end(); i++) {
        (*i)->revalidate();
    }
}

int ProtoNode::getNumColumns() const {
    return NUM_DATA_COLUMNS;
}

bool ProtoNode::isRepeated() const {
    return fieldDescriptor && fieldDescriptor->is_repeated();
}

bool ProtoNode::isOfTypeRepeatedSingular() const {
    return this->isRepeated() && !this->isOfTypeMessageField();
}

bool ProtoNode::isOfTypeRepeatedMessage() const {
    return this->isRepeated() && this->isOfTypeMessageField();
}

bool ProtoNode::isOfTypeMessageField() const {
    return (fieldDescriptor &&
            fieldDescriptor->type() == FieldDescriptor::TYPE_MESSAGE);
}

bool ProtoNode::isOfTypeMessage() const {
    return this->isOfTypeMessageField() || message != NULL;
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

const Message* ProtoNode::getMessageFromParentDescriptor() const {
    const Message* parentM =
            reinterpret_cast<const ProtoNode*>(getParent())->getMessage();
    return &(parentM->GetReflection()->GetMessage(*parentM, fieldDescriptor));
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
                static_cast<qint64>(
                reflection->GetRepeatedInt64(*parentM, fieldDescriptor, index)));
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
                static_cast<quint64>(
                reflection->GetRepeatedUInt64(*parentM, fieldDescriptor, index)));
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
        return QVariant(static_cast<qint64>(
                reflection->GetInt64(*parentM, fieldDescriptor)));
    case FieldDescriptor::CPPTYPE_MESSAGE:
        return QVariant();
    case FieldDescriptor::CPPTYPE_STRING:
        //TODO: use GetStringReference
        return QVariant(reflection->GetString(*parentM, fieldDescriptor).data());
    case FieldDescriptor::CPPTYPE_UINT32:
        return QVariant(reflection->GetUInt32(*parentM, fieldDescriptor));
    case FieldDescriptor::CPPTYPE_UINT64:
        return QVariant(static_cast<qint64>(
                reflection->GetUInt64(*parentM, fieldDescriptor)));
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
}
}
