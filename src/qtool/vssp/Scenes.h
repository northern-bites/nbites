#pragma once

#include <boost/circular_buffer.hpp>
#include <iostream>

struct Scene
{
    int i;
    int j;
    Scene(int k, int l) : i(k), j(l) {};
};

typedef boost::circular_buffer<Scene>::iterator SceneIt;
class SceneBuffer
{
public:
    SceneBuffer(int capacity) : cb(capacity) {};

    bool push_back(Scene current)
    {
        cb.push_back(current);
    }

    void printInfo()
    {
        std::cout << "The buffer has capacity " << cb.capacity() <<
            "\nThe buffer has size " << cb.size() << std::endl;
        SceneIt it = cb.begin();
        while (it != cb.end())
        {
            std::cout << (*it).i << ", " <<(*it).j << std::endl;
            it++;
        }
    }

private:
    boost::circular_buffer<Scene> cb;
};
