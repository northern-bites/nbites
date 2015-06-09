
#ifndef nbites_SExpr
#define nbites_SExpr

#include <string>
#include <vector>
#include <stdexcept>

namespace nblog {
    
    /*
     an sexpr is either an atom or a list of sexprs.  Therefore only one of the two fields {value, list} is ever relevant.
     
     An atom may have empty value: ""
     A list may have no items: ()
    
     Both are valid sexprs.  If a find or indexing operation CANNOT complete, it returns NULL.
     
     */
    
    class SExpr {
    public:
        
        //Translate the given string into an SExpression
        static SExpr * read(const std::string s, ssize_t& p);
        static SExpr * read(const std::string s) {
            ssize_t start = 0;
            return SExpr::read(s, start);
        }
        
        /* copy constructor: same as what the compiler would provide */
        
        SExpr( const SExpr& other ) :
            _atom( other._atom ), _list( other._list ), _value( other._value)
        {}
        
        SExpr& operator=( const SExpr& other ) {
            _atom = other._atom;
            _value = other._value;
            _list = other._list;
            return *this;
        }
        
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
        
        /* MODIYFING AN EXISTING SEXPR (may change type)*/
        void setList(const std::vector<SExpr>& newContents);
        void setList(std::initializer_list<SExpr> exprs);
        void setAtom(std::string val);
        void setAtomAsCopy(SExpr atomToCopy);
        
        void insert(int index, SExpr& inserted);
        bool remove(int index);
        
        bool isAtom() { return _atom; }
        ssize_t count() { return _atom ? -1 : _list.size(); }
        
        std::string value();
        int valueAsInt();
        long valueAsLong();
        double valueAsDouble();
        
        //returns NULL if doesn't exist.
        SExpr * get(int i);
        std::vector<SExpr> * getList();
        
        // Adds the given list/single of SExpressions to this list
        // No effect if an atom
        void append(const std::vector<SExpr>& l);
        void append(const SExpr s);
        
        bool readAndAppend(std::string expr) {
            ssize_t i = 0;
            SExpr * found = SExpr::read(expr, i);
            
            if (!found)
                return false;
            else {
                this->append(*found);
                delete found;
                return true;
            }
        }
        
        // String representation of this SExpression (and nested ones)
        std::string serialize();
        
        // Nicely formatted SExpression (more human-readable)
        std::string print(int indent = 2, int lineLimit = 64, int level = 0);
        
        /*
         Returns a pointer to the first expression found whose first element is an atom with value name.
         Useful for finding the sexpr equivalent of keys.
         
         Returns NULL if:
            this is an atom or
            the value could not be found.
         */
        SExpr * find(std::string name);
        
        // Special chars that we need to look out for
        static const char special[];
        // Returns index of FIRST special char in string. returns string::npos if none found
    private:
        // True if an atom. Implies null=false, list->empty
        bool _atom;
        // Describes the expression, only non-empty if atom
        std::string _value;
        // List of nested expressions, only non-empty if not atom
        std::vector<SExpr> _list;
    };
    
}

#endif
