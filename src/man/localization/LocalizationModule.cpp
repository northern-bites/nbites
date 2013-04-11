#include "LocalizationModule.h"

namespace man
{
    namespace localization
    {
    LocalizationModule::LocalizationModule()
        : portals::Module(),
          output(base())
    {
        particleFilter = new ParticleFilter();
        particleFilter->resetLocTo(100,100,0);
        lastOdometry.set_x(-101.f);
        curOdometry.set_x(-101.f);
    }

    LocalizationModule::~LocalizationModule()
    {
        delete particleFilter;
    }

    void LocalizationModule::update()
    {
        motionInput.latch();
        visionInput.latch();
        resetInput.latch();

        if (lastReset != resetInput.message().timestamp())
        {
            lastReset = resetInput.message().timestamp();
            particleFilter->resetLocTo(resetInput.message().x(),
                                       resetInput.message().y(),
                                       resetInput.message().h());
        }


        // NOTE: Particle Filter wants to get deltaX, deltaY, etc...
        lastOdometry.set_x(curOdometry.x());
        lastOdometry.set_y(curOdometry.y());
        lastOdometry.set_h(curOdometry.h());

        curOdometry.set_x(motionInput.message().x());
        curOdometry.set_y(motionInput.message().y());
        curOdometry.set_h(motionInput.message().h());

        if(lastOdometry.x() < -100.f) // so we havent had two updates yet and thus invalid odometry data
        {
            messages::RobotLocation tempOdometry;
            tempOdometry.set_x(0.f);
            tempOdometry.set_y(0.f);
            tempOdometry.set_h(0.f);

            particleFilter->update(tempOdometry, visionInput.message());
        }
        else
        {
            deltaOdometry.set_x(curOdometry.x() - lastOdometry.x());
            deltaOdometry.set_y(curOdometry.y() - lastOdometry.y());
            deltaOdometry.set_h(curOdometry.h() - lastOdometry.h());
            particleFilter->update(deltaOdometry, visionInput.message());
        }

        portals::Message<messages::RobotLocation> locMessage(&particleFilter->
                                                             getCurrentEstimate());
        output.setMessage(locMessage);
    }

    void LocalizationModule::run_()
    {
        update();
    }

    } // namespace localization
} // namespace localization
