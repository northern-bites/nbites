//
//  Log.cpp
//
//  Created by Philip Koch on 3/19/16.
//

#include <stdexcept>
#include "Log.hpp"
#include "nblogio.h"
#include "utilities-test.hpp"

namespace nbl {

    SharedConstants * SharedConstants::theInstance = nullptr;
    SharedConstants * SharedConstants::get() {
        if (theInstance) return theInstance;
        else return (theInstance = new SharedConstants());
    }

    json::Object getConstantsObjectFromFile() {
        std::string filePath;

        static const std::string onlinePath(
                                     "/home/nao/nbites/Config/LOGGING_CONSTANTS.json");

        static const std::string offlineSuffix(
                                        "/src/share/logshare/LOGGING_CONSTANTS.json");

        char * envNBITES_DIR = getenv("NBITES_DIR");
        bool offline = false;

        /* some attempt is made to use the correct filepath */
        /* but if OFFLINE is defined and not NBITES_DIR, that's a guaranteed error */

#ifdef OFFLINE
        offline = true;
#endif

        if (offline && (envNBITES_DIR == NULL)) {
            throw std::runtime_error("environment var NBITES_DIR required for OFFLINE testing!");
        }

        if (envNBITES_DIR == NULL) {
            filePath = onlinePath;
        } else {
            filePath = std::string(envNBITES_DIR) + offlineSuffix;
        }

        NBL_WARN("nbl::SharedConstants constructor reading from: %s\n",
               filePath.c_str());

        std::string contents;
        io::readFileToString(contents, filePath);

        return json::parse(contents).asObject();
    }

    /* re-def allows us to copy paste from SharedConstants in header */
    /* this test ensures the constants exist, are findable, and are formatted properly */

#define NBL_CREATE_CONSTANT_METHOD(name)    \
    SharedConstants::name();

#define NBL_CREATE_INT_CONSTANT_METHOD(name)    \
    SharedConstants::name();

    NBL_ADD_TEST_TO(SharedConstantsTest, Log) {

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

        return true;
    }

    SharedConstants::SharedConstants() :
        dict( getConstantsObjectFromFile() )
    { }

    void Block::parseTypeCheck(const std::string& reqType) const {
        if (type != reqType) {
            throw std::domain_error(utilities::format("cannot parse block of type %s as %s", type.c_str(), reqType.c_str())
                                    );
        }
    }
#ifndef NBL_STANDALONE
    messages::YUVImage Block::parseAsYUVImage() const {
        parseTypeCheck(SharedConstants::YUVImageType_DEFAULT());

        int width = 2 * dict.at(SharedConstants::LOG_BLOCK_IMAGE_WIDTH_PIXELS()).asConstNumber().asInt();
        int height = dict.at(SharedConstants::LOG_BLOCK_IMAGE_HEIGHT_PIXELS()).asConstNumber().asInt();

        return messages::YUVImage( (uint8_t *) &data[0], width, height, width );
    }

    messages::YUVImage Block::copyAsYUVImage() const {
        parseTypeCheck(SharedConstants::YUVImageType_DEFAULT());

        int width = 2 * dict.at(SharedConstants::LOG_BLOCK_IMAGE_WIDTH_PIXELS()).asConstNumber().asInt();
        int height = dict.at(SharedConstants::LOG_BLOCK_IMAGE_HEIGHT_PIXELS()).asConstNumber().asInt();
        uint8_t * buffer = new uint8_t[data.size()];
        memcpy(buffer, &data[0], data.size());
        return messages::YUVImage( buffer, width, height, width );
    }

    messages::YUVImage Block::copyAsYUVImage(std::string& localBuffer) const {
        parseTypeCheck(SharedConstants::YUVImageType_DEFAULT());

        int width = 2 * dict.at(SharedConstants::LOG_BLOCK_IMAGE_WIDTH_PIXELS()).asConstNumber().asInt();
        int height = dict.at(SharedConstants::LOG_BLOCK_IMAGE_HEIGHT_PIXELS()).asConstNumber().asInt();

        localBuffer = data;
        return messages::YUVImage( (uint8_t *) &localBuffer[0], width, height, width );
    }

#endif

