#include "SExpr.h"
#include "nbdebug.h"

#include <ctype.h>
#include <iostream>
#include <algorithm>
#include <assert.h>
#include <map>
#include <queue>

namespace nblog {
    
    SExpr SExpr::createNotFound() {
        
        SExpr var;
        var._type = SEXPR_NOTFOUND;
        return var;
    }
    
    SExpr SExpr::NOT_FOUND = SExpr::createNotFound();
    
    SExpr::SExpr( const SExpr& other ) :
    _type(other._type), _list( other._list ), _value( other._value)
    {}
    
    SExpr& SExpr::operator=( const SExpr& other ) {
        _type = other._type;
        _value = other._value;
        _list = other._list;
        return *this;
    }
    
    //atom sexpr from string
    SExpr::SExpr(const std::string& v) :
    _type(SEXPR_ATOM),
    _value(v),
    _list()
    {
        NBDEBUGs(SECTION_SEXPR, "atom SExpr(const std::string& v)\n");
    }
    
    SExpr::SExpr(const char * n) :
    _type(SEXPR_ATOM),
    _list(),
    _value(n)
    {
        NBDEBUGs(SECTION_SEXPR, "atom SExpr(const char * n)\n");
    }
    
    //atom from int
    SExpr::SExpr(int v) :
    _type(SEXPR_ATOM),
    _list()
    {
        NBDEBUGs(SECTION_SEXPR, "atom SExpr(int v)\n");
        char buf[100];
        snprintf(buf, 100, "%i", v);
        _value = std::string(buf);
    }
    
    //atom from long
    SExpr::SExpr(long v) :
    _type(SEXPR_ATOM),
    _list()
    {
        NBDEBUGs(SECTION_SEXPR, "atom SExpr(long v)\n");
        char buf[100];
        snprintf(buf, 100, "%li", v);
        _value = std::string(buf);
    }
    
    //atom from double
    SExpr::SExpr(double v) :
    _type(SEXPR_ATOM),
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
    _type(SEXPR_LIST),
    _value("")
    {
        _list = {
            SExpr(CONTENT_TYPE_S, type),
            SExpr(CONTENT_FROM_S, from),
            SExpr(CONTENT_WHEN_S, (long) created),
            SExpr(CONTENT_IINDEX_S, (long) image_index),
            SExpr(CONTENT_NBYTES_S, (long) nbytes)
        };
    }
    
    //list sexpr from vector
    SExpr::SExpr(const std::vector<SExpr>& l) :
    _type(SEXPR_LIST),
    _value(""),
    _list(l)
    {
        NBDEBUGs(SECTION_SEXPR, "SExpr(const std::vector<SExpr>& l)\n");
    }
    
    //emtpy list
    SExpr::SExpr() :
    _type(SEXPR_LIST),
    _value(""),
    _list()
    {
        NBDEBUGs(SECTION_SEXPR, "SExpr()\n");
    }
    
    SExpr::SExpr(const std::string& key, SExpr& val) :
    _type(SEXPR_LIST),
    _value("")
    {
        _list = {SExpr(key), val};
    }
    
    SExpr::SExpr(const std::string& key, const std::string& val) :
    _type(SEXPR_LIST),
    _value("")
    {
        _list = {SExpr(key), SExpr(val)};
    }
    
    SExpr::SExpr(const std::string& key, int val)  :
    _type(SEXPR_LIST),
    _value("")
    {
        _list = {SExpr(key), SExpr(val)};
    }
    
    SExpr::SExpr(const std::string& key, long val)  :
    _type(SEXPR_LIST),
    _value("")
    {
        _list = {SExpr(key), SExpr(val)};
    }
    
    SExpr::SExpr(const std::string& key, double val)  :
    _type(SEXPR_LIST),
    _value("")
    {
        _list = {SExpr(key), SExpr(val)};
    }
    
    SExpr::SExpr(const std::string& key, int index, int cval) :
    _type(SEXPR_LIST),
    _value("")
    {
        _list = {SExpr(key), SExpr(index), SExpr(cval)};
    }
    
