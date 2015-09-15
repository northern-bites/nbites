

#ifndef nb_json_array_header
#define nb_json_array_header

#include "Json.h"
#include <vector>

namespace json {
    
    class Array : public std::vector<JsonValue *>, public JsonValue {
        
    public:
        Array(){}
        
        Array(std::initializer_list<JsonValue *> list) :
            std::vector<JsonValue *>(list)
        { }
        
        Array(const std::vector<JsonValue *>& vec) :
            std::vector<JsonValue *> (vec)
        { }
        
        const std::string toString() {
            return stdprintf("JsonArray[%zu]", size());
        }
        
        ~JsonValue(){ }
        
        const std::string serialize() const;
        const std::string toString() const;
        const std::string printi(int indent) const;
        
        JsonValueType type() const {
            return ArrayType;
        }
    };
    
}

#endif