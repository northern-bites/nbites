#pragma once

#include <boost/circular_buffer.hpp>
#include <string>
#include <map>
#include "memory/protos/Vision.pb.h"

class Scene;

// TemporalObject
class TemporalObject
{
public:
    TemporalObject(std::string name,
                   int x,
                   int y,
                   float dist,
                   float bear,
                   Scene* sc,
                   TemporalObject* prev,
                   TemporalObject* nex) : objectType(name),
                                         visualX(x),
                                         visualY(y),
                                         visualDistance(dist),
                                         visualBearing(bear),
                                         scene(sc),
                                         previousObject(prev),
                                         nextObject(nex) {};

    inline std::string type() { return objectType; }
    inline int x() { return visualX; }
    inline int y() { return visualY; }
    inline float distance() { return visualDistance; }
    inline float bearing() { return visualBearing; }

    Scene* getScene() { return scene; }
    TemporalObject* next() { return nextObject; }
    TemporalObject* previous() { return previousObject; }

private:
    std::string objectType;
    int visualX;
    int visualY;
    float visualDistance;
    float visualBearing;

    Scene* scene;
    TemporalObject* previousObject;
    TemporalObject* nextObject;
};

// Scene
class Scene
{
public:
    Scene(man::memory::proto::PVision input, int frame);

    inline int stamp() { return framestamp; }

private:
    int framestamp;
    std::multimap<std::string, TemporalObject> objects;
};

typedef boost::circular_buffer<Scene>::iterator SceneIt;

// SceneBuffer
class SceneBuffer
{
public:
    explicit SceneBuffer(int capacity) : cb(capacity) {};

    bool addScene(Scene current)
    {
        cb.push_back(current);
    }

    bool addScene(man::memory::proto::PVision current, int frame)
    {
        cb.push_back(Scene(current, frame));
    }

    SceneIt begin() { return cb.begin(); }
    SceneIt end() { return cb.end(); }

    void printInfo();

private:
    boost::circular_buffer<Scene> cb;
};
