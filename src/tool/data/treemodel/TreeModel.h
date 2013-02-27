#ifndef TreeModel_H
#define TreeModel_H

/**
 *
 * @author Octavian Neamtu
 *
 * @class TreeModel
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

namespace qtool {
namespace data {
namespace treemodel {

class TreeModel : public QAbstractItemModel
{
    Q_OBJECT
public:
    class Node;
    //the TreeModel takes ownership of the Node* and will delete it at
    // destruction
    TreeModel(Node* root, QObject *parent = 0);
    ~TreeModel();

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

    const Node* getRoot() const {return root;}

    void revalidateModel();

signals:

public slots:

private:
    Node* root;

};
}
}
}
#include "Node.h"

#endif // TreeModel_H
