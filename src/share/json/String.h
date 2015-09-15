

#ifndef nb_json_string_header
#define nb_json_string_header

#include "Json.h"

namespace json {
    class String {
    public:
        String();
        
        const std::string serialize() const;
        const std::string toString() const;
        const std::string printi(int indent) const;
        JsonValueType type() const { return StringType; }
        
        ~String(){}
    private:
    };    
}

#endif