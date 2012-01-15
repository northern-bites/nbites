
#include "FieldViewer.h"
#include <vector>
#include "man/memory/Memory.h"

namespace qtool {
namespace viewer {

using namespace data;
using namespace man::memory;

FieldViewer::FieldViewer(DataManager::ptr dataManager) :
		QMainWindow(),
		dataManager(dataManager),
		worldDataViewer(new WorldDataViewer()){
  
  paintField = new PaintField();

  // Adds the Field
  this->setCentralWidget(paintField);
  QDockWidget *dockWidget = new QDockWidget(tr("World Information"), this);
  dockWidget->setAllowedAreas(Qt::LeftDockWidgetArea |
                               Qt::RightDockWidgetArea);
  dockWidget->setWidget(worldDataViewer);
  addDockWidget(Qt::LeftDockWidgetArea, dockWidget);

}

}
}
