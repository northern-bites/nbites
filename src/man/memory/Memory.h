/**
 * @class Memory
 *
 * This class keeps instances of all the different memory objects and provides
 * an interface through which they get updated (each memory object copies data
 * from its corresponding man object)
 *
 * Each MObject is associated with a string that identifies it; this string
 * is usually the class name
 *
 * Memory is more or less a glorified ProtobufMessage map
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

    typedef common::io::ProtobufMessage Object;

public:
    ADD_SHARED_PTR(Memory)

    typedef std::pair<std::string, boost::shared_ptr<Object> > MObject_IDPair;
    typedef std::map<std::string, boost::shared_ptr<Object> > MObject_IDMap;

    typedef MObject_IDMap::iterator iterator;
    typedef MObject_IDMap::const_iterator const_iterator;

public:
    Memory() {}
    virtual ~Memory() {}

public:
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
        return boost::dynamic_pointer_cast<const T>(this->getByName(class_name<T>()));
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

    int numObjects() const { return mobject_IDMap.size(); }

protected:
    MObject_IDMap mobject_IDMap;
};


}
}