    SExpr::SExpr(const std::string& key, double roll, double pitch) :
    _type(SEXPR_LIST),
    _value("")
    {
        _list = {SExpr(key), SExpr(roll), SExpr(pitch)};
    }

    /*
     Instance methods
     */
    
    void SExpr::setList(const std::vector<SExpr>& newContents) {
        if (_type == SEXPR_NOTFOUND) {
            throw std::domain_error("sexpr not found");
        }
        _type = SEXPR_LIST;
        _value = "";
        _list = newContents;
    }
    
    void SExpr::setList(std::initializer_list<SExpr> exprs) {
        if (_type == SEXPR_NOTFOUND) {
            throw std::domain_error("sexpr not found");
        }
        _type = SEXPR_LIST;
        _value = "";
        _list = std::vector<SExpr>(exprs);
    }
    
    void SExpr::setAtom(std::string val) {
        if (_type == SEXPR_NOTFOUND) {
            throw std::domain_error("sexpr not found");
        }
        _type = SEXPR_ATOM;
        _value = val;
        _list = {};
    }
    
    void SExpr::setAtomAsCopy(SExpr atomToCopy) {
        if (_type == SEXPR_NOTFOUND) {
            throw std::domain_error("sexpr not found");
        }
        _type = SEXPR_ATOM;
        _value = atomToCopy.value();
        _list = {};
    }
    
    void SExpr::insert(int index, SExpr& inserted) {
        if ((_type != SEXPR_LIST) || !inserted.exists())
            return;
        
        if (index < 0 || index > _list.size())
            return;
        
        auto it = _list.begin() + index;
        _list.insert(it, inserted);
    }
    
    bool SExpr::remove(int index) {
        if ((_type != SEXPR_LIST) || index < 0 || index > _list.size())
            return false;
        
        _list.erase(_list.begin() + index);
        return true;
    }

    const std::string SExpr::value() const {
        if (_type != SEXPR_ATOM) throw std::domain_error("sexpr is not atom");
        return _value;
    }
    
    int SExpr::valueAsInt() const {
        if (_type != SEXPR_ATOM) throw std::domain_error("sexpr is not atom");
        return std::stoi(_value);
    }
    
    long SExpr::valueAsLong() const {
        if (_type != SEXPR_ATOM) throw std::domain_error("sexpr is not atom");
        return std::stol(_value);
    }
    
    double SExpr::valueAsDouble() const {
        if (_type != SEXPR_ATOM) throw std::domain_error("sexpr is not atom");
        return std::stod(_value);
    }
    
    SExpr * SExpr::get(int i)
    {
        if ( _type == SEXPR_LIST && i < _list.size())
        {
            return &_list[i];
        }
        else return NULL;
    }
    
    SExpr & SExpr::safeGet(int i) {
        SExpr * fnd = get(i);
        if (fnd) return *fnd;
        return NOT_FOUND;
    }
    
    std::vector<SExpr> * SExpr::getList() {
        if (_type != SEXPR_LIST)
            return NULL;
        return &_list;
    }
    
    void SExpr::append(const std::vector<SExpr>& l)
    {
        if (_type != SEXPR_LIST) return;
        
        _list.insert(_list.end(), l.begin(), l.end());
    }
    
    void SExpr::append(const SExpr s)
    {
        if (_type != SEXPR_LIST || !s.exists()) return;
        
        _list.push_back(s);
    }
    
    const char SExpr::special[] = {' ', '(', ')', '\r', '\n', '\t', '\0'};
    
