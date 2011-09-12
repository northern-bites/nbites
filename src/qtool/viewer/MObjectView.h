/**
 *
 * @class MObjectView
 *
 * A widget that shows the contents of a proto message in a tree view widget
 * using a TreeModel; it can subscribe to receive updates
 *
 * @author Octavian Neamtu
 *
 */


#pragma once

#include <boost/shared_ptr.hpp>
#include <QTreeView>
#include "data/DataTypes.h"
#include "man/memory/Memory.h"
#include "data/treemodel/TreeModel.h"

namespace qtool {
namespace viewer {

class MObjectView : public QTreeView, public Subscriber<data::MObject_ID> {

public:
    MObjectView(boost::shared_ptr<const man::memory::ProtoMessage> messageViewed,
                QWidget* parent = 0);
    virtual ~MObjectView();

    void update(data::MObject_ID);

private:
    void createNewTreeModel();

private:
    boost::shared_ptr<const man::memory::ProtoMessage> messageViewed;
    data::treemodel::TreeModel* treeModel;

};


}
}
