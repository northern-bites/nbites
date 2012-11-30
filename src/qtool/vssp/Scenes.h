#pragma once

#include "/home/ecat/nbites/src/man/portals/RoboGrams.h"
#include <boost/circular_buffer.hpp>
#include <iostream>

struct Scene
{
    int i;
    int j;
    Scene(int k, int l) : i(k), j(l) {};
};

typedef boost::circular_buffer<Scene*>::iterator SceneIt;
class SceneBuffer
{
public:
    SceneBuffer(int capacity) : cb(capacity) {};

    bool push_back(Scene* current)
    {
        bool replaced = false;
        if(cb.full())
        {
            delete cb.front();
            replaced = true;
            std::cout << "Replaced an element." << std::endl;
        }

        cb.push_back(current);
    }

    void printInfo()
    {
        std::cout << "The buffer has capacity " << cb.capacity() <<
            "\nThe buffer has size " << cb.size() << std::endl;
        SceneIt it = cb.begin();
        while (it != cb.end())
        {
            if ( *it == NULL)
            {
                std::cout << "NULL" << std::endl;
                it++;
                continue;
            }
            std::cout << (*it)->i << ", " <<(*it)->j << std::endl;
            it++;
        }
    }

private:
    boost::circular_buffer<Scene*> cb;
};

// class SceneHolder : private RefCounter
// {
//     friend class Scene;
//     static SceneBuffer scenes;
// };
