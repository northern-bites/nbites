#include "LocalizationModule.h"

namespace man
{
    namespace localization
    {
    LocalizationModule::LocalizationModule()
        : portals::Module(),
          output(base())
    {
        motionModel = boost::shared_ptr<MotionSystem>(new MotionSystem());
        visionModel = boost::shared_ptr<VisionSystem>(new VisionSystem());

        particleFilter = boost::shared_ptr<ParticleFilter>(new ParticleFilter(motionModel,
                                                                              visionModel));
        particleFilter->resetLocTo(100,100,0);
        std::cout << particleFilter->getXEst() << "\t"
                  << particleFilter->getYEst() << "\t"
                  << particleFilter->getHEst() << "\n";
    }

    LocalizationModule::~LocalizationModule()
    {
    }

    void LocalizationModule::update()
    {
        motionInput.latch();
        visionInput.latch();

        messages::RobotLocation jokeOutput;
        jokeOutput.set_x(1);
        jokeOutput.set_y(2);
        jokeOutput.set_h(3);

        std::cout << "Prepare to update filter \n";
        particleFilter->update(motionInput.message(), visionInput.message());
        std::cout << "Updated filter \n";
        portals::Message<messages::RobotLocation> locMessage(0);
        *locMessage.get() = messages::RobotLocation();

        messages::RobotLocation l = particleFilter->getCurrentEstimate();
        std::cout << "X estimate:\t " << l.x() << "\nY estimate:\t " << l.y()
                  << "\nH estimate:\t " << l.h() << "\n\n";

        locMessage.get()->CopyFrom(particleFilter->getCurrentEstimate());
//        locMessage.get()->CopyFrom(jokeOutput);
        output.setMessage(locMessage);


        // output.setMessage(const particleFilter.getCurrentEstimate());
    }

    void LocalizationModule::run_()
    {
        std::cout << "Localization run called\n";
        update();
    }

    } // namespace localization
} // namespace localization
