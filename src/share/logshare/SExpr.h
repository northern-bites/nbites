
#ifndef nbites_SExpr
#define nbites_SExpr

#include <string>
#include <vector>

namespace nblog {
    
    /*
     an sexpr is either an atom or a list of sexprs.  Therefore only one of the two fields {value, list} is ever relevant.
     
     An atom may have empty value: ""
     A list may have no items: ()
    
     Both are valid sexprs.  If a find or indexing operation CANNOT complete, it returns NULL.
     
     */
    
    class SExpr {
    public:
        
        //Constructor for a generic content item.
        //use .append() to add more specific keys
        SExpr(const std::string& type,
              const std::string& from, clock_t created,
              size_t image_index, size_t nbytes);
        
        //list node with elements in l
        SExpr(const std::vector<SExpr>& l);
        //atom with value n
        SExpr(const std::string& n);
        //atom with empty value.
        SExpr();
        
        //key-value constructors
        //these make a LIST with two atoms, (key val)
        SExpr(const std::string& key, SExpr& val);
        SExpr(const std::string& key, const std::string& val);
        SExpr(const std::string& key, int val);
        SExpr(const std::string& key, long val);
        SExpr(const std::string& key, double val);
        
        bool isAtom() { return _atom; }
        ssize_t count() { return _atom ? -1 : _list.size(); }
        
        std::string value() { return _value; }
        SExpr * get(int i);
        
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
        
        //Translate the given string into an SExpression
        
        static SExpr * read(std::string s, ssize_t& p);
        
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
