#include "SExpr.h"
#include "nbdebug.h"

#include <ctype.h>
#include <iostream>
#include <algorithm>
#include <assert.h>

namespace nblog {
    
    const char SExpr::special[] = {' ', '(', ')', '\r', '\n', '\t'};
    
    /*
     ATOM CONSTRUCTORS
     */
    
    //atom sexpr from string
    SExpr::SExpr(const std::string& v) :
    _atom(true),
    _value(v),
    _list()
    {
        NBDEBUGs(SECTION_SEXPR, "atom SExpr(const std::string& v)\n");
    }
    
    SExpr::SExpr(const char * n) :
    _atom(true),
    _list()
    {
        _value = std::string(n);
        NBDEBUGs(SECTION_SEXPR, "atom SExpr(const char * n)\n");
    }
    
    //atom from int
    SExpr::SExpr(int v) :
    _atom(true),
    _list()
    {
        NBDEBUGs(SECTION_SEXPR, "atom SExpr(int v)\n");
        char buf[100];
        snprintf(buf, 100, "%i", v);
        _value = std::string(buf);
    }
    
    //atom from long
    SExpr::SExpr(long v) :
    _atom(true),
    _list()
    {
        NBDEBUGs(SECTION_SEXPR, "atom SExpr(long v)\n");
        char buf[100];
        snprintf(buf, 100, "%li", v);
        _value = std::string(buf);
    }
    
    //atom from double
    SExpr::SExpr(double v) :
    _atom(true),
    _list()
    {
        NBDEBUGs(SECTION_SEXPR, "atom SExpr(double v)\n");
        char buf[100];
        snprintf(buf, 100, "%f", v);
        _value = std::string(buf);
    }
    
    /*
     LIST CONSTRUCTORS
     */

    //standard content item initializer
    SExpr::SExpr(const std::string& type,
                 const std::string& from, clock_t created,
                 size_t image_index, size_t nbytes) :
    _atom(false),
    _value("")
    {
        _list = {
            SExpr("type", type),
            SExpr("from", from),
            SExpr("when", (long) created),
            SExpr("iindex", (long) image_index),
            SExpr("nbytes", (long) nbytes)
        };
    }
    
    //list sexpr from vector
    SExpr::SExpr(const std::vector<SExpr>& l) :
    _atom(false),
    _value(""),
    _list(l)
    {
        NBDEBUGs(SECTION_SEXPR, "SExpr(const std::vector<SExpr>& l)\n");
    }
    
    //emtpy list
    SExpr::SExpr() :
    _atom(false),
    _value(""),
    _list()
    {
        NBDEBUGs(SECTION_SEXPR, "SExpr()\n");
    }
    
    SExpr::SExpr(const std::string& key, SExpr& val) :
    _atom(false),
    _value("")
    {
        _list = {SExpr(key), val};
    }
    
    SExpr::SExpr(const std::string& key, const std::string& val) :
    _atom(false),
    _value("")
    {
        _list = {SExpr(key), SExpr(val)};
    }
    
    SExpr::SExpr(const std::string& key, int val)  :
    _atom(false),
    _value("")
    {
        _list = {SExpr(key), SExpr(val)};
    }
    
    SExpr::SExpr(const std::string& key, long val)  :
    _atom(false),
    _value("")
    {
        _list = {SExpr(key), SExpr(val)};
    }
    
    SExpr::SExpr(const std::string& key, double val)  :
    _atom(false),
    _value("")
    {
        _list = {SExpr(key), SExpr(val)};
    }
    
    SExpr::SExpr(const std::string& key, int index, int cval) :
    _atom(false),
    _value("")
    {
        _list = {SExpr(key), SExpr(index), SExpr(cval)};
    }
    
    /*
     Instance methods
     */
    
    SExpr * SExpr::get(int i)
    {
        if (i < _list.size())
        {
            return &_list[i];
        }
        else return NULL;
    }
    
    void SExpr::append(const std::vector<SExpr>& l)
    {
        if (_atom) return;
        
        _list.insert(_list.end(), l.begin(), l.end());
    }
    
    void SExpr::append(const SExpr s)
    {
        if (_atom) return;
        
        _list.push_back(s);
    }
    