    const std::string SExpr::serialize() const
    {
        if (_type == SEXPR_NOTFOUND) {
            return "NOT_FOUND";
        }
        
        if (_type == SEXPR_ATOM)
        {
            if (_value.find_first_of(special) == std::string::npos) {
                NBDEBUGs(SECTION_SEXPR, "%i: returning [%s]\n", __LINE__, _value.c_str());
                return _value;
            } else {
                NBDEBUGs(SECTION_SEXPR, "special at %i\n", _value.find_first_of(special));
                
                //Handle internal quotes.
                std::string replaced = _value;
                size_t lpos = replaced.find_first_of("\"", 0);
                
                while (lpos != std::string::npos) {
                    replaced.replace(lpos, 1, "\"\"");
                    
                    //Ignore the new ""
                    lpos = replaced.find_first_of("\"", lpos + 2);
                }
                
                NBDEBUGs(SECTION_SEXPR, "replaced to [%s]\n", replaced.c_str());
                
                //Now, wrap in more quotes!
                char buffer[replaced.size() + 2 + 1];
                NBLassert(snprintf(buffer, replaced.size() + 2 + 1,
                                "\"%s\"", replaced.c_str())
                       == replaced.size() + 2);
                
                std::string returned(buffer);
                NBDEBUGs(SECTION_SEXPR, "%i: returning [%s]\n", __LINE__, returned.c_str());
                return returned;
            }
        }
        
        std::string s = "(";
        for (unsigned int i=0; i < _list.size(); i++) {
            if (s.length() > 1)
                s += ' ';
            
            s += _list[i].serialize();
        }
        s += ")";
        
        NBDEBUGs(SECTION_SEXPR, "%i: returning [%s]\n", __LINE__, s.c_str());
        return s;
    }
    
    const std::string SExpr::print (int indent, int lineLimit, int level) const
    {
        if (_type == SEXPR_NOTFOUND) {
            return "NOT_FOUND";
        }
        
        std::string prefix(indent * level, ' ');
        
        std::string s = serialize();
        if ((_type == SEXPR_ATOM) || (int) s.length() + indent * level <= lineLimit)
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
    
    SExpr * SExpr::find(const std::string name)
    {
        if (_type == SEXPR_LIST)
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
    
    SExpr& SExpr::safeFind(const std::string name) {
        SExpr * fnd = find(name);
        if (fnd)
            return *fnd;
        return NOT_FOUND;
    }
    
    SExpr * SExpr::firstValueOf(const std::string name) {
        SExpr * fnd = find(name);
        if (fnd && fnd->count() > 1) {
            return fnd->get(1);
        }
        
        return NULL;
    }
    
    SExpr & SExpr::safeFirstValueOf(const std::string name) {
        SExpr * fnd = firstValueOf(name);
        if (fnd)
            return *fnd;
        return NOT_FOUND;
    }
    
    std::vector<SExpr *> traverse(std::map<SExpr *, SExpr *> & pmap, SExpr * start) {
        std::vector<SExpr *> ret;
        SExpr * cur = start;
        while (cur) {
            ret.insert(ret.begin(), cur);
            cur = pmap[cur];
        }
        
        return ret;
    }
 
    std::vector<std::vector<SExpr *>> internalRecursiveFind(const std::string key, int num, SExpr * start) {
        std::map<SExpr *, SExpr *> pmap;
        std::queue<SExpr *> queue;
        std::vector<std::vector<SExpr *>> retv;
        
        queue.push(start);
        pmap[start] = NULL;
        
        while (!queue.empty()) {
            SExpr * current = queue.front();
            queue.pop();
            
            for (int i = 0; i < current->count(); ++i) {
                SExpr * child = current->get(i);
                if (child->isAtom()) {
                    if (child->value() == key) {
                        retv.push_back(traverse(pmap, current));
                        
                        if (retv.size() == num)
                            return retv;
                    }
                } else {
                    pmap[child] = current;
                    queue.push(child);
                }
            }
        }
        
        return retv;
    }
    
    
    std::vector<SExpr *> SExpr::recursiveFind(const std::string name) {
        std::vector<SExpr *> could_not_complete;
        
        if (_type != SEXPR_LIST || _list.empty()) {
            return could_not_complete;
        }
        
        std::vector<std::vector<SExpr *>> matches = internalRecursiveFind(name, 1, this);
        
        if (matches.empty()) {
            return could_not_complete;
        }
        
        return matches[0];
    }
    
    std::vector<std::vector<SExpr *>> SExpr::recursiveFindAll(const std::string name) {
        std::vector<std::vector<SExpr *>> could_not_complete;
        
        if (_type != SEXPR_LIST || _list.empty())
            return could_not_complete;
        
        return internalRecursiveFind(name, INT32_MAX, this);
    }
    
    
    /*
     Class method to read from string.
     */
    SExpr * SExpr::read(const std::string s, ssize_t& p)
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
    
}
