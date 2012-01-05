
#include "MObjectViewer.h"

#include "data/treemodel/ProtoNode.h"

namespace qtool {
namespace viewer {

using data::MObject_ID;
using namespace data::treemodel;
using boost::shared_ptr;
using namespace man::memory;

MObjectViewer::MObjectViewer(shared_ptr<const ProtoMessage> messageViewed, QWidget* parent) :
        QTreeView(parent), messageViewed(messageViewed) {
    this->createNewTreeModel();
}

MObjectViewer::~MObjectViewer() {
    delete treeModel;
}

void MObjectViewer::createNewTreeModel() {
    ProtoNode* root = new ProtoNode(NULL, NULL,
                                    messageViewed.get());
    treeModel = new TreeModel(root);
    this->setModel(treeModel);
}

void MObjectViewer::update(MObject_ID) {
    treeModel->revalidateModel();
    QModelIndex top = treeModel->index(0, 0, QModelIndex());
    QModelIndex bottom = treeModel->index(treeModel->rowCount(QModelIndex())-1,
            treeModel->columnCount(QModelIndex())-1, QModelIndex());
    emit dataChanged(top, bottom);
}

}
}
