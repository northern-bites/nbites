
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

      PF::ParticleSet updateParticles;
      proto::PLoc::Particle p;
      for(int i = 0; i < localizationStream->get()->particles_size(); ++i)
      {
	assert(i < localizationStream->get()->particles_size());
	p = localizationStream->get()->particles(i);
	//std::cout << i << " " << localizationStream->get()->particles_size() << "\n";
      	//std::cout << p.DebugString() << std::endl;
      	float x = p.x();
      	float y = p.y();
      	float h = p.h();
      	float w = p.w();
      	updateParticles.push_back(PF::LocalizationParticle(PF::Location(x, y, h), w));
      }

      paintLocalization->updateWithParticles(updateParticles);

      fieldView->updateView();

      // std::cout << "Updating localization... (" 
      // 		<< xEst << ", " 
      // 		<< yEst << ", "
      // 		<< hEst << ") "
      // 		<< std::endl;

      //std::cout << "Counted " << localizationStream->get()->particles_size()
      //<< "." << std::endl;
  }

}
}
