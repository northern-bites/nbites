
/**
 *
 * Implementation of the MessageModel
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

#include "MessageModel.h"
#include <google/protobuf/descriptor.h>
#include <iostream>

namespace ProtoView {

using namespace google::protobuf;
using namespace std;

const static uint32 NUM_DATA_COLS = 2;

MessageModel::MessageModel(Message* message, QObject *parent) :
    QAbstractItemModel(parent),
    message(message)
{
}

MessageModel::~MessageModel() {}

int MessageModel::rowCount(const QModelIndex &index) const {

    // we expect index.column() to be 0 (see MessageModel::parent )
    if (index.column() > 0) {
        return 0;
    }

    const Descriptor* indexDescriptor = NULL;
    if (!index.isValid()) {
        //means we have the root item
        indexDescriptor = message->GetDescriptor();
    } else {
        //means we have a non-root item - safe to typecast it
        const FieldDescriptor* thisField =
                static_cast<const FieldDescriptor*> (index.internalPointer());

        if (thisField->type() == FieldDescriptor::TYPE_MESSAGE) {
            indexDescriptor = thisField->message_type();
        }
    }

    if (indexDescriptor) {
        return indexDescriptor->field_count();
    } else {
        return 0;
    }
}

int MessageModel::columnCount(const QModelIndex &parent) const {
    return NUM_DATA_COLS;
}

QModelIndex MessageModel::index(int row, int column,
                                const QModelIndex &index) const {

    if (!hasIndex(row, column, index)) {
        return QModelIndex();
    }

    const Descriptor* indexDescriptor = NULL;
    if (!index.isValid()) {
        //means it just started constructing the index and there is no index
        //for this element
        indexDescriptor = message->GetDescriptor();
    } else {
        //means we have a non-root item - safe to typecast it
        const FieldDescriptor* thisField =
                static_cast<const FieldDescriptor*> (index.internalPointer());

        if (thisField->type() == FieldDescriptor::TYPE_MESSAGE) {
            indexDescriptor = thisField->message_type();
        }
    }

    if (indexDescriptor) {
        if ( 0 <= row && row < indexDescriptor->field_count()) {
            return createIndex(row, column, (void*) indexDescriptor->field(row));
        }
    } else {
        return QModelIndex();
    }

}

const FieldDescriptor* MessageModel::getFieldByDescriptor(
        const Descriptor* descriptor) const {

    //do a breadth-first search through the whole descriptor tree
    QList <const Descriptor*> descriptors;
    //add the root
    descriptors.push_back(message->GetDescriptor());

    while (!descriptors.empty()) {
        const Descriptor* d = descriptors.front();
        descriptors.pop_front();
        for (int i = 0; i < d->field_count(); i++ ) {
            const FieldDescriptor* childFD = d->field(i);
            if (childFD->type() == FieldDescriptor::TYPE_MESSAGE) {
                const Descriptor* childD = childFD->message_type();
                if (childD == descriptor) {
                    return childFD;
                }
                descriptors.push_back(childD);
            }
        }
    }
    return NULL;
}

QModelIndex MessageModel::parent(const QModelIndex &index) const {

    if (!index.isValid()) {
        return QModelIndex();
    }

    FieldDescriptor* thisField =
            static_cast<FieldDescriptor*> (index.internalPointer());
    const Descriptor* parentDescriptor = thisField->containing_type();

    if (!parentDescriptor) {
        return QModelIndex();
    }
    // we don't return a parent if the parent is the root item
    if (parentDescriptor == message->GetDescriptor()) {
        return QModelIndex();
    }

    const FieldDescriptor* parentField = getFieldByDescriptor(parentDescriptor);

    if (parentField) {
        return createIndex(parentField->index(), 0, (void *) parentField);
    } else {
        return QModelIndex();
    }
}

QVariant MessageModel::data(const QModelIndex &index, int role) const {
    if (!index.isValid())
        return QVariant();

    if (role != Qt::DisplayRole)
        return QVariant();

    FieldDescriptor* thisField =
                static_cast<FieldDescriptor*> (index.internalPointer());

    if (index.column() == 0) {
        return QVariant(QString(thisField->name().data()));
    }
    if (index.column() == 1) {
        return QVariant(QString(thisField->name().data()));
    }
}

QVariant MessageModel::headerData(int section, Qt::Orientation orientation,
                                  int role) const {
    if (orientation == Qt::Horizontal && role == Qt::DisplayRole)
        return QVariant("Message");

    return QVariant();
}

Qt::ItemFlags MessageModel::flags(const QModelIndex &index) const {
    if (!index.isValid())
        return 0;

    return Qt::ItemIsEnabled | Qt::ItemIsSelectable;
}

}
