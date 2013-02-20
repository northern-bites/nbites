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

#include "data/Typedefs.h"

#include "man/memory/Memory.h"
#include "man/memory/MemoryCommon.h"
#include "data/treemodel/TreeModel.h"

namespace qtool {
namespace viewer {

class MObjectViewer : public QTreeView {

    Q_OBJECT
public:
    MObjectViewer(common::io::ProtobufMessage::const_ptr protoMessage,
                  QWidget* parent = 0);
    virtual ~MObjectViewer();

public slots:
    void updateView();

protected:
    void showEvent(QShowEvent* event);
    void paintEvent(QPaintEvent*);

private:
    void createNewTreeModel();

private:
    common::io::ProtobufMessage::const_ptr messageViewed;
    data::treemodel::TreeModel* treeModel;
    bool shouldRedraw;

};


}
}
