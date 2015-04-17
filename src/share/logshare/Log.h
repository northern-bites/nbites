#ifndef nbites_Log
#define nbites_Log

#include <string>
#include "SExpr.h"

/*
 Every log has two components:
   * a set of opaque data – byes – encoding some arbitrary set of information such as pixels or a protobuf.
   * a Lisp s-expression tree structure composed of s-expressions and plain text, which describes, provides context for, and outlines the log.
 
 In this class, the internal representation of the description is always an sexpr tree, though it may be updated with either format.
 */

namespace logshare {
    
    class Log {
    public:
        
        /*
         class manipulators.
         
         constructors
         */
        Log(const std::string& lfrom, int32_t checksum, clock_t created, int version, SExpr contents);
        
        Log(std::string& description);
        Log(const SExpr& expr);
        Log(Log * old); //copies argument
        
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
        size_t fullSize();
        void acquire();
        bool release(); //returns true if ( (--_refs) == 0 )
        bool written();
        
        //returns NULL on failure.
        static Log * recv(int sock, double max_wait);
        
    private:
        //actual internal data structures.
        SExpr _tree;
        std::string _data;
        
        //io stuff
        volatile int _refs;
        volatile bool _written;
    };
}

#endif