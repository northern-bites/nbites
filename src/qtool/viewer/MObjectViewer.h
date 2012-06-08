/**
 *
 * @class MObjectViewer
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

class MObjectViewer : public QTreeView {

    Q_OBJECT
public:
    MObjectViewer(boost::shared_ptr<const man::memory::ProtoMessage> messageViewed,
                QWidget* parent = 0);
    virtual ~MObjectViewer();

public slots:
    void updateView();

protected:
    void showEvent(QShowEvent* event);

private:
    void createNewTreeModel();

private:
    boost::shared_ptr<const man::memory::ProtoMessage> messageViewed;
    data::treemodel::TreeModel* treeModel;

};


}
}
