#include "ProtoViewer.h"
#include <QtDebug>
#include "treemodel/ProtoNode.h"

namespace tool {
namespace unlog {
namespace view {

ProtoViewer::ProtoViewer(const google::protobuf::Message* msg,
                         QWidget* parent) :
        QTreeView(parent),
        messageViewed(const_cast<google::protobuf::Message*>(msg))
{
    this->createNewTreeModel();
}

ProtoViewer::~ProtoViewer() {
    delete treeModel;
}

void ProtoViewer::createNewTreeModel() {
    ProtoNode* root = new ProtoNode(NULL, NULL,
                                    messageViewed);
    treeModel = new TreeModel(root);
    this->setModel(treeModel);
}

void ProtoViewer::updateView(const google::protobuf::Message* msg)
{
    messageViewed->CopyFrom(*msg);
    updateView();
}

void ProtoViewer::updateView() {
    if (this->isVisible() && shouldRedraw) {
        treeModel->revalidateModel();
        QModelIndex top = treeModel->index(0, 0, QModelIndex());
        QModelIndex bottom = treeModel->index(treeModel->rowCount(QModelIndex())-1,
                treeModel->columnCount(QModelIndex())-1, QModelIndex());
        shouldRedraw = false;
        emit dataChanged(top, bottom);
    }
    this->QTreeView::update();
}

void ProtoViewer::showEvent(QShowEvent * e) {
    QTreeView::showEvent(e);
    //explicitely update the bitmap when the widget becomes visible again
    //since it might have changed!
    this->updateView();
}

void ProtoViewer::paintEvent(QPaintEvent* e) {
    QTreeView::paintEvent(e);
    shouldRedraw = true;
}

}
}
}
