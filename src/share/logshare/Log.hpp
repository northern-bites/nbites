//
//  Log.hpp
//
//  Created by Philip Koch on 3/19/16.
//

#ifndef Log_hpp
#define Log_hpp

#include <stdio.h>

#include <string>
#include <iostream>
#include <memory>

#ifndef NBL_STANDALONE

#ifdef __APPLE__
#include <google/protobuf/message.h>
#include "Images.h"
#else 
#include "google/protobuf/message.h"
#include "../messages/Images.h"
#endif

#endif

#include "json.hpp"
#include "SExpr.h"
#include "nblogio.h"

#define LOG_VERSION (8)

namespace nbl {

#define NBL_CREATE_CONSTANT_METHOD(name)    \
    static const std::string name() { return get()->dict.at( std::string( #name ) ).asConstString(); }

#define NBL_CREATE_INT_CONSTANT_METHOD(name)    \
    static int name() { return get()->dict.at(std::string(#name)).asConstNumber().asInt(); }

    /* helper class for retrieving constants from json config file */
    /* see json file for values and comments */
    class SharedConstants {
        SharedConstants();
    public:
        const json::Object dict;

        static SharedConstants * theInstance;
        static SharedConstants * get();

        NBL_CREATE_CONSTANT_METHOD(YUVImageType_DEFAULT)
        NBL_CREATE_CONSTANT_METHOD(YUVImageType_Y16)
        NBL_CREATE_CONSTANT_METHOD(YUVImageType_Y8)

        NBL_CREATE_CONSTANT_METHOD(SexprType_DEFAULT)
        NBL_CREATE_CONSTANT_METHOD(JsonType_DEFAULT)
        NBL_CREATE_CONSTANT_METHOD(LogType_DEFAULT)

        NBL_CREATE_CONSTANT_METHOD(LogClass_Null)
        NBL_CREATE_CONSTANT_METHOD(LogClass_Flags)
        NBL_CREATE_CONSTANT_METHOD(LogClass_Tripoint)
        NBL_CREATE_CONSTANT_METHOD(LogClass_YUVImage)

        NBL_CREATE_CONSTANT_METHOD(LogClass_EdgeImage)
        NBL_CREATE_CONSTANT_METHOD(LogClass_String)

        NBL_CREATE_CONSTANT_METHOD(LogClass_RPC_Call)
        NBL_CREATE_CONSTANT_METHOD(LogClass_RPC_Return)
        NBL_CREATE_CONSTANT_METHOD(RPC_NAME)
        NBL_CREATE_CONSTANT_METHOD(RPC_KEY)

        NBL_CREATE_INT_CONSTANT_METHOD(ROBOT_PORT)
        NBL_CREATE_INT_CONSTANT_METHOD(CROSS_PORT)

        NBL_CREATE_INT_CONSTANT_METHOD(REMOTE_HOST_TIMEOUT)

        NBL_CREATE_CONSTANT_METHOD(ROBOT_LOG_PATH_PREFIX)

        NBL_CREATE_CONSTANT_METHOD(LOG_TOPLEVEL_MAGIC_KEY)

        NBL_CREATE_CONSTANT_METHOD(LOG_TOPLEVEL_BLOCKS)
        NBL_CREATE_CONSTANT_METHOD(LOG_TOPLEVEL_LOGCLASS)
        NBL_CREATE_CONSTANT_METHOD(LOG_TOPLEVEL_CREATED_WHEN)
        NBL_CREATE_CONSTANT_METHOD(LOG_TOPLEVEL_HOST_TYPE)
        NBL_CREATE_CONSTANT_METHOD(LOG_TOPLEVEL_HOST_NAME)
        NBL_CREATE_CONSTANT_METHOD(LOG_TOPLEVEL_HOST_ADDR)

        NBL_CREATE_CONSTANT_METHOD(LOG_BLOCK_TYPE)
        NBL_CREATE_CONSTANT_METHOD(LOG_BLOCK_WHERE_FROM)
        NBL_CREATE_CONSTANT_METHOD(LOG_BLOCK_WHEN_MADE)
        NBL_CREATE_CONSTANT_METHOD(LOG_BLOCK_IMAGE_INDEX)
        NBL_CREATE_CONSTANT_METHOD(LOG_BLOCK_NUM_BYTES)

        NBL_CREATE_CONSTANT_METHOD(LOG_BLOCK_IMAGE_WIDTH_PIXELS)
        NBL_CREATE_CONSTANT_METHOD(LOG_BLOCK_IMAGE_HEIGHT_PIXELS)
    };

    class Log;
    typedef std::shared_ptr<Log> logptr;

    //std::shared_ptr overloads the conversion-to-bool of naked pointers
    //therefore we can just use the shared_ptr for NULL checks
    // ... also this makes much clearer assertion failures...
#define IS_PTR_VALID(lp) (lp)

    //type for relating derived information to a specific image
    typedef ssize_t iindex_t;

    /* item of logging information, 1 or more go into every log */
    /* Log provides factory methods for standard Block types */

    static const iindex_t IMAGE_INDEX_INVALID = -1;

    class Block {
    public:
        std::string data;
        json::Object dict;

        std::string type;
        std::string whereFrom;
        iindex_t      imageIndex;
        clock_t     createdWhen;

        Block() :
        dict(std::map<std::string, json::Value>()),
        imageIndex(0),
        createdWhen( clock() ){ }

        //Note that the 'keys' argument can be empty, the required keys
        //are auto-generated from the other arguments
        Block(std::string data,
              std::map<std::string, json::Value> keys,
              std::string type,
              std::string from,
              iindex_t ii,
              clock_t ca
              ) :
            data(data),
            dict(keys),
            type(type),
            whereFrom(from),
            imageIndex(ii),
            createdWhen(ca)
        { }

        Block(std::string data, std::string type) :
            data(data),
            type(type),
            imageIndex(0),
            createdWhen(0)
            {}

#ifndef NBL_STANDALONE
        template <class M>
        bool parseAsProtobuf( M& protobuf ) {
            std::string mtype = protobuf.GetTypeName();
            if (mtype.find(type) != std::string::npos) {
                return protobuf.ParseFromString(data);
            } else {
                NBL_WARN("block of type '%s' does not contain protobuf of type '%s'", type.c_str(),
                         mtype.c_str());

                return false;
            }
        }
#endif

        //Assumes 422 encoding
        //NOTE: the constructed YUVImage depends on the memory in 'data',
        //therefore the return value MUST NOT outlive that std::string and this Block.
#ifndef NBL_STANDALONE
        messages::YUVImage parseAsYUVImage() const;
        messages::YUVImage copyAsYUVImage() const;
        messages::YUVImage copyAsYUVImage(std::string& localBuffer) const;
#endif

        nbl::SExpr parseAsSexpr() const;

        json::Value parseAsJson() const;

        logptr parseAsLog() const;

        //Note: the returned object is a copy and modifications do not change the Block.
        json::Object getFullDictionary() const ;

    private:
        void parseTypeCheck(const std::string& reqType) const;
    };

#define NBL_HOST_TYPE_SET   \
    NBL_HOST_TYPE_SET_M(V5ROBOT)    \
    NBL_HOST_TYPE_SET_M(V4ROBOT)    \
    NBL_HOST_TYPE_SET_M(NBCROSS)    \
    NBL_HOST_TYPE_SET_M(UNKNOWN)    \


#define NBL_HOST_TYPE_SET_M(name) name,
    enum host_type_e {
        NBL_HOST_TYPE_SET
    };
#undef NBL_HOST_TYPE_SET_M

    /* Values set elsewhere, used as attributes for every Log */

    void configTopLevelLogAttrs(host_type_e ht, std::string hn);

    extern host_type_e HOST_TYPE;
    extern std::string HOST_NAME;

    class Log {
        
    public:
        std::vector<Block> blocks;
        json::Object topLevelDictionary;

        time_t createdWhen;
        std::string logClass;

        /* ret indicates success */

#ifndef NBL_STANDALONE
        bool addBlockFromProtobuf(const google::protobuf::Message& message,
                                  const std::string& whereFrom, iindex_t imageIndex, clock_t createdWhen);

        bool addBlockFromImage(const messages::YUVImage& image, const std::string& whereFrom, iindex_t imageIndex, clock_t createdWhen);
#endif

        bool addBlockFromSexpr(const nbl::SExpr& sexpr, const std::string& whereFrom, iindex_t imageIndex, clock_t createdWhen);

        bool addBlockFromJson(const json::Value& val, const std::string& whereFrom, iindex_t imageIndex, clock_t createdWhen);

        bool addBlockFromLog(const Log& log);

        /* pointer is to Block in blocks, do not memory manage.  NULL on failure. */
        Block* find(const std::string& type);

        /* All construction is done via factory methods */

        static logptr emptyLog() {
            return logptr(new Log());
        }

        static logptr explicitLog(const std::vector<Block>& initBlocks,
                                  const json::Object initDict, std::string logClass, time_t cwhen = -1) {
            return logptr(new Log(initBlocks, initDict, logClass,
                                         cwhen == -1 ? time(NULL) : cwhen));
        }

        static logptr parseFrom(const std::string& buffer);
        static logptr parseFromParts(const std::string&json, const std::string& data);

        logptr deepCopy();

        /* IO OPERATIONS */
        static logptr recv(io::client_socket_t sock);
        static logptr read(int fd);

        io::ioret send(io::client_socket_t sock) const ;
        io::ioret write(int fd) const ;

        //Serialize entire log to buffer.
        void serialize(std::string& buffer) const;

    private:
        void init(const std::string& json, const std::string& data);

        Log() : createdWhen(time(NULL)), logClass(SharedConstants::LogClass_Null()) { }
        Log(const std::vector<Block>& initBlocks,
            const json::Object initDict, std::string clss, time_t cwhen) :
            logClass(clss),
            createdWhen(cwhen),
            blocks(initBlocks), topLevelDictionary(initDict) {
                if (logClass == "") logClass = SharedConstants::LogClass_Null();
            }

        //Parsing logs from serialized parts
        Log(const std::string& buffer); //all-in-one, with length encoded
        Log(const std::string& json, const std::string& data); //separate
    };
    
}   //namespace

#endif /* Log_hpp */
