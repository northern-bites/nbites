/*
 Top-level header – mirrors nbtool.data.json.Json.java
 */

#ifndef nb_json_header
#define nb_json_header

#include <string>
#include <stdexcept>

namespace json {
    
    /*
     These two collections are generated with x-macros, look 'em up.
     */
#define JXM(x) x ## Type ,
    enum JsonValueType {
#include "JsonvalueTypes.defs.h"
    };
#undef JXM
    
    extern const char * JSON_VALUE_TYPE_STRINGS[];
    
    //Forward decls for casting.
    
#define JXM(x) class x;
#include "JsonvalueTypes.defs.h"
#undef JXM
    
    class JsonValue {
    public:
        virtual const std::string serialize() const =0;
        virtual const std::string toString() const =0;
        virtual const std::string printi(int indent) const =0;
        virtual JsonValueType type() const = 0;
        virtual ~JsonValue(){}
        
        const std::string print() const {
            return printi(0);
        }
        
        //Generate checked daisy-chain casting for various value types.
#define JXM(x) \
    x * as ## x() const { \
        if (type() == (x ## Type) ) return (x *) this;   \
        else throw std::domain_error("cannot cast this JsonValue to that type!"); \
    }
        
#include "JsonvalueTypes.defs.h"
#undef JXM
        
    };
    
    extern std::string indentStr(int i);
    extern std::string stdprintf(const char * format, ...);
    
    extern bool isNull(JsonValue *);
    
    class Null : public JsonValue {
    public:
        const std::string serialize() const {
            return std::string("null");
        }
        
        const std::string toString() const {
            return std::string("JsonValue-null");
        }
        
        const std::string printi(int i) const {
            return indentStr(i) + serialize();
        }
        
        JsonValueType type() const { return NullType; }
        
        ~Null(){}
    };
 
    const extern Null NULL_VALUE;
}

#include "Boolean.h"
#include "String.h"
#include "Number.h"
#include "Array.h"
#include "Object.h"

/*
 parsing
 */

namespace json {
    
    JsonValue * parse(std::string text);
    
    JsonValue * parse(std::string text, int * end);
    
    JsonValue * parseFrom(std::string text, int from);
    
    JsonValue * parseFrom(std::string text, int from, int * end);
}

#endif