    std::string SExpr::serialize()
    {
        if (_atom)
        {
            if (_value.find_first_of(special) == std::string::npos)
                return _value;
            else {
                //Handle internal quotes.
                std::string replaced = _value;
                size_t lpos = replaced.find_first_of("\"", 0);
                
                while (lpos != std::string::npos) {
                    replaced.replace(lpos, 1, "\"\"");
                    
                    //Ignore the new ""
                    lpos = replaced.find_first_of("\"", lpos + 2);
                }
                
                //Now, wrap in more quotes!
                char buffer[replaced.size() + 2 + 1];
                assert(snprintf(buffer, replaced.size() + 2 + 1,
                                "\"%s\"", replaced.c_str())
                       == replaced.size() + 2);
                
                return std::string(buffer);
            }
        }
        
        std::string s = "(";
        for (unsigned int i=0; i < _list.size(); i++) {
            if (s.length() > 1)
                s += ' ';
            
            s += _list[i].serialize();
        }
        s += ")";
        
        return s;
    }
    
    std::string SExpr::print(int indent, int lineLimit, int level)
    {
        std::string prefix(indent * level, ' ');
        
        std::string s = serialize();
        if (_atom || (int) s.length() + indent * level <= lineLimit)
            return prefix + s;
        
        s = prefix + '(';
        for (unsigned int i = 0; i < _list.size(); i++)
        {
            std::string e = _list[i].print(indent, lineLimit, level+1);
            if (i>0)
            {
                s += "\n";
                s += e;
            }
            else
                s += e.substr(indent * level + 1);
        }
        s += "\n";
        s += prefix;
        s += ")";
        return s;
    }
    
    SExpr * SExpr::find(std::string name)
    {
        if (!_atom)
        {
            for (unsigned int i = 0; i < _list.size(); i++)
            {
                SExpr& exp = _list[i];
                if (exp.count() > 0 && exp.get(0)->isAtom()
                    && exp.get(0)->value() == name)
                    return &exp;
            }
        }
        
        return NULL;
    }
    
    /*
     Class method to read from string.
     */
    
    SExpr * SExpr::read(std::string s, ssize_t& p)
    {
        while (p < (int)s.length() && isspace(s[p]))
            p++;
        if (p >= s.size())
            return NULL;
        
        switch (s[p]) {
            case '"':
            {
                // Atoms starting with "
                std::string value;
                while (p < (int)s.length() && s[p] == '"')
                {
                    if (value.size() > 0)
                        //We're reading an escaped "
                        value += '"';
                    
                    size_t q = s.find('"', ++p);
                    if (q == std::string::npos)
                        q = s.size();
                    
                    value += s.substr(p, q - p);
                    
                    if (q < s.size())
                        p = q + 1;  //move past this "
                    else p = q;     //already at the end of s
                }
                
                return new SExpr(value);
            }
                
            case '(':
            {
                ++p;
                std::vector<SExpr> list;
                
                while (true)
                {
                    SExpr * e = read(s, p);
                    if (e != NULL) {
                        list.push_back(*e);
                        delete e;
                    } else {
                        break;
                    }
                }
                
                //This should always be true, since the above list
                //should have terminated with the below case.
                if (p < (int)s.length() && s[p] == ')')
                    p++;
                
                return new SExpr(list);
            }
                
            case ')':
            {
                return NULL;
            }
                
            default:
            {
                // Atoms not starting with ""
                //size_t q = findSpecialChar(s, p);
                size_t q = s.find_first_of(special, p);
                if (q == std::string::npos)
                    q = s.length();
                std::string value2 = s.substr(p, q - p);
                p = q;
                
                return new SExpr(value2);
            }
        }
    }
    
    /*
     int main(int argc, char** argv)
     {
     std::string test = "(image (height (hurry hurry 55) (\"W(HY)??? \" lolajk???sdalfjksd?? )) (width 99))";
     int i = 0;
     SExpr s = SExpr::read(test, i);
     std::cout << "Finding" << std::endl;
     SExpr found = s.find("height").find("\"W(HY)??? \"");
     
     std::cout << s.print() << std::endl << std::endl;
     std::cout << found.print() << std::endl;
     } */
    
}
