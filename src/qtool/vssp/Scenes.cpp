#include "Scenes.h"
#include <iostream>

using man::memory::proto::PVision;

Scene::Scene(PVision input, int frame) : framestamp(frame)
{
    // Make objects from the protobuf and put them into the multimap here
    TemporalObject theBall("ball",
                           input.visual_ball().visual_detection().x(),
                           input.visual_ball().visual_detection().y(),
                           input.visual_ball().visual_detection().distance(),
                           input.visual_ball().visual_detection().bearing(),
                           this,
                           NULL,
                           NULL);
}

void SceneBuffer::printInfo()
{
    std::cout << "The buffer has capacity " << cb.capacity() <<
        "\nThe buffer has size " << cb.size() << std::endl;
}


