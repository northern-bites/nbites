#include "Log.h"

namespace nblog {
    
    nbhost_e HOST_TYPE = UNKNOWN;
    std::string HOST_NAME = "";
    
    int32_t getChecksum(const std::string& data) {
        int32_t sum = 0;
        for (int i = 0; i < data.size(); ++i)
            sum += (uint8_t) data[i];
        
        return sum;
    }
    
    Log Log::simple(const std::string type, const std::string data) {
        return Log::ofTypeWithFields(type, data, {});
    }
    
    Log Log::ofType(const std::string type, const std::string data) {
        return Log::ofTypeWithFields(type, data, {});
    }
    
    Log Log::ofTypeWithFields(const std::string type, const std::string data, std::initializer_list<SExpr> fields) {
        std::vector<SExpr> scv = {
            SExpr(CONTENT_TYPE_S, type)
        };
        
        for (auto it = fields.begin();
             it != fields.end();
             ++it) {
            scv.push_back(*it);
        }
        
        SExpr sc(scv);
        
        std::vector<SExpr> contv = {
            SExpr(LOG_CONTENTS_S),
            sc
        };
        SExpr contents(contv);
        
        std::vector<SExpr> tv = {
            SExpr(LOG_FIRST_ATOM_S),
            contents
        };
        
        SExpr top(tv);
        
        Log ret;
        ret.setTree(top);
        ret.setData(data);
        
        return ret;
    }
    
    Log Log::withContentItems(std::initializer_list<SExpr> items, const std::string data) {
        std::vector<SExpr> contv = {
            SExpr(LOG_CONTENTS_S),
        };
        
        for (auto it = items.begin();
             it != items.end();
             ++it) {
            contv.push_back(*it);
        }

        
        SExpr contents(contv);
        
        std::vector<SExpr> tv = {
            SExpr(LOG_FIRST_ATOM_S),
            contents
        };
        
        SExpr top(tv);
        
        Log ret;
        ret.setTree(top);
        ret.setData(data);
        
        return ret;
    }
    
    Log::Log() :
    _written(false),
    _refs(0)
    {
        
    }
    
    void Log::generic(const std::string& log_class,
                  const std::string& where_made,
                  time_t when_made,
                  int version,
                  const std::vector<SExpr>& contents_list)
    {
        int32_t cs = getChecksum(_data);
        
        tm * ptm = localtime(&when_made);
        char buffer[100];
        strftime(buffer, 100, "%d.%m.%Y %H:%M:%S", ptm);
        std::string time(buffer);
        
        std::vector<SExpr> made_list = {
            SExpr(LOG_CREATED_S),
            SExpr(where_made),
            SExpr(time)
        };
        
        std::vector<SExpr> clist = {
            SExpr(LOG_CONTENTS_S)
        };
        clist.insert(clist.end(), contents_list.begin(), contents_list.end());
        
        std::vector<SExpr> keys = {
            SExpr(log_class),           //atom
            SExpr(made_list),           //list
            SExpr(LOG_VERSION_S, version),  //key-value list
            SExpr(LOG_CHECKSUM_S, cs),      //key-value list
            SExpr(clist)                //list, first is "contents" atom
        };
        
        if (HOST_TYPE == V5ROBOT) {
            keys.push_back(SExpr(LOG_HOST_TYPE_S, "V5ROBOT"));
        } else if (HOST_TYPE == V4ROBOT) {
            keys.push_back(SExpr(LOG_HOST_TYPE_S, "V4ROBOT"));
        } else {
            keys.push_back(SExpr(LOG_HOST_TYPE_S, "unknown"));
        }
        
        if (HOST_NAME != "") {
            keys.push_back(SExpr(LOG_HOST_NAME_S, HOST_NAME));
        }
        
        _tree = SExpr(keys);
    }
    
    Log::Log(const std::string& log_class,
             const std::string& where_made,
             time_t when_made,
             int version,
             const std::vector<SExpr>& contents_list,
             const std::string& contents_data) :
    _written(false),
    _refs(0),
    _data(contents_data)
    {
        generic(log_class, where_made, when_made, version, contents_list);
    }
    
    Log::Log(const std::string& log_class,
             const std::string& where_made,
             time_t when_made,
             int version,
             const std::vector<SExpr>& contents_list,
             const void * buffer, size_t nbytes) :
        _written(false),
        _refs(0),
        _data( (const char *) buffer, nbytes)
    {
        generic(log_class, where_made, when_made, version, contents_list);
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

        // printf("SENDING LOG: %u desc, %u data\n",
        //     desc_hlen, data_hlen);
        
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