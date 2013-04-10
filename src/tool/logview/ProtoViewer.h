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
#include "RoboGrams.h"

namespace tool {
namespace logview {

class ProtoViewer : public QTreeView {
    Q_OBJECT;

public:
    ProtoViewer(QWidget* parent = 0);
    virtual ~ProtoViewer();

protected:
    void showEvent(QShowEvent* event);
    void paintEvent(QPaintEvent*);
    void updateView();
    void createNewTreeModel();

    google::protobuf::Message* messageViewed;
    TreeModel* treeModel;
    bool shouldRedraw;
};

template<class T>
class TypedProtoViewer : public ProtoViewer, public portals::Module
{
public:
    TypedProtoViewer(QWidget* parent = 0) : ProtoViewer(parent)
    {
        messageViewed = new T();
        createNewTreeModel();
    }

    portals::InPortal<T> input;

protected:
    virtual void run_()
    {
        input.latch();
        messageViewed->CopyFrom(input.message());
        updateView();
    }
};

}
}
