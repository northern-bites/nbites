#ifndef MESSAGEMODEL_H
#define MESSAGEMODEL_H

/**
 *
 * @author Octavian Neamtu
 *
 * @class MessageModel
 *
 * It is an implementation of QAbstractItemModel that handles a Google Protocol
 * Buffer message and displays its information accordingly.
 *
 * It envisions the Message as a tree with nodes at nested messages and repeated
 * fields (like vectors) and leaves at singular fields (like int, float, etc).
 *
 * See the following tutorial for a quick clarification:
 * http://doc.trolltech.com/4.3/itemviews-simpletreemodel.html
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

    // QAbstractItemModel overloaded functions - check out
    // http://doc.qt.nokia.com/latest/qmodelindex.html

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
    Message* message;

private:
    //utilitary function to get the corresponding FielDescriptor for a nested
    //Descriptor
    const google::protobuf::FieldDescriptor* getFieldByDescriptor(
            const google::protobuf::Descriptor* descriptor) const;

};
}
#endif // MESSAGEMODEL_H
