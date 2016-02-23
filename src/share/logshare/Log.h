#ifndef nbites_Log
#define nbites_Log

#include <string>
#include <initializer_list>
#include "SExpr.h"

/*
 Every log has two components:
   * a set of opaque data – byes – encoding some arbitrary set of information such as pixels or a protobuf.
   * a Lisp s-expression tree structure composed of s-expressions and plain text, which describes, provides context for, and outlines the log.
 
 In this class, the internal representation of the description is always an sexpr tree, though it may be updated with either format.
 */

namespace nblog {
    
    enum nbhost_e {
        V5ROBOT,
        V4ROBOT,
        UNKNOWN
    };
    
    extern nbhost_e HOST_TYPE;
    extern std::string HOST_NAME;
    
    class Log {
    public:
        
        static Log simple(const std::string type, const std::string data);
        static Log ofType(const std::string type, const std::string data);
        static Log ofTypeWithFields(const std::string type, const std::string data, std::initializer_list<SExpr> fields);
        
        static Log withContentItems(std::initializer_list<SExpr> items, const std::string data);
        
        //default constructor
        Log();
        
        //Generic log constructor with standard values.
        Log(const std::string& log_class,
            const std::string& where_made,
            time_t when_made,
            int version,
            const std::vector<SExpr>& contents_list,
            const std::string& contents_data);
        
        Log(const std::string& log_class,
            const std::string& where_made,
            time_t when_made,
            int version,
            const std::vector<SExpr>& contents_list,
            const void * buffer, size_t nbytes);
        
        //Parses the SExpr expected to be in description.
        Log(std::string& description);
        //Uses expr as new tree
        Log(const SExpr& expr);
        //copies argument
        Log(Log * old);
        
        bool setTree(std::string desc);
        bool setTree(const SExpr& expr);
    
        std::string description();  //serialized, not pretty.
        SExpr& tree();
        
        //Reference to data of this log.
        const std::string& data();
        
        //Data copied.
        void setData(const std::string& newd);
        
        /*
         IO operations
         
         return indicates success
         */
        
        bool write(int fd); //fd is filedescriptor
        bool send(int sock);//sock is socket
        
        //io stuff
        size_t fullSize();  //serialize.length() + data.length()
        void acquire();
        bool release();     //returns true if ( (--_refs) == 0 )
        bool written();
        
        //returns NULL on failure.
        static Log * recv(int sock, double max_wait);
        
    private:
        //actual internal data structures.
        SExpr _tree;
        std::string _data;
        
        //io stuff
        volatile bool _written;
        volatile int _refs;
        
        void generic(const std::string& log_class,
                      const std::string& where_made,
                      time_t when_made,
                      int version,
                      const std::vector<SExpr>& contents_list);
    };
}

#endif