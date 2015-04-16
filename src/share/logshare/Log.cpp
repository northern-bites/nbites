#include "Log.h"

namespace log {
    
    Log::Log(const std::string& lfrom, int32_t checksum, clock_t created, int version, SExpr contents) : _written(false), _refs(0) {
        std::vector<SExpr> list = {
            SExpr("nblog"),
            SExpr("from", lfrom),
            SExpr("checksum", checksum),
            SExpr("created", (long) created),
            SExpr("version", version),
            contents
        };
        _tree = SExpr(list);
    }
    
    Log::Log(std::string& desc) : _written(false), _refs(0) {
        ssize_t i = 0;
        SExpr * expr = SExpr::read(desc, i);
        if (expr != NULL) {
            _tree = *expr;
            delete expr;
        }
    }
    
    Log::Log(const SExpr& expr) :
    _tree(expr), _written(false), _refs(0)
    { }
    
    Log::Log(Log * old) : _written(false), _refs(0) {
        _tree = old->tree();
        _data = old->data();
        
    }
    
    bool Log::setTree(std::string desc) {
        ssize_t i = 0;
        SExpr * derived = SExpr::read(desc, i);
        
        if (derived) {
            _tree = *derived;
            delete derived;
            return true;
        }
        
        return false;
    }
    
    bool Log::setTree(const SExpr& expr) {
        _tree  = expr;
        return true;
    }
    
    std::string Log::description() {
        return _tree.serialize();
    }
    
    SExpr& Log::tree() {
        return _tree;
    }
    
    const std::string& Log::data() {
        return _data;
    }
    
    void Log::setData(const std::string& newd) {
        _data = newd;
    }
    
    /*
     IO operations
     */
    
#include "exactio.h"
    
    bool Log::write(int fd) {
        NBLassert(fd >= 0);
        std::string desc = this->description();
        
        uint32_t desc_hlen = (uint32_t) desc.size();
        uint32_t data_hlen = (uint32_t) _data.size();
        
        uint32_t desc_nlen = htonl(desc_hlen);
        uint32_t data_nlen = htonl(data_hlen);
        
        if (write_exact(fd, 4, &desc_nlen)) {
            return false;
        }
        if (write_exact(fd, desc_hlen, desc.data())) {
            return false;
        }
        if (write_exact(fd, 4, &data_nlen)) {
            return false;
        }
        if (data_hlen) {
            if (write_exact(fd, data_hlen, _data.data())) {
                return false;
            }
        }
        
        _written = true;
        return true;
    }
    
    bool Log::send(int sock) {
        NBLassert(sock >= 0);
        std::string desc = this->description();
        
        uint32_t desc_hlen = (uint32_t) desc.size();
        uint32_t data_hlen = (uint32_t) _data.size();
        
        uint32_t desc_nlen = htonl(desc_hlen);
        uint32_t data_nlen = htonl(data_hlen);
        
        if (send_exact(sock, 4, &desc_nlen)) {
            return false;
        }
        if (send_exact(sock, desc_hlen, desc.data())) {
            return false;
        }
        if (send_exact(sock, 4, &data_nlen)) {
            return false;
        }
        if (data_hlen) {
            if (send_exact(sock, data_hlen, _data.data())) {
                return false;
            }
        }
        
        _written = true;
        return true;
    }
    
    size_t Log::fullSize() {
        return _data.size() + description().size();
    }
    
    void Log::acquire() {
        NBLassert(_refs >= 0);
        ++_refs;
    }
    
    bool Log::release() {
        NBLassert(_refs > 0);
        --_refs;
        return _refs == 0;
    }
    
    bool Log::written() {
        return _written;
    }
    
    Log * Log::recv(int sock, double max_wait) {
        NBLassert(sock >= 0);
        
        uint32_t ndesc;
        uint32_t hdesc;
        
        uint32_t ndata;
        uint32_t hdata;
        
        if (recv_exact(sock, 4, &ndesc, IO_SEC_TO_BREAK)) {
            return NULL;
        }
        
        hdesc = ntohl(ndesc);
        char desc_buffer[hdesc];
        
        if (recv_exact(sock, hdesc, desc_buffer, IO_SEC_TO_BREAK)) {
            return NULL;
        }
        
        if (recv_exact(sock, 4, &ndata, IO_SEC_TO_BREAK)) {
            return NULL;
        }
        
        hdata = ntohl(ndata);
        char data_buffer[hdata];
        
        if (hdata) {
            if (recv_exact(sock, hdata, data_buffer, IO_SEC_TO_BREAK)) {
                
                return NULL;
            }
        }
        
        std::string desc(desc_buffer, hdesc);
        std::string data(data_buffer, hdata);
        
        Log* found = new Log(desc);
        if (found != NULL) {
            found->setData(data);
        }
        
        return found;
    }
}