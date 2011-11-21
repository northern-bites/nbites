/**
 * @class Parser
 *
 * Abstract log parser
 *
 * @author Octavian Neamtu
 */

#pragma once

#include "io/InProvider.h"
#include "ClassHelper.h"

namespace man {
namespace memory {
namespace parse {

class Parser {

    ADD_SHARED_PTR(Parser)

protected:
    typedef common::io::InProvider InProvider;

public:
    Parser(InProvider::const_ptr inProvider) :
                inProvider(inProvider) {
    }

    virtual ~Parser() {}

    virtual bool getNext() = 0;
    virtual bool getPrev() = 0;

protected:
    InProvider::const_ptr inProvider;

};

}
}
}
