#include "Scenes.h"
#include <iostream>

using man::memory::proto::PVision;

Scene::Scene(PVision input, int frame) : framestamp(frame)
{
    // Make objects from the protobuf and put them into the multimap here
}

void SceneBuffer::printInfo()
{
    std::cout << "The buffer has capacity " << cb.capacity() <<
        "\nThe buffer has size " << cb.size() << std::endl;
}


