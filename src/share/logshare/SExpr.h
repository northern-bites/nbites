
#ifndef nbites_SExpr
#define nbites_SExpr

#include <string>
#include <vector>
#include <stdexcept>
#include <iostream>

/*
 an sexpr is either an atom or a list of sexprs.  Therefore only one of the two fields {_value, _list} is ever relevant.
 
 An atom may have empty value: ""
 A list may have no items: ()
 
 Both are valid sexprs.  If a non-safe find or indexing operation CANNOT complete, it returns NULL,
 */

namespace nblog {
    
    typedef enum {
        SEXPR_ATOM,
        SEXPR_LIST,
        SEXPR_NOTFOUND
    } SExprType;
    
    class SExpr {
    public:
        
        /**** standard constructors */
        
        /* copy constructor: same as what the compiler would provide */
        
        SExpr( const SExpr& other );
        SExpr& operator=( const SExpr& other );
        
        //Constructor for a generic content item.
        //use .append() to add more specific keys
        SExpr(const std::string& type,
              const std::string& from, clock_t created,
              size_t image_index, size_t nbytes);
        
        //list node with elements in l
        SExpr(const std::vector<SExpr>& l);
        //empty LIST.
        SExpr();
        //atom with value n
        SExpr(const std::string& n);
        SExpr(const char * n);
        
        //more atoms.
        SExpr(int v);
        SExpr(long v);
        SExpr(double v);
        
        //key-value constructors
        //these make a LIST with two atoms, (key val)
        SExpr(const std::string& key, SExpr& val);
        SExpr(const std::string& key, const std::string& val);
        SExpr(const std::string& key, int val);
        SExpr(const std::string& key, long val);
        SExpr(const std::string& key, double val);
        
        //key double value for use in log_main.
        SExpr(const std::string& key, int index, int cval);
        
        /**** factory methods to stack */
        
        static SExpr atom(const std::string& n) {return SExpr(n);}
        static SExpr atom(const char * n) {return SExpr(n);}
        
        //more atoms.
        static SExpr atom(int v) {return SExpr(v);}
        static SExpr atom(long v) {return SExpr(v);}
        static SExpr atom(double v) {return SExpr(v);}
        
        static SExpr list() {return SExpr();}
        static SExpr list(std::initializer_list<SExpr> exprs) {
            std::vector<SExpr> vec(exprs);
            return SExpr(vec);
        }
        
        static SExpr keyValue(const std::string& key, SExpr& val) {
            return SExpr(key, val);
        }
        
        static SExpr keyValue(const std::string& key, const std::string& val) {
            return SExpr(key, val);
        }
        
        static SExpr keyValue(const std::string& key, int val) {
            return SExpr(key, val);
        }
        
        static SExpr keyValue(const std::string& key, long val) {
            return SExpr(key, val);
        }
        
        static SExpr keyValue(const std::string& key, double val) {
            return SExpr(key, val);
        }
        
        /**** factory methods to heap: requires `delete`! */
        //Translate the given string into an SExpression
        static SExpr * read(const std::string s, ssize_t& p);
        static SExpr * read(const std::string s) {
            ssize_t start = 0;
            return SExpr::read(s, start);
        }
        
        /**** instance type modifiers */
        void setList(const std::vector<SExpr>& newContents);
        void setList(std::initializer_list<SExpr> exprs);
        void setAtom(std::string val);
        void setAtomAsCopy(SExpr atomToCopy);
        
        /**** list contents modifiers */
        void insert(int index, SExpr& inserted);
        bool remove(int index);
        void append(const std::vector<SExpr>& l);
        void append(const SExpr s);
        
        bool readAndAppend(std::string expr) {
            ssize_t i = 0;
            SExpr * found = SExpr::read(expr, i);
            
            if (!found)
                return false;
            else {
                this->_type  = SEXPR_LIST;
                this->_value = "";
                this->append(*found);
                delete found;
                return true;
            }
        }
        
        /**** list retrieval */
        ssize_t count() { return (_type == SEXPR_LIST) ? _list.size() : -1; }
        SExpr * get(int i);
        SExpr & safeGet(int i);
        std::vector<SExpr> * getList();
        
        /**** list find operations */
        SExpr * find(const std::string name);
        SExpr & safeFind(const std::string name);
        
        //Returns equivalent of find(name)->get(1)
        SExpr * firstValueOf(const std::string name);
        SExpr & safeFirstValueOf(const std::string name);
        
        //return represents path from (this) to target
        std::vector<SExpr *> recursiveFind(const std::string name);
        std::vector<std::vector<SExpr *>> recursiveFindAll(const std::string name);
        
        /**** atom value retrieval */
        const std::string value();
        int valueAsInt();
        long valueAsLong();
        double valueAsDouble();
        
        /**** type retrieval */
        bool isAtom() { return _type == SEXPR_ATOM; }
        bool isList() { return _type == SEXPR_LIST; }
        bool exists() const {return _type != SEXPR_NOTFOUND;}
        
        SExprType type() {return _type;}
        
        /**** conversion to strings */
        
        // String representation of this SExpression (and nested ones)
        const std::string serialize();
        
        // Nicely formatted SExpression (more human-readable)
        const std::string print(int indent = 2, int lineLimit = 64, int level = 0);
        
        // Special chars that we need to look out for
        static const char special[];
    private:
        SExprType _type;
        // Describes the expression, only non-empty if atom
        std::string _value;
        // List of nested expressions, only non-empty if not atom
        std::vector<SExpr> _list;
        
        static SExpr NOT_FOUND;
        static SExpr createNotFound();
    };
}

#endif
