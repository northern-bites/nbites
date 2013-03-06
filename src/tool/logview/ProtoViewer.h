/**
 *
 * @class ProtoViewer
 *
 * A widget that shows the contents of a proto message in a tree view widget
 * using a TreeModel; it can subscribe to receive updates
 *
 * @author Octavian Neamtu
 *
 */


#pragma once

#include <QTreeView>
#include "treemodel/TreeModel.h"
#include <google/protobuf/message.h>

namespace tool {
namespace viewer {

class ProtoViewer : public QTreeView {
    Q_OBJECT;

public:
    ProtoViewer(google::protobuf::Message* msg, QWidget* parent = 0);
    virtual ~ProtoViewer();

public slots:
    void updateView();

protected:
    void showEvent(QShowEvent* event);
    void paintEvent(QPaintEvent*);

private:
    void createNewTreeModel();

private:
    google::protobuf::Message* messageViewed;
    TreeModel* treeModel;
    bool shouldRedraw;

};

}
}
