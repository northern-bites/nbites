
#include <QtDebug>

#include "MObjectViewer.h"

#include "data/treemodel/ProtoNode.h"

namespace qtool {
namespace viewer {

using namespace data::treemodel;
using namespace man::memory;
using namespace common::io;

MObjectViewer::MObjectViewer(ProtobufMessage::const_ptr messageViewed, QWidget* parent) :
        QTreeView(parent), messageViewed(messageViewed) {
    this->createNewTreeModel();
}

MObjectViewer::~MObjectViewer() {
    delete treeModel;
}

void MObjectViewer::createNewTreeModel() {
    ProtoNode* root = new ProtoNode(NULL, NULL,
                                    messageViewed->getProtoMessage());
    treeModel = new TreeModel(root);
    this->setModel(treeModel);
}

void MObjectViewer::updateView() {
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

void MObjectViewer::showEvent(QShowEvent * e) {
    QTreeView::showEvent(e);
    //explicitely update the bitmap when the widget becomes visible again
    //since it might have changed!
    this->updateView();
}

void MObjectViewer::paintEvent(QPaintEvent* e) {
    QTreeView::paintEvent(e);
    shouldRedraw = true;
}

}
}
