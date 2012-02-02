/**
 * @class SubscriberEmiterConvertor
 *
 * Provides a way to convert the notifier/subscriber framework in man to
 * signals/slots in qtool
 *
 * @author Octavian Neamtu
 */

#pragma once

#include "Subscriber.h"
#include <QObject>

#include <vector>
#include "man/memory/Memory.h"

namespace qtool {
namespace data {

class SubscriberEmiterConvertor : public QObject, public Subscriber {

    Q_OBJECT

public:
    SubscriberEmiterConvertor() {}
    virtual ~SubscriberEmiterConvertor() {}

    void update() {
        emit subscriberUpdate();
    }

signals:
    void subscriberUpdate();

};



/**
 * @class MemorySignaling Interface
 *
 * Manages a 1-1 map of SubscriberEmiterConvertor to MObjects
 * for all MObjects in the class Memory
 *
 * So when an MObject calls its updateSubscribers method, this
 * interface translates that into a signal
 */

class MemorySignalingInterface {

protected:
    typedef man::memory::MObject_ID MObject_ID;

    typedef std::pair<MObject_ID, SubscriberEmiterConvertor*> ms_pair;
    typedef std::map<MObject_ID, SubscriberEmiterConvertor*> ms_map;

public:
    MemorySignalingInterface(man::memory::Memory::const_ptr memory) :
        memory(memory) {

        using namespace man::memory;

        for (MObject_ID id = FIRST_OBJECT_ID;
                        id != LAST_OBJECT_ID; id++) {
            SubscriberEmiterConvertor* convertor = new SubscriberEmiterConvertor;
            memory->addSubscriber(convertor, id);
            convertors.insert(ms_pair(id, convertor));
        }
    }

    virtual ~MemorySignalingInterface() {
        for (ms_map::iterator it = convertors.begin();
                it != convertors.end(); it++) {
            memory->unsubscribe(it->second, it->first);
            delete it->second;
        }
    }

    void subscribeSlotToMObject(const QObject* subscriber,
                   const char* slot, MObject_ID mobject_id ) const {
        ms_map::const_iterator it = convertors.find(mobject_id);

        if (it != convertors.end()) {
            QObject::connect(it->second, SIGNAL(subscriberUpdate()),
                             subscriber, slot);
        }
    }

private:
    ms_map convertors;
    man::memory::Memory::const_ptr memory;

};

}
}
