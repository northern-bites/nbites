/**
 * @class Memory
 *
 * This class keeps instances of all the different memory objects and provides
 * an interface through which they get updated (each memory object copies data
 * from its corresponding man object
 *
 * Each MObject is associated with a MObject_ID that identifies it
 *
 * Future work: we will be able to keep multiple instances of selected objects
 *
 * @author Octavian Neamtu
 */

#pragma once

#include <boost/shared_ptr.hpp>
#include <map>
#include <exception>

namespace man {
namespace memory {
class Memory; //forward declaration
}
}

#include "MObject.h"
#include "MVision.h"
#include "Vision.h"
#include "MVisionSensors.h"
#include "MMotionSensors.h"
#include "MImage.h"
#include "Camera.h"
#include "MLocalization.h"
#include "Sensors.h"
#include "Profiler.h"
#include "ClassHelper.h"
#include "io/ProtobufMessage.h"

namespace man {
namespace memory {

/**
 * Meant to be thrown when one tries to get an object that is not included
 * in memory (or at least the memory object map)
 *      - Octavian
 */
class NonExistentMemoryObjectError : public std::exception {

public:
    NonExistentMemoryObjectError(std::string class_name) : class_name(class_name) {}
    virtual ~NonExistentMemoryObjectError() throw() {}

    virtual const char* what() const throw () {
        return ("Couldn't find " + class_name).c_str();
    }

private:
        std::string class_name;

};

class Memory {

    typedef common::io::NotifyingProtobufMessage Object;

public:
    ADD_NULL_INSTANCE(Memory)
    ADD_SHARED_PTR(Memory)

    typedef std::pair<string, boost::shared_ptr<Object> > MObject_IDPair;
    typedef std::map<string, boost::shared_ptr<Object> > MObject_IDMap;

    typedef MObject_IDMap::iterator iterator;
    typedef MObject_IDMap::const_iterator const_iterator;

public:
    Memory(boost::shared_ptr<Vision> vision_ptr = boost::shared_ptr<Vision>(),
           boost::shared_ptr<Sensors> sensors_ptr = boost::shared_ptr<Sensors>(),
           boost::shared_ptr<LocSystem> loc_ptr = boost::shared_ptr<LocSystem>());
    virtual ~Memory();
    /**
     * calls the update function on @obj
     * this will usually make the MObject pull data
     * from its corresponding man object
     */
    void update(boost::shared_ptr<Object> obj);
    void updateVision();

public:
    MVision::const_ptr getMVision() const {return mVision;}
    MVisionSensors::const_ptr getMVisionSensors() const {return mVisionSensors;}
    MMotionSensors::const_ptr getMMotionSensors() const {return mMotionSensors;}
    MImage::const_ptr getMImage(corpus::Camera::Type which) const;

    const_iterator begin() const { return mobject_IDMap.begin(); }
    const_iterator end() const { return mobject_IDMap.end(); }

    void subscribe(Subscriber* subscriber, std::string name) const throw(NonExistentMemoryObjectError) {
        this->getByName(name)->addSubscriber(subscriber);
    }

    void unsubscribe(Subscriber* subscriber, std::string name) const throw(NonExistentMemoryObjectError) {
        this->getByName(name)->unsubscribe(subscriber);
    }

    //templated getters - generally pretty awesome
    template<class T>
    boost::shared_ptr<const T> get() const throw(NonExistentMemoryObjectError) {
        return boost::dynamic_pointer_cast<T>(this->getByName(class_name<T>()));
    }

    template<class T>
    boost::shared_ptr<T> get() throw(NonExistentMemoryObjectError) {
        return boost::dynamic_pointer_cast<T>(this->getByName(class_name<T>()));
    }

    Object::const_ptr getByName(std::string name) const throw(NonExistentMemoryObjectError) {
        MObject_IDMap::const_iterator it = mobject_IDMap.find(name);

        if (it != mobject_IDMap.end()) {
            return it->second;
        } else {
            throw NonExistentMemoryObjectError(name);
        }
    }

    Object::ptr getByName(std::string name) throw(NonExistentMemoryObjectError) {
        MObject_IDMap::iterator it = mobject_IDMap.find(name);

        if (it != mobject_IDMap.end()) {
            return it->second;
        } else {
            throw NonExistentMemoryObjectError(name);
        }
    }

private:
    MObject_IDMap mobject_IDMap;
    boost::shared_ptr<MVision> mVision;
    boost::shared_ptr<MVisionSensors> mVisionSensors;
    boost::shared_ptr<MMotionSensors> mMotionSensors;
    boost::shared_ptr<MBottomImage> bottomMImage;
    boost::shared_ptr<MTopImage> topMImage;
    boost::shared_ptr<MLocalization> mLocalization;
};
}
}
