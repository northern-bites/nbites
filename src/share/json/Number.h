

#ifndef nb_json_number_header
#define nb_json_number_header

#include "Json.h"

namespace json {
    class Number {
    public:
        Number();
        
        const std::string serialize() const;
        const std::string toString() const;
        const std::string printi(int indent) const;
        JsonValueType type() const { return NumberType; }
        
        ~Number(){}
        
    private:
    };
    
}

#endif