    nbl::SExpr Block::parseAsSexpr() const {
        parseTypeCheck(SharedConstants::SexprType_DEFAULT());

        nbl::SExpr * sex = nbl::SExpr::read(data);
        if (sex) {
            nbl::SExpr ret = *sex;
            delete sex;
            return ret;
        } else {
            throw std::domain_error("cannot parse Block as valid Sexpr!");
        }
    }

    json::Value Block::parseAsJson() const {
        parseTypeCheck(SharedConstants::JsonType_DEFAULT());
        return json::parse(data);
    }

    logptr Block::parseAsLog() const {
        parseTypeCheck(SharedConstants::LogType_DEFAULT());
        return Log::parseFrom(data);
    }

    json::Object Block::getFullDictionary() const {
        json::Object ret(dict);

        ret[SharedConstants::LOG_BLOCK_TYPE()] = json::String(type);
        ret[SharedConstants::LOG_BLOCK_WHERE_FROM()] = json::String(whereFrom);
        ret[SharedConstants::LOG_BLOCK_IMAGE_INDEX()] = json::Number( (long) imageIndex);
        ret[SharedConstants::LOG_BLOCK_WHEN_MADE()] = json::Number((long) createdWhen);

        ret[SharedConstants::LOG_BLOCK_NUM_BYTES()] = json::Number((long) data.size());

        return ret;
    }

    host_type_e HOST_TYPE = UNKNOWN;
    std::string HOST_NAME = "";

#define NBL_HOST_TYPE_SET_M(name) NBL_STRINGIFY(name) ,
    const char * HOST_TYPE_S[] = {
        NBL_HOST_TYPE_SET
    };
#undef NBL_HOST_TYPE_SET_M

