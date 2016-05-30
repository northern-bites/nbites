
//
//  json.cpp
//
//  Created by Philip Koch on 3/19/16.
//
//  json implementation for nbites
//

#include "json.hpp"
#include "utilities-test.hpp"
#include <assert.h>
#include <stdexcept>

namespace json {
    
#define JXM(x) #x ,
    const char * JSON_VALUE_TYPE_STRINGS[] = {
        JXM_TYPESET
    };
#undef JXM
    
    std::string indentStr(int i) {
        return std::string(i * 2, ' ');
    }
    
    const std::string Null::serialize() const {
        return "null";
    };
    
    const std::string Null::print_i(int indent) const {
        return indentStr(indent) + serialize();
    };
    
    const std::string Boolean::serialize() const {
        return val_ ? "true" : "false";
    };
    
    const std::string Boolean::print_i(int indent) const {
        return indentStr(indent) + serialize();
    };
    
    const std::string Number::serialize() const {
        return rep;
    };
    
    const std::string Number::print_i(int indent) const {
        return indentStr(indent) + serialize();
    };
    
    Number::Number(int val) {
        char buf[100];
        snprintf(buf, 100, "%i", val);
        rep = std::string(buf);
    }
    
    Number::Number(long val) {
        char buf[100];
        snprintf(buf, 100, "%li", val);
        rep = std::string(buf);
    }
    
    Number::Number(double val) {
        char buf[100];
        snprintf(buf, 100, "%lf", val);
        rep = std::string(buf);
    }
    
    Number::Number(float val) {
        char buf[100];
        snprintf(buf, 100, "%f", val);
        rep = std::string(buf);
    }
    
    Number::Number(const std::string valrep) {
        rep = valrep;
    }
    
    int Number::asInt() const {
        return std::stoi(rep);
    }
    
    long Number::asLong() const {
        return std::stoll(rep);
    }
    
    float Number::asFloat() const {
        return std::stof(rep);
    }
    
    double Number::asDouble() const {
        return std::stod(rep);
    }
    
    const std::string escaped(const std::string& input) {
        std::string buffer = input;
        for (size_t i = 0; i < input.length(); ) {
            size_t j = buffer.find_first_of('"', i);
            
            if (j != std::string::npos) {
                if (j == 0 || buffer[j - 1] != '\\') {
                    buffer.insert(j, "\\");
                    i = j + 2;
                } else {
                    i = j + 1;
                }
            } else break;
        }
        
        return buffer;
    }
    
    const std::string String::serialize() const {
        return "\"" + escaped(*this) + "\"";
    };
    
    const std::string String::print_i(int indent) const {
        return indentStr(indent) + serialize();
    };
    
    const std::string Array::serialize() const {
        std::string builder;
        builder.append("[");
        
        for (int i = 0; i < size(); ++i) {
            builder.append((*this)[i].serialize());
            if (i + 1 < size())
                builder.append(",");
            else break;
        }
        
        builder.append("]");
        return builder;
    };
    
    const std::string Array::print_i(int indent) const {
        
        std::string builder;
        builder.append(indentStr(indent) + "[\n");
        
        for (int i = 0; i < size(); ++i) {
            builder.append((*this)[i].print_i(indent + 1));
            if (i + 1 < size())
                builder.append(",");
            builder.append("\n");
        }
        
        builder.append(indentStr(indent) + "]");
        return builder;
    };
    
    const std::string Object::serialize() const {
        std::string builder = "{";
        
        int i = 0;
        for (
             std::map<std::string, Value>::const_iterator it =
             this->begin(); it != this->end();
             ++it
             ) {
            builder += '"' + escaped((*it).first) + '"';
            builder += ':';
            builder += (*it).second.serialize();
            
            if ((++i) < this->size()) {
                builder += ',';
            }
        }
        
        builder += '}';
        return builder;
    };
    
    const std::string Object::print_i(int indent) const {
        std::string builder = indentStr(indent) + "{\n";
        
        int i = 0;
        for (
             std::map<std::string, Value>::const_iterator it =
             this->begin(); it != this->end();
             ++it
             ) {
            builder += indentStr(indent + 1) + '"' + escaped((*it).first) + '"';
            builder += " :\n";
            builder += (*it).second.print_i(indent + 2);
            
            if ((++i) < this->size()) {
                builder += ',';
            }
            
            builder += '\n';
        }
        
        builder += indentStr(indent) + '}';
        return builder;
    };
    
#define JXM(x)  \
    case x ## Type: return x::serialize();
    
    const std::string Value::serialize() const {
        switch (type_) {
            JXM_TYPESET
#undef JXM
                
            default:
                break;
        }
    };
    
#define JXM(x)  \
    case x ## Type: return x::print_i(indent);
    
    const std::string Value::print_i(int indent) const {
        switch (type_) {
                JXM_TYPESET
#undef JXM
                
            default:
                break;
        }
    };
    
    /*
     STRUCTURES & FUNCTIONS RELATED TO PARSING
     */
    
