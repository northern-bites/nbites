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
    }

    void getPrev() {
        parsingBoard.rewindAll();
    }

    man::memory::Memory::const_ptr getMemory() const {
        return memory;}

public slots:
    void newInputProvider(common::io::InProvider::const_ptr newInput);

    void addSubscriber(Subscriber* subscriber, MObject_ID mobject_id);

private:
    man::memory::Memory::ptr memory;
    man::memory::parse::ParsingBoard parsingBoard;

};

}
}
