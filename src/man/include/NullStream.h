/**
 * @class NullStream
 *
 * Anything streamed to this class goes nowhere
 *
 * @author Octavian Neamtu - with some help from stackoverflow
 *
 * http://stackoverflow.com/questions/1134388/stdendl-is-of-unknown-type-when-overloading-operator
 *
 * Thank you GMan!
 *
 */


#pragma once

#include <iostream>
#include "NullInstanceHelper.h"

class NullStream {

public:
    ADD_NULL_INSTANCE(NullStream);

    template<typename TPrintable>
    NullStream& operator<< (const TPrintable& ) { return *this; }

    //to solve for std::endl
    typedef std::basic_ostream<char, std::char_traits<char> > CoutType;

    // this is the function signature of std::endl
    typedef CoutType& (*StandardEndLine)(CoutType&);

    // define an operator<< to take in std::endl
    NullStream& operator<<(StandardEndLine stdendl_method)
    {
        //do nothing
        return *this;
    }
};