    std::map<std::string, Value> RESERVED_STRINGS = {
        { std::string("null"), Null() },
        { std::string("true"), Boolean(true) },
        { std::string("false"), Boolean(false) }
    };
    
    enum TokenType {
        ARRAY_START,
        ARRAY_END,
        SEPARATOR,
        
        OBJECT_START,
        OBJECT_DIVIDER,
        OBJECT_END,
        
        STRING,
        NUMBER,
        OTHER
    };
    
    bool isWhitespaceChar(char c) {
        return std::isspace(c);
    }
    
    bool isLetterCharStart(char c) {
        return std::isalpha(c);
    }
    
    bool isLetterCharCont(char c) {
        return std::isalnum(c) || c == '_';
    }
    
    bool isNumberChar(char c) {
        return std::isalnum(c) || c == '.';
    }
    
    bool isTerminated(const std::string& text, int pos, char strstart) {
        assert(pos > 0);
        return text[pos] == strstart && text[pos - 1] != '\\';
    }

    bool consumeComment(const std::string& text, int& pos) {
        return false;
    }
    
    struct Token {
        TokenType type;
        int startInd;
        int afterEnd;
    };
    
    class Parser {
    public:
        
        Parser(const std::string& tref, int p) :
            text(tref), pos(p)  {}
        
        const std::string& text;
        int pos;
        
        Value parse(Token tok) {
            
            enum {
                START,
                NEED_VAL,
                NEED_SEP
            } rstate;
            
            switch (tok.type) {
                    
                case ARRAY_START: {
                    rstate = START;
                    std::vector<Value> val_array;
                    
                    while (true) {
                        Token nt = next();
                        
                        if (nt.type == ARRAY_END) {
                            if (rstate == NEED_VAL)
                                throw std::range_error(nbl::utilities::format("Json::Parser::parse() cannot parse {%c} at loc %i, expected value.",
                                                                 text[tok.startInd], tok.startInd));
                            else break;
                            
                        } else {
                            
                            if (rstate == NEED_SEP) {
                                
                                if (nt.type == SEPARATOR) {
                                    rstate = NEED_VAL;
                                } else {
                                    throw std::range_error(nbl::utilities::format("Json::Parser::parse() cannot parse {%c} at loc %i",
                                                                     text[tok.startInd], tok.startInd));
                                }
                                
                            } else {
                                //need val or start
                                val_array.push_back(parse(nt));
                                rstate = NEED_SEP;
                            }
                            
                        }
                    }
                    
                    return Array(val_array);
                }
                    
                case OBJECT_START: {
                    rstate = START;
                    std::map<std::string, Value> val_map;
                    
                    while (true) {
                        Token nt = next();
                        
                        if (nt.type == OBJECT_END) {
                            if (rstate == NEED_VAL)
                                throw std::range_error(nbl::utilities::format("Json::Parser::parse() cannot parse {%c} at loc %i, expected value.",
                                                                 text[tok.startInd], tok.startInd));
                            else break;
                            
                        } else {
                            
                            if (rstate == NEED_SEP) {
                                
                                if (nt.type == SEPARATOR) {
                                    rstate = NEED_VAL;
                                } else {
                                    throw std::range_error(nbl::utilities::format("Json::Parser::parse() cannot parse {%c} at loc %i",
                                                                     text[tok.startInd], tok.startInd));
                                }
                                
                            } else {
                                //need val or start
                                
                                Value key = parse(nt);
                                if (key.type() != StringType)
                                    throw std::range_error(nbl::utilities::format("Json::Parser::parse() cannot parse {%c} at loc %i, key MUST be string",
                                                                     text[tok.startInd], tok.startInd));
                                
                                Token div = next();
                                if (div.type != OBJECT_DIVIDER)
                                    throw std::range_error(nbl::utilities::format("Json::Parser::parse() cannot parse {%c} at loc %i, need divider.",
                                                                     text[tok.startInd], tok.startInd));
                                
                                Token valt = next();
                                Value val = parse(valt);
                                val_map[key.asString()] = val;
                                
                                rstate = NEED_SEP;
                            }
                            
                        }
                    }
                    
                    return Object(val_map);
                }
                    
                case STRING: {
                    //int after_escape = tok.startInd + 1;
                    std::string string = text.substr(tok.startInd, tok.afterEnd - tok.startInd);
                    
                    return String(string);
                }
                    
                case NUMBER: {
                    std::string numrep = text.substr(tok.startInd, tok.afterEnd - tok.startInd);
                    return Number(numrep);
                }
                    
                case OTHER: {
                    std::string other = text.substr(tok.startInd, tok.afterEnd - tok.startInd);
                    if (RESERVED_STRINGS.find(other) == RESERVED_STRINGS.end()) {
                        return String(other);
                    } else {
                        return RESERVED_STRINGS[other];
                    }
                }
                    
                default:
                    throw std::range_error(nbl::utilities::format("Json::Parser::parse() cannot parse {%c} at loc %i",
                                                     text[tok.startInd], tok.startInd));
            }
        }

