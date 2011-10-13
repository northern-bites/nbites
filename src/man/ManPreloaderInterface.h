
/**
 * @class ManPreloaderInterface
 *
 * A generic interface to a class that preloads/loads Man
 *
 * @author Octavian Neamtu
 */

#pragma once

#include <boost/shared_ptr.hpp>

#include "NullInstanceHelper.h"

class ManPreloaderInterface {

public:
    typedef boost::shared_ptr<ManPreloaderInterface> ptr;
    typedef boost::shared_ptr<const ManPreloaderInterface> const_ptr;

    virtual ~ManPreloaderInterface(){}

protected:
    ManPreloaderInterface(){}

public:
    ADD_NULL_INSTANCE(ManPreloaderInterface);

    virtual void reloadMan(){}
    virtual void createMan(){}
    virtual void destroyMan(){}
};
