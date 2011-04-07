
/**
 *
 * Implementation of the TreeModel
 *
 * We use the Descriptor of the Message to layout the message and
 * the Reflection of the message to access the data.
 *
 * The main descriptor of the message is the root node, and all of the
 * model indices are FieldDescriptors.
 *
 * Used this following tutorial as a guide
 * http://doc.trolltech.com/4.3/itemviews-simpletreemodel.html
 *
 */

#include <QtGui>

#include "TreeModel.h"
#include <google/protobuf/descriptor.h>
#include <iostream>

namespace ProtoView {

using namespace google::protobuf;
using namespace std;

const static uint32 NUM_DATA_COLS = 2;

TreeModel::TreeModel(Node* _root, QObject *_parent) :
    QAbstractItemModel(_parent),
    root(_root)
{
    root->constructTree();
}

TreeModel::~TreeModel() {}

int TreeModel::rowCount(const QModelIndex &index) const {

    // we expect index.column() to be 0 (see TreeModel::parent )
    if (index.column() > 0) {
        return 0;
    }

    const Node* parentNode = NULL;
    if (!index.isValid()) {
        //root element
        parentNode = root;
    } else {
        //means we have a non-root item - safe to typecast it
        parentNode =
                static_cast<const Node*> (index.internalPointer());
    }

    if (parentNode) {
        return parentNode->childCount();
    } else {
        return 0;
    }
}

int TreeModel::columnCount(const QModelIndex &parent) const {
    return NUM_DATA_COLS;
}

QModelIndex TreeModel::index(int row, int column,
                                const QModelIndex &parent) const {

    if (!hasIndex(row, column, parent)) {
        return QModelIndex();
    }

    const Node* parentNode = NULL;
    if (!parent.isValid()) {
        //root element
        parentNode = root;
    } else {
        //means we have a non-root item - safe to typecast it
        parentNode =
                static_cast<const Node*> (parent.internalPointer());
    }

    if (parentNode) {
        const Node* childNode = parentNode->getChild(row);
        return createIndex(row, column, (void*) childNode );
    } else {
        return QModelIndex();
    }

}

QModelIndex TreeModel::parent(const QModelIndex &child) const {

    if (!child.isValid()) {
        return QModelIndex();
    }

    const Node* childNode =
            static_cast<const Node*> (child.internalPointer());
    const Node* parentNode =childNode->getParent();

    if (!parentNode) {
        return QModelIndex();
    }
    // we don't return a parent if the parent is the root item
    if (parentNode == root) {
        return QModelIndex();
    }

    return createIndex(
            parentNode->indexInParentList(), 0, (void *) parentNode);
}

QVariant TreeModel::data(const QModelIndex &index, int role) const {
    if (!index.isValid())
        return QVariant();

    if (role != Qt::DisplayRole)
        return QVariant();

    return QVariant("blank");

//    FieldDescriptor* thisField =
//            static_cast<FieldDescriptor*> (index.internalPointer());
//    FieldDescriptor* parentField =
//            static_cast<FieldDescriptor*> (index.parent().internalPointer());
//
//    switch(index.column()) {
//    case 0 :
//        return QVariant(QString(thisField->name().data()));
//    case 1 :
////        if (parentField->type() == FieldDescriptor::TYPE_MESSAGE) {
////            //Message* message = getMessageFromFieldDescriptor(parentField);
////        } else {
//            Message* message = this->message;
////        }
//
////        if (thisField->is_repeated()) {
////            return getSingularDataFromFieldDescriptor(message, thisField);
//        } else {
//
//        }
        return QVariant();

}

QVariant TreeModel::headerData(int section, Qt::Orientation orientation,
                                  int role) const {
    if (orientation == Qt::Horizontal && role == Qt::DisplayRole)
        return QVariant();

    return QVariant();
}

Qt::ItemFlags TreeModel::flags(const QModelIndex &index) const {
    if (!index.isValid())
        return 0;

    return Qt::ItemIsEnabled | Qt::ItemIsSelectable;
}

//QVariant TreeModel::getSingularDataFromFieldDescriptor(
//        const google::protobuf::Message* message,
//        const google::protobuf::FieldDescriptor* fieldDescriptor) const {
//
//    const Reflection* reflection = message->GetReflection();
//    switch (fieldDescriptor->cpp_type()) {
//    case FieldDescriptor::CPPTYPE_BOOL:
//        return QVariant(reflection->GetBool(*message, fieldDescriptor));
//    case FieldDescriptor::CPPTYPE_DOUBLE:
//        return QVariant(reflection->GetDouble(*message, fieldDescriptor));
//    case FieldDescriptor::CPPTYPE_FLOAT:
//        return QVariant(reflection->GetFloat(*message, fieldDescriptor));
//    case FieldDescriptor::CPPTYPE_INT32:
//        return QVariant(reflection->GetInt32(*message, fieldDescriptor));
//    case FieldDescriptor::CPPTYPE_INT64:
//        return QVariant(reflection->GetInt64(*message, fieldDescriptor));
//    case FieldDescriptor::CPPTYPE_MESSAGE:
//        return QVariant();
//    case FieldDescriptor::CPPTYPE_STRING:
//        return QVariant(reflection->GetString(*message, fieldDescriptor).data());
//    case FieldDescriptor::CPPTYPE_UINT32:
//        return QVariant(reflection->GetUInt32(*message, fieldDescriptor));
//    case FieldDescriptor::CPPTYPE_UINT64:
//        return QVariant(reflection->GetUInt64(*message, fieldDescriptor));
//    default:
//        return QVariant();
//    }
//}

}
