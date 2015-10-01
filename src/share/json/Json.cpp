

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
        std::string val;
        JsonTokenType type;
        
        int start;
        int after;
        
        Token(std::string t, JsonTokenType ty, int s, int a) : val(t), type(ty), start(a), after(s) {}
        
        Token() {}
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
        { std::string("true"), (JsonValue *) &TRUE_VALUE },
        { std::string("false"), (JsonValue *) &FALSE_VALUE }
    };
    
    class parse_exception : public std::range_error {
    public:
        parse_exception(const char * what) :
            std::range_error(what) {}
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
    
    Token peek(const std::string& text, int _from) {
        int pos = _from;
        for (; pos < text.length() && isWhitespace(text, pos); ++pos);
        
        if (pos >= text.length() ||
            text[pos] == '\0') {
            throw parse_exception("expected token but reached end of input");
        }
        
        char c = text[pos];
        
        if ( SPECIAL.find(c) != SPECIAL.end() ) {
            JsonTokenType type = SPECIAL[c];
            
            if (type != STRING) {
                Token ret;
                ret.type = type;
                ret.start = pos;
                ret.after = pos + 1;
                
                return ret;
            } else {
                //Escaped string.
                int end = pos + 1;
                for (; end < text.length() &&
                     !stringTerminated(text, end, c); ++end
                     );
                
                if ( end >= text.length() )
                    throw parse_exception("missing character");
                assert(text[end] == c);
                
                Token ret;
                ret.type = STRING;
                //Must include strchar! Otherwise "null" cannot be differentiated from null .
                //ret.val = text.substring(pos, end + 1);
                ret.val = text.substr(pos, end - pos + 1);
                ret.start = pos;
                ret.after = end + 1;
                
                return ret;
            }
        }
        
        //Number token – all numbers start with either a minus or a digit.
        if (c == '-' || std::isdigit(c)) {
            int after = pos + 1;
            for (; after < text.length() && isNumberChar(text, after); ++after);
            
            Token ret;
            ret.type = NUMBER;
            ret.val = text.substr(pos, pos - after);
            ret.start = pos;
            ret.after = after;
            
            return ret;
        }
        
        //Unescaped string (or possibly reserved string)
        if (std::isalpha(c)) {
            int after = pos + 1;
            for (; after < text.length() &&
                 !isWhitespace(text, after) &&
                 !SPECIAL.count(text[after]); ++after);
            
            Token ret;
            ret.type = STRING;
            ret.val = text.substr(pos, pos - after);
            ret.start = pos;
            ret.after = after;
            
            return ret;
        }
        
        throw parse_exception("unexpected character");
    }
    
    Token next(const std::string& text, int& loc) {
        Token found = peek(text, loc);
        loc = found.after;
        return found;
    }
    
    /*
    JsonValue * _parse(Token tok) {
        switch(tok.type) {
            case ARRAY_END: {
                throw parse_exception("unexpected array end");
            }
                
            case ARRAY_START: {
                JsonArray array = new JsonArray();
                
                Token first = peekToken();
                if (first.type == TokenType.ARRAY_END) {
                    nextToken();	//Consume ARRAY_END
                    return array;
                }
                
                for(;;) {
                    Token value = nextToken();
                    array.add( _parse(value) );
                    
                    Token after = nextToken();
                    if (after.type == TokenType.ARRAY_END)
                        break;
                    if (after.type != TokenType.SEPARATOR)
                        throw new MissingTokenException(TokenType.SEPARATOR, after);
                        }
                
                return array;
            }
                
            case NUMBER: {
                return new JsonNumber(tok.val);
            }
                
            case OBJECT_DIVIDER: {
                throw new UnexpectedTokenException(tok);
            }
                
            case OBJECT_END: {
                throw new UnexpectedTokenException(tok);
            }
                
            case OBJECT_START: {
                JsonObject object = new JsonObject();
                
                Token first = peekToken();
                if (first.type == TokenType.OBJECT_END) {
                    //Logger.println("emtpy object");
                    nextToken();	//Consume OBJECT_END
                    return object;
                }
                
                for (;;) {
                    readObjectPair(object);
                    
                    Token after = nextToken();
                    if (after.type == TokenType.OBJECT_END)
                        break;
                    if (after.type != TokenType.SEPARATOR)
                        throw new MissingTokenException(TokenType.SEPARATOR, after);
                        }
                
                return object;
            }
                
            case SEPARATOR: {
                throw new UnexpectedTokenException(tok);
            }
                
            case STRING: {
                if (tok.val.trim().isEmpty()) {
                    throw new JsonParseException("empty STRING token: " + tok, tok.start, text);
                }
                
                if (RESERVED.containsKey(tok.val)) {
                    return RESERVED.get(tok.val);
                }
                
                String string = tok.val;
                char start = string.charAt(0);
                if (SPECIAL.containsKey(start)) {
                    assert(SPECIAL.get(start) == TokenType.STRING);
                    assert(string.endsWith("" + start));
                    
                    string = string.substring(1, string.length() - 1);
                }
                
                return new JsonString(string);
            }
                
            default:
                throw parse_exception("unexpected token type");
        }		
    } */
    
    JsonValue * parse(std::string text);
    
    JsonValue * parse(std::string text, int * end);
    
    JsonValue * parseFrom(std::string text, int from);
    
    JsonValue * parseFrom(std::string text, int from, int * end);
}