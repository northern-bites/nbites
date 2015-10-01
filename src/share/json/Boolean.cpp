#include "Boolean.h"
#include <sstream>

namespace json {
    bool Boolean::value() {
        return _value;
    }
    
    Boolean::Boolean(bool v) {
        _value = v;
    }
    
    const std::string Boolean::serialize() const {
        return _value ? std::string("true") :
            std::string("false") ;
    }
    
    const std::string Boolean::toString() const {
        return stdprintf("JsonBoolean-%s",
                         serialize().c_str());
    }
    
    const std::string Boolean::printi(int indent) const {
        return indentStr(indent) + serialize();
    }
    
    const Boolean TRUE_VALUE(true);
    const Boolean FALSE_VALUE(false);
}