        void skip() {
            bool comment = false;
            for (; pos < text.size(); ++pos) {
                if (comment) {
                    if (text[pos] == '\n') comment = false;
                } else {
                    if (isWhitespaceChar(text[pos]))
                        continue;
                    if (text[pos] == '#') {
                        comment = true;
                        continue;
                    }

                    return;
                }
            }
        }
        
        Token next() {
            Token ret;
            //for (; pos < text.size() && isWhitespaceChar(text[pos]); ++pos);
            skip();
            
            if ( pos >= text.size() || text[pos] == '\0') {
                throw std::range_error("Json::Parser::next() expected character but reached end of input");
            }
            
            char c1 = text[pos];
            ret.startInd = pos;
            ret.afterEnd = pos + 1;
            
            pos = ret.afterEnd;
            
            switch (c1) {
                case '[':
                    ret.type = ARRAY_START;
                    return ret;
                    
                case ']':
                    ret.type = ARRAY_END;
                    return ret;
                    
                case ',':
                    ret.type = SEPARATOR;
                    return ret;
                    
                case '{':
                    ret.type = OBJECT_START;
                    return ret;
                    
                case ':':
                    ret.type = OBJECT_DIVIDER;
                    return ret;
                    
                case '}':
                    ret.type = OBJECT_END;
                    return ret;
                    
                case '"':
                case '\'':
                    ret.type = STRING;
                    for (; ret.afterEnd < text.size() &&
                         !isTerminated(text, ret.afterEnd, c1); ++ret.afterEnd) { }
                    
                    if (ret.afterEnd >= text.size())
                        throw std::range_error("Json::Parser::next() expected character but reached end of input");
                    assert(text[ret.afterEnd] == c1);
                    
                    ++ret.startInd;
                    pos = ret.afterEnd + 1;
                    return ret;
                    
                default: {
                    
                    if (std::isdigit(c1) || c1 == '-') {
                        //number
                        ret.type = NUMBER;
                        for (; ret.afterEnd < text.size() && isNumberChar(text[ret.afterEnd]); ++ret.afterEnd);
                        
                        pos = ret.afterEnd;
                        return ret;
                        
                    } else if (isLetterCharStart(c1)){
                        //unescaped string OR reserved word
                        ret.type = OTHER;
                        for (; ret.afterEnd < text.size() && isLetterCharCont(text[ret.afterEnd]); ++ret.afterEnd);
                        
                        pos = ret.afterEnd;
                        return ret;
                        
                    } else {
                        throw std::range_error(nbl::utilities::format("Json::Parser::next() found unexpected character {%c}", c1));
                    }
                }
            }
        }
    };

    //parse and return the first json value in 'text.'
    Value parse(const std::string& text) {
        return parseFrom(text, 0, NULL);
    }
    
    //parse and return the first json value in 'text.'
    //set end to index of first character after returned json value.
    Value parse(const std::string& text, int * end) {
        return parseFrom(text, 0, end);
    }
    
    //parse and return the first json value in 'text,' starting
    //at character text[from]
    Value parseFrom(const std::string& text, int from) {
        return parseFrom(text, from, NULL);
    }
    
    //parse and return the first json value in 'text,' starting
    //at character text[from], setting end to index of first character
    //after returned json value.
    Value parseFrom(const std::string& text, int from, int * end) {
        NBL_ASSERT_LT(from, text.size());
        NBL_ASSERT_GE(from, 0);

        Parser parser(text, from);
        Value found = parser.parse(parser.next());

        if (end) {
            *end = parser.pos;
        }

        return found;
    }

    NBL_ADD_TEST_TO(json_parse, json) {

        std::string son = "{key:null, key2:'whatttt',key5:5}";
        int end;
        Value val = parseFrom(son, 0, &end);

        NBL_ASSERT_EQ(val.type(), ObjectType);
        NBL_ASSERT_EQ(end, son.size())

        std::string son2 = "[1,2,3] null";
        Value v2 = parse(son2, &end);
        NBL_ASSERT_EQ(end, 7)
        NBL_ASSERT_EQ(v2.asArray().size(), 3)

        Value v3 = parseFrom(son2, 7);
        NBL_ASSERT_EQ(v3.type(), NullType);

        return true;
    }

    NBL_ADD_TEST_TO(json_number, json) {
        Number n1("1");
        NBL_ASSERT_EQ(n1.asInt(), 1)
        NBL_ASSERT_EQ(n1.asDouble(), 1.00)
        NBL_ASSERT_EQ(n1.asFloat(), 1.00)
        NBL_ASSERT_EQ(n1.asLong(), 1L)

        Number n2("1000000000000000000000");
        NBL_ASSERT_THROW_OF( ::std::out_of_range, n2.asInt(), "out of range")

        Number n3(" 3.5");

        NBL_ASSERT_EQ(n3.asInt(), 3)
        NBL_ASSERT_EQ(n3.asDouble(), 3.5)

        return true;
    }
    
}