    void configTopLevelLogAttrs(host_type_e ht, std::string hn) {
        NBL_PRINT("HOST_TYPE=%s HOST_NAME=%s",
                  HOST_TYPE_S[ht], hn.c_str() );
        HOST_TYPE = ht;
        HOST_NAME = hn;
    }

#define ADD_BLOCK_FAIL_IF(cond, msg, ...)    \
    if (cond) { NBL_WARN(msg, ## __VA_ARGS__ ); return false; }

#ifndef NBL_STANDALONE
    bool Log::addBlockFromProtobuf(const google::protobuf::Message &message, const std::string &whereFrom, iindex_t imageIndex, clock_t createdWhen) {
        std::string data;
        std::string type;

        bool ser_success = message.SerializeToString(&data);
        ADD_BLOCK_FAIL_IF(!ser_success, "failed to serialize protobuf")

        size_t substrFrom = message.GetTypeName().find_last_of('.');
        type = message.GetTypeName().substr(substrFrom + 1);

        blocks.push_back(Block(
                               data,
                               std::map<std::string, json::Value>(),
                               type,
                               whereFrom,
                               imageIndex,
                               createdWhen
                         ));

        return true;
    }

    bool Log::addBlockFromImage(const messages::YUVImage &image, const std::string &whereFrom, iindex_t imageIndex, clock_t createdWhen) {
        std::map<std::string, json::Value> keys;

        keys[SharedConstants::LOG_BLOCK_IMAGE_WIDTH_PIXELS()] = json::Number( image.width() / 2);
        keys[SharedConstants::LOG_BLOCK_IMAGE_HEIGHT_PIXELS()] = json::Number(image.height());

        long im_size = (image.width() * image.height() * 1);

        blocks.push_back(Block(
                               std::string((char *) image.pixelAddress(0, 0), im_size),
                               keys,
                               SharedConstants::YUVImageType_DEFAULT(),
                               whereFrom,
                               imageIndex,
                               createdWhen
                               )
                         );

        return true;
    }
#endif

    bool Log::addBlockFromSexpr(const nbl::SExpr &sexpr, const std::string &whereFrom, iindex_t imageIndex, clock_t createdWhen) {
        blocks.push_back(Block(sexpr.serialize(), std::map<std::string, json::Value>(), SharedConstants::SexprType_DEFAULT(), whereFrom, imageIndex, createdWhen));

        return true;
    }

    bool Log::addBlockFromJson(const json::Value &val, const std::string &whereFrom, iindex_t imageIndex, clock_t createdWhen) {
        blocks.push_back(Block(val.serialize(), std::map<std::string, json::Value>(), SharedConstants::JsonType_DEFAULT(), whereFrom, imageIndex, createdWhen));

        return true;
    }

    bool Log::addBlockFromLog(const nbl::Log& log) {
        std::string data;
        log.serialize(data);

        blocks.push_back(Block(data, std::map<std::string, json::Value>(), SharedConstants::LogType_DEFAULT(), "n/a", -1, -1));

        return true;
    }

    Block* Log::find(const std::string& type) {
        for (Block& b : blocks) {
            if (b.type == type)
                return &b;
        }

        return NULL;
    }

    json::Value getValueAndRemoveKey(const std::string& key, json::Object& obj) {
        json::Value ret = obj.at(key);
        obj.erase(key);
        return ret;
    }

    void putNetworkInt(int val, void * loc) {
        int * iptr = (int *) loc;
        *iptr = htonl(val);
    }

    void Log::serialize(std::string &buffer) const {
        json::Object topLevel(topLevelDictionary);

        topLevel[SharedConstants::LOG_TOPLEVEL_MAGIC_KEY()] = json::Number(LOG_VERSION);
        topLevel[SharedConstants::LOG_TOPLEVEL_LOGCLASS()] = json::String(logClass);
        topLevel[SharedConstants::LOG_TOPLEVEL_CREATED_WHEN()] = json::Number(createdWhen);
        topLevel[SharedConstants::LOG_TOPLEVEL_BLOCKS()] = json::Array();
        json::Array& blockObj = topLevel[SharedConstants::LOG_TOPLEVEL_BLOCKS()].asArray();

        topLevel[SharedConstants::LOG_TOPLEVEL_HOST_TYPE()] = json::String(HOST_TYPE_S[HOST_TYPE]);
        topLevel[SharedConstants::LOG_TOPLEVEL_HOST_NAME()] = json::String(HOST_NAME);

        size_t totalDataSize = 0;
        for (int i = 0; i < blocks.size(); ++i) {
            totalDataSize += blocks[i].data.size();
            blockObj.push_back(blocks[i].getFullDictionary());
        }

        std::string serializedDict = topLevel.serialize();

        buffer.resize(totalDataSize + serializedDict.size() + 8, 0);
        char * internal = &(buffer[0]);

        putNetworkInt( (int) serializedDict.size(), internal);
        internal += 4;
        memcpy(internal, &serializedDict[0], serializedDict.size() );
        internal += serializedDict.size();
        putNetworkInt( (int) totalDataSize, internal);
        internal += 4;
        for (int i = 0; i < blocks.size(); ++i) {
            const Block& bi = blocks[i];
            memcpy(internal, &(bi.data[0]), bi.data.size());
            internal += bi.data.size();
        }

        size_t bytes_written = internal - &(buffer[0]);
        NBL_ASSERT(bytes_written == buffer.size());
    }

    int getNetworkInt(const void * loc) {
        const int * iptr = (const int *) loc;
        return ntohl(*iptr);
    }

    void Log::init(const std::string &jsonStr, const std::string &data) {
        int end;
        json::Object topObj = json::parseFrom(jsonStr, 0, &end).asObject();
        NBL_ASSERT_EQ(end, jsonStr.size());

        logClass = getValueAndRemoveKey(SharedConstants::LOG_TOPLEVEL_LOGCLASS(), topObj).asString();
        createdWhen = getValueAndRemoveKey(SharedConstants::LOG_TOPLEVEL_CREATED_WHEN(), topObj).asNumber().asLong();

        json::Array blockArray = getValueAndRemoveKey(SharedConstants::LOG_TOPLEVEL_BLOCKS(), topObj).asArray();

        topLevelDictionary = topObj;

        const char * internal = &data[0];
        for (int i = 0; i < blockArray.size(); ++i) {
            json::Object& blockObj = blockArray[i].asObject();

            size_t dataSize = getValueAndRemoveKey(SharedConstants::LOG_BLOCK_NUM_BYTES(), blockObj).asNumber().asLong();

            std::string type = getValueAndRemoveKey(SharedConstants::LOG_BLOCK_TYPE(), blockObj).asString();
            std::string from = getValueAndRemoveKey(SharedConstants::LOG_BLOCK_WHERE_FROM(), blockObj).asString();
            size_t ii = getValueAndRemoveKey(SharedConstants::LOG_BLOCK_IMAGE_INDEX(), blockObj).asNumber().asLong();
            clock_t ca = getValueAndRemoveKey(SharedConstants::LOG_BLOCK_WHEN_MADE(), blockObj).asNumber().asLong();

            blocks.push_back(Block(std::string(internal, dataSize), blockObj, type, from, ii, ca));

            internal += dataSize;
        }

        size_t nbytes = internal - &data[0];
        NBL_ASSERT_EQ(nbytes, data.size());
    }

    Log::Log(const std::string& buffer) {
        const char * internal = &buffer[0];
        int jsonLength = getNetworkInt(internal);
        internal += 4;

        std::string jsonStr(internal, jsonLength);
        internal += jsonLength;

        int dataLength = getNetworkInt(internal);
        internal += 4;

        std::string dataStr(internal, dataLength);

        NBL_ASSERT_EQ(jsonLength + dataLength + 8, buffer.size() );

        init(jsonStr, dataStr);
    }

    logptr Log::parseFrom(const std::string &buffer) {
        return logptr(new Log(buffer));
    }

    Log::Log(const std::string& json, const std::string& data) {
        init(json, data);
    }

    logptr Log::parseFromParts(const std::string &json, const std::string &data) {
        return logptr(new Log(json, data));
    }

    logptr Log::deepCopy() {
        logptr ret = Log::explicitLog(blocks, topLevelDictionary, logClass);
        ret->createdWhen = createdWhen;
        return ret;
    }

    io::ioret Log::send(io::client_socket_t sock) const {
        std::string buffer;
        serialize(buffer);
        return io::send_exact(sock, buffer.size(), &buffer[0], io::IO_MAX_DELAY());
    }

    io::ioret Log::write(int fd) const {
        std::string buffer;
        serialize(buffer);
        return io::write_exact(fd, buffer.size(), &buffer[0]);
    }

    logptr Log::recv(io::client_socket_t sock) {

        int32_t sizeBuffer;
        if( io::recv_exact(sock, 4, &sizeBuffer, io::IO_MAX_DELAY()) != io::SUCCESS) {
            return nullptr;
        }

        int jsonSize = ntohl(sizeBuffer);
        NBL_ASSERT_GT(jsonSize, SharedConstants::LOG_TOPLEVEL_MAGIC_KEY().size());

        char jsonBuffer[jsonSize];
        if( io::recv_exact(sock, jsonSize, jsonBuffer, io::IO_MAX_DELAY()) != io::SUCCESS) {
            return nullptr;
        }

        if( io::recv_exact(sock, 4, &sizeBuffer, io::IO_MAX_DELAY()) != io::SUCCESS) {
            return nullptr;
        }

        int dataSize = ntohl(sizeBuffer);
        char dataBuffer[dataSize];
        if( io::recv_exact(sock, dataSize, dataBuffer, io::IO_MAX_DELAY()) != io::SUCCESS) {
            return nullptr;
        }

        return parseFromParts( std::string(jsonBuffer, jsonSize), std::string(dataBuffer, dataSize) );
    }

    logptr Log::read(int fd) {
        std::string buffer;
        io::readFileToString(buffer, fd);

        return parseFrom(buffer);
    }
}