/**
 *
 * @class DataManager
 *
 * This class should handle the notifying all subscribers to new data
 * coming into the system (be it online or offline)
 *
 * @author Octavian Neamtu
 *
 */

#pragma once

#include <boost/shared_ptr.hpp>
#include <QObject>

#include "io/InProvider.h"
#include "man/memory/Memory.h"
#include "man/memory/parse/ParsingBoard.h"
#include "include/MultiProvider.h"
#include "DataTypes.h"
#include "ClassHelper.h"

namespace qtool {
namespace data {

class DataManager : public QObject {

    Q_OBJECT

ADD_SHARED_PTR(DataManager);

public:
    DataManager();

    virtual ~DataManager();

    void getNext() {
        parsingBoard.parseNextAll();
        memory->notifySubscribers(man::memory::MIMAGE_ID);
        memory->notifySubscribers(man::memory::MVISION_ID);
        memory->notifySubscribers(man::memory::MVISION_SENSORS_ID);
        memory->notifySubscribers(man::memory::MMOTION_SENSORS_ID);
    }

    void getPrev() {
        parsingBoard.rewindAll();
        memory->notifySubscribers(man::memory::MIMAGE_ID);
        memory->notifySubscribers(man::memory::MVISION_ID);
        memory->notifySubscribers(man::memory::MVISION_SENSORS_ID);
        memory->notifySubscribers(man::memory::MMOTION_SENSORS_ID);
    }

    man::memory::Memory::const_ptr getMemory() const {
        return memory;}

public slots:
    void newInputProvider(common::io::InProvider::const_ptr newInput);

    void addSubscriber(Subscriber<MObject_ID>* subscriber,
            MObject_ID mobject_id);
    void addSubscriber(Subscriber<MObject_ID>* subscriber);

private:
    man::memory::Memory::ptr memory;
    man::memory::parse::ParsingBoard parsingBoard;

};

}
}
