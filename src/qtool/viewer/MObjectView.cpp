
#include "MObjectView.h"

#include "data/treemodel/ProtoNode.h"

namespace qtool {
namespace viewer {

using data::MObject_ID;
using namespace data::treemodel;
using boost::shared_ptr;
using namespace man::memory;

MObjectView::MObjectView(shared_ptr<const ProtoMessage> messageViewed, QWidget* parent) :
        QTreeView(parent), messageViewed(messageViewed) {
    this->createNewTreeModel();
}

MObjectView::~MObjectView() {
    delete treeModel;
}

void MObjectView::createNewTreeModel() {
    ProtoNode* root = new ProtoNode(NULL, NULL,
                                    messageViewed.get());
    treeModel = new TreeModel(root);
    this->setModel(treeModel);
}

void MObjectView::update(MObject_ID) {
    delete treeModel;
    this->createNewTreeModel();
}

}
}
