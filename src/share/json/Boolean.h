

#ifndef nb_json_boolean_header
#define nb_json_boolean_header

#include "Json.h"

namespace json {
    class Boolean : public JsonValue {
    public:
        bool value();
        Boolean(bool v);
        
        const std::string serialize() const;
        const std::string toString() const;
        const std::string printi(int indent) const;
        JsonValueType type() const { return BooleanType; }
        
        ~Boolean(){}
        
    private:
        bool _value;
    };
    
    const extern Boolean TRUE_VALUE;
    const extern Boolean FALSE_VALUE;
}

#endif