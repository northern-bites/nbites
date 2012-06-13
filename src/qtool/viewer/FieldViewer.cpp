#include "FieldViewer.h"
#include "man/memory/Memory.h"
#include "image/BMPImage.h"

#include <QDebug>
#include <vector>
#include "man/memory/Memory.h"

namespace qtool {
namespace viewer {

using namespace data;
using namespace man::memory;
using namespace image;
FieldViewer::FieldViewer(DataManager::ptr dataManager) :
        QMainWindow(), dataManager(dataManager) {

  dataManager->connectSlot(this,
                    SLOT( updateLocalization() ),
                    "MLocalization");

  localizationStream = dataManager->getMemory()->get<MLocalization>();

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
      float xEst = localizationStream->get()->location().x();
      float yEst = localizationStream->get()->location().y();
      float hEst = localizationStream->get()->location().h();


      // UNCOMMENT to attempt drawing of particle swarm
      PF::ParticleSet updateParticles;
      proto::PLoc::Particle p;
      for(int i = 0; i < localizationStream->get()->particles_size(); ++i)
      {
          try {
              p = localizationStream->get()->particles(i);
          } catch (google::protobuf::FatalException& e) {
              std::cout << "you suck" << std::endl;
          }
          //std::cout << i << " " << localizationStream->get()->particles_size() << "\n";
          //std::cout << p.DebugString() << std::endl;
          float x = p.x();
          float y = p.y();
          float h = p.h();
          float w = p.w();
          updateParticles.push_back(PF::LocalizationParticle(PF::Location(x, y, h), w));
      }

//      std::cout << "Updating " << localizationStream->get()->particles_size() << " particles." << std::endl;

      paintLocalization->updateWithParticles(updateParticles);


      paintLocalization->updateEstimates(xEst, yEst, hEst);

      fieldView->updateView();

      // std::cout << "Updating localization... ("
      //        << xEst << ", "
      //        << yEst << ", "
      //        << hEst << ") "
      //        << std::endl;

      //std::cout << "Counted " << localizationStream->get()->particles_size()
      //<< "." << std::endl;
  }

}
}
