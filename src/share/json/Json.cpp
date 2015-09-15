

#include "Json.h"
#include <stdarg.h>
#include <assert.h>

namespace json {
    
    //Definition of JSON_VALUE_TYPE_STRINGS string array using x-macros.
#define JXM(x) #x ,
    const char * JSON_VALUE_TYPE_STRINGS[] = {
#include "JsonvalueTypes.defs.h"
    };
#undef JXM
    
    std::string indentStr(int i) {
        return std::string(i * 2, ' ');
    }
    
    std::string stdprintf(const char * format, ...) {
        va_list args;
        va_start(args, format);
        
        //+1 for \0
        size_t len = (size_t) vsnprintf(NULL, 0, format, args) + 1;
        assert(len > 0);    //vsnprintf returns -1 on format error or some legacy systems.
        va_end(args);
        va_start(args, format);
        
        char buffer[ len ];
        vsnprintf(buffer, len, format, args);
        va_end(args);
        
        return std::string(buffer);
    }
    
    const Null NULL_VALUE{};
    
    bool isNull(JsonValue * val) {
        if (val == NULL || val->type() == NullType)
            return true;
        return false;
    }
    
}

/*
 parsing
 */
#include <map>
namespace json {
    
    enum JsonTokenType {
        ARRAY_START,
        ARRAY_END,
        SEPARATOR,
        
        OBJECT_START,
        OBJECT_DIVIDER,
        OBJECT_END,
        
        STRING,
        NUMBER
    };
    
    class Token {
    public:
        std::string text;
        JsonTokenType type;
        
        int start;
        int after;
    };
    
    std::map<char, JsonTokenType> SPECIAL = {
        {'[', ARRAY_START},
        {']', ARRAY_END},
        {',', SEPARATOR},
        {'{', OBJECT_START},
        {':', OBJECT_DIVIDER},
        {'}', OBJECT_END},
        {'\'', STRING},
        {'"', STRING}
    };
    
    std::map<std::string, JsonValue *> RESERVED = {
        { std::string("null"), (JsonValue *) &NULL_VALUE },
        { std::string("true"), (JsonValue *) &TRUE },
        { std::string("false"), (JsonValue *) &FALSE }
    };
    
    class parse_exception : public std::exception {
        
    };
    
    bool stringTerminated(const std::string& str,
                          int i, char strchar) {
        return str[i] == strchar && str[i-1] != '\\';
    }
    
    bool isNumberChar(const std::string& str,
                      int i) {
        return std::isalnum(str[i]) ||
            str[i] == '.';
    }
    
    bool isWhitespace(const std::string& str, int i) {
        return std::isspace(str[i]);
    }
    
    JsonValue * parse(std::string text);
    
    JsonValue * parse(std::string text, int * end);
    
    JsonValue * parseFrom(std::string text, int from);
    
    JsonValue * parseFrom(std::string text, int from, int * end);
}