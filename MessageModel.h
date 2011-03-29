#ifndef MESSAGEMODEL_H
#define MESSAGEMODEL_H

/**
 *
 *
 */

#include <QAbstractItemModel>
#include <QModelIndex>
#include <QVariant>
#include <google/protobuf/message.h>

namespace ProtoView {

    using ::google::protobuf::Message;

class MessageModel : public QAbstractItemModel
{
    Q_OBJECT
public:
    MessageModel(Message* message, QObject *parent = 0);
    ~MessageModel();

    QVariant data(const QModelIndex &index, int role) const;
    Qt::ItemFlags flags(const QModelIndex &index) const;
    QVariant headerData(int section, Qt::Orientation orientation,
                        int role = Qt::DisplayRole) const;
    QModelIndex index(int row, int column,
                      const QModelIndex &parent = QModelIndex()) const;
    QModelIndex parent(const QModelIndex &index) const;
    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    int columnCount(const QModelIndex &parent = QModelIndex()) const;

signals:

public slots:

private:
    Message* _message;
    void* root; //dummy pointer

};
}
#endif // MESSAGEMODEL_H
