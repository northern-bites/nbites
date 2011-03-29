
#include <QtGui>

#include "MessageModel.h"

namespace ProtoView {

MessageModel::MessageModel(Message* message, QObject *parent) :
    QAbstractItemModel(parent),
    _message(message)
{
}

MessageModel::~MessageModel() {}

int MessageModel::rowCount(const QModelIndex &parent) const {
    return 0;
}

int MessageModel::columnCount(const QModelIndex &parent) const {
    return 0;
}

QModelIndex MessageModel::index(int row, int column, const QModelIndex &parent) const {

}

QModelIndex MessageModel::parent(const QModelIndex &index) const {

    //if (!index->isValid)
}

QVariant MessageModel::data(const QModelIndex &index, int role) const {

}

QVariant MessageModel::headerData(int section, Qt::Orientation orientation, int role) const {

}

Qt::ItemFlags MessageModel::flags(const QModelIndex &index) const {

}

}
