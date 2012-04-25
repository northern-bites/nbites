
#include "FieldViewer.h"
#include <vector>
#include "man/memory/Memory.h"

namespace qtool {
namespace viewer {

using namespace data;
using namespace man::memory;
using namespace image;

FieldViewer::FieldViewer(DataManager::ptr dataManager) :
        QMainWindow(), dataManager(dataManager) {

  dataManager->connectSlotToMObject(this,
				    SLOT( updateLocalization() ),
				    man::memory::MLOCALIZATION_ID);

  localizationStream = dataManager->getMemory()->getMLocalization();

    paintField = new PaintField(this);
    paintLocalization = new PaintLocalization(this);
    particlesOnField = new OverlayedImage(paintField, paintLocalization, this);
    fieldView = new BMPImageViewer(particlesOnField, this);

    // Adds the Field
    this->setCentralWidget(fieldView);

    fieldView->updateView();
}

  void FieldViewer::updateLocalization()
  {
      float xEst = localizationStream->get()->x_est();
      float yEst = localizationStream->get()->y_est();
      float hEst = localizationStream->get()->h_est();
      std::cout << "Updating localization... (" 
		<< xEst << ", " 
		<< yEst << ", "
		<< hEst << ") "
		<< std::endl;

      std::cout << "Counted " << localizationStream->get()->particles_size()
		<< "." << std::endl;
  }

}
}
