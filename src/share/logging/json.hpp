
//
//  json.h
//
//  Created by Philip Koch on 3/19/16.
//
//  json classes and types for nbites
//

#ifndef _nbl_json_h_
#define _nbl_json_h_

#include <stdio.h>
#include <string>
#include <map>
#include <vector>
#include <algorithm>

#include "utilities.hpp"

namespace json {
        
    /*
     --------------------------------------------------
     x-macros to generate various data sets related to value types.
     */
#define JXM_TYPESET \
    JXM(Null) JXM(Boolean) JXM(Number) \
    JXM(String) JXM(Array) JXM(Object)
    
#define JXM(x) x ## Type ,
    enum JsonValueType {
        JXM_TYPESET
    };
#undef JXM
    
    //forward declarations for casting (see JsonValue)
    /*
#define JXM(x) class x;
    JXM_TYPESET
#undef JXM
     */
    
    //enum-indexed array of json value type names
    //defined in json.cpp
    extern const char * JSON_VALUE_TYPE_STRINGS[];
    
    /*
     --------------------------------------------------
     json classes from most abstract to most concrete
     */
    
    class Value;
    
    //Interface for all json value types.
    
    //All value implementations must provide:
    //  a protected no-args constructor for derived Value
    //  type-specific plain-text implementations
    
#define JSON_CLASS_WVIS(name, vis, ...)         \
        class name __VA_ARGS__                \
            {                                   \
            vis     :                           \
                name (){};                      \
            public  :                           \
                virtual const std::string       \
                serialize() const;              \
                virtual const std::string       \
                print_i(int indent) const;      \
                virtual const std::string       \
                print() const { return print_i(0); }

#define JSON_CLASS(name, ...)   \
    JSON_CLASS_WVIS(name, protected, __VA_ARGS__)

    //Value implementations
    
    JSON_CLASS_WVIS(Null, public)
        Null(const Null& other) {}
    };

    JSON_CLASS(Boolean)
        Boolean(bool val) {val_ = val;}
        Boolean(const Boolean& other) {
            val_ = other.value();
        }

        bool value() const {return val_;}
    private:
        bool val_;
    };

    JSON_CLASS(Number)
        Number(int val);
        Number(long val);
        Number(double val);
        Number(float val);

        Number(const std::string valrep);

        Number(const Number& other) {
            rep = other.rep;
        }

        int asInt() const;
        long asLong() const;
        float asFloat() const;
        double asDouble() const;

    private:
        std::string rep;
    };

    JSON_CLASS(String, : public std::string)
        String(const std::string& val) :
            std::string(val)
        {}

        String(const String& other) :
            std::string(static_cast<const std::string&>(other))
        {}
    };

    JSON_CLASS_WVIS(Array, public, : public std::vector<Value>)
        Array(const std::vector<Value>& other) :
            std::vector<Value>(other) {}

        Array(std::initializer_list<Value> other) :
            std::vector<Value>(other) {}

        Array(const Array& other) :
            std::vector<Value>(static_cast<const std::vector<Value>&>(other)) {}
    };

    JSON_CLASS_WVIS(Object, public, : public std::map<std::string, Value>)
        Object(const Object& other) :
            std::map<std::string, Value>
                (static_cast<const std::map<std::string, Value>&>(other)) {}

        Object(const std::map<std::string, Value>& other) :
            std::map<std::string, Value>(other) {}
    };

    //Union class to avoid object sliding
    class Value :
        private Null, Boolean, Number, String, Array, Object {
            
        private:
            JsonValueType type_;
            
        public:
            
            Value() : Null(), type_(NullType){ }
            
#define JXM(x)  \
    Value(const x& super) : x(super), type_(x ## Type) {}
            
            JXM_TYPESET
#undef JXM

            JsonValueType type() const { return type_; }
            
            virtual const std::string
                serialize() const;
            virtual const std::string
                print_i(int indent) const;
            
            //Need to redclare to disambiguate method.
            const std::string print() const { return print_i(0); }
            
            //Generate checked daisy-chain casting for various value types.
#define JXM(x) \
            x & as ## x() { \
                if ( type() == (x ## Type) )        \
                    return static_cast<x&> (*this); \
                else throw std::domain_error(       \
                    nbl::utilities::format("json: cannot cast json::%s to json::%s!",  \
                    JSON_VALUE_TYPE_STRINGS[type()], JSON_VALUE_TYPE_STRINGS[x ## Type] \
                ) ); \
            }
            
            JXM_TYPESET
#undef JXM

#define JXM(x) \
            const x & asConst ## x() const { \
                if ( type() == (x ## Type) )        \
                    return static_cast<const x&> (*this); \
                else throw std::domain_error(       \
                    nbl::utilities::format("json: cannot cast json::%s to json::%s!",  \
                    JSON_VALUE_TYPE_STRINGS[type()], JSON_VALUE_TYPE_STRINGS[x ## Type] \
                ) ); \
            }

            JXM_TYPESET
#undef JXM

    };

    //parse and return the first json value in 'text.'
    Value parse(const std::string& text);

    //parse and return the first json value in 'text.'
    //set end to index of first character after returned json value.
    Value parse(const std::string& text, int * end);

    //parse and return the first json value in 'text,' starting
    //at character text[from]
    Value parseFrom(const std::string& text, int from);

    //parse and return the first json value in 'text,' starting
    //at character text[from], setting end to index of first character
    //after returned json value.
    Value parseFrom(const std::string& text, int from, int * end);
}

#endif /* defined(_nbl_json_h_) */




