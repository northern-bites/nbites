

#ifndef nb_json_object_header
#define nb_json_object_header

#include "Json.h"

#include <map>
#include <string>

namespace json {
    class Object :
        public std::map<std::string, JsonValue *>,
        public JsonValue
    {
    public:
        Object();
        
        const std::string serialize() const;
        const std::string toString() const;
        const std::string printi(int indent) const;
        JsonValueType type() const { return ObjectType; }
        
        ~Object(){}
        
    private:
    };    
}

#endif