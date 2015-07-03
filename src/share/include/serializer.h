
#ifndef __boss_synch__serializer__
#define __boss_synch__serializer__

#include <stdio.h>
#include <vector>
#include <string>
#include <assert.h>
#include "google/protobuf/message.h"

class SerializableBase {
public:
    virtual int32_t length() = 0;
    virtual const void * data() = 0;
};

class ProtoSer: public SerializableBase {
    std::string _serialized;
public:

    ProtoSer(google::protobuf::Message* m) {
        m->SerializeToString(&_serialized);
    }

    int32_t length() {
        return _serialized.size();
    }

    const void * data() {
        return _serialized.data();
    }
};

class StringSer: public SerializableBase {
    char * _data;
    int32_t _length;

public:
    StringSer(char * chars, int len) {
        _data = chars;
        _length = len;
    }

    int32_t length() {
        return _length;
    }

    const void * data() {
        return _data;
    }
};

//true on success, false on failure (null pointer, <= 0 length, oversized)
// **** DELETES POINTERS IN >objects< AFTER SERIALIZING ****
inline bool serializeTo(std::vector<SerializableBase *> objects, int64_t objectIndex, void * destStart, size_t maxSize, size_t * usedReturn) {

    if (maxSize > (size_t)INT32_MAX) {
        printf("ERROR: serializeTo() designed for 32 bit size values!  maxSize %zu exceeds INT32_MAX %u!\n",
               maxSize, INT32_MAX);
        return false;
    }

    size_t loc = 0;
    uint8_t * bytePointer = (uint8_t *) destStart;

    int64_t * indexPointer = (int64_t *) bytePointer;
    *indexPointer = objectIndex;
    loc += sizeof(int64_t);

    int32_t * intPointer = (int32_t *) (bytePointer + loc);
    *intPointer = (int32_t) objects.size();
    ++intPointer;
    loc += 2 * sizeof(int32_t); // reserve space for numBytes.

    for (size_t i = 0; i < objects.size(); ++i) {
        int32_t size = objects[i]->length();
        const void * data = objects[i]->data();

        if (size < 0) {
            printf("Size < 0!\n");
            return false;
        }

        if (size > 0 && data == NULL) {
            printf("Data Null\n");
            return false;
        }

        if (loc + sizeof(int32_t) + size > maxSize) {
            printf("ERROR: serialize.h:%i not enough space for serialization! (was given %zu bytes)\n", __LINE__, maxSize);
            return false;
        }

        int32_t * sp = (int32_t *) (bytePointer + loc);
        void * dp = (void *) (bytePointer + loc + sizeof(int32_t));
        *sp = size;
        if (size > 0) {
            memcpy(dp, data, size);
        }

        loc += size + sizeof(int32_t);
        delete objects[i];
    }

    *intPointer = (int32_t) loc;
    if (usedReturn)
        *usedReturn = loc;           //Return bytes used by pointer
    return true;
}

class Deserialize {
    uint8_t * base;
    size_t loc;

    int askIndex;

    int64_t _dataIndex;
    std::vector<std::string> objs;

    int32_t _length() {
        uint32_t * ip = (uint32_t *) (base + loc);
        return *ip;
    }

    const void * _data() {
        return (void *) (base + loc + 4);
    }

    const std::string _string() {
        int32_t length = _length();
        if (length > 0) return std::string((char *) _data(), (size_t) length);
        else return std::string();
    }

public:

    Deserialize(void * start) :
    base(NULL),
    loc(-1),
    askIndex(-1),
    _dataIndex(-1)
    {
        base = (uint8_t *) start;
    }

    bool parse() {
        if (askIndex >= 0) {
            printf("ERROR: Deserialize object already parsed!\n");
            return false;
        }
        askIndex = 0;
        loc = 0;

        size_t nobjs, tbytes;

        _dataIndex = *((int64_t *) (base + loc));
        loc += sizeof(int64_t);

        nobjs = * ((uint32_t *) (base + loc));
        loc += sizeof(int32_t);

        tbytes = * ((uint32_t *) (base + loc));
        loc += sizeof(int32_t);

        assert(nobjs > 0);
        assert(tbytes > 0);

        for (size_t i = 0; i < nobjs; ++i) {
            size_t obj_end = loc + 4 + _length();
            if (loc >= tbytes || obj_end > tbytes) {
                printf("ERROR: deserialization did not complete with indicated frame length %i!\n", tbytes);
                return false;
            }

            objs.push_back(_string());
            loc = obj_end;
        }

        // If we try to parse memory which holds nothing (zero-set) we shouldn't error
        if (loc != tbytes && loc > 16) {
            printf("ERROR: deserialization did not use entire frame! %zu / %i bytes\n",
                   loc, tbytes);
            return false;
        }

        return true;
    }

    bool parsed() {
        return askIndex >= 0;
    }

    bool next() {
        if (!parsed()) {
            printf("ERROR: Deserializer not yet parsed!\n");
            return false;
        }

        if (askIndex == (int)objs.size()) {
            printf("ERROR: next() on dead Deserializer!\n");
            assert(false);
            return false;
        }

        ++askIndex;
        if (askIndex == (int)objs.size()) {
            return false;
        } else {
            return true;
        }
    }

    const std::string& string() {
        if (!parsed()) {
            printf("ERROR: Deserializer not yet parsed!\n");
            assert(false);
        }

        return objs[askIndex];
    }

    const std::string& stringNext() {
        if (!parsed()) {
            printf("ERROR: Deserializer not yet parsed!\n");
            assert(false);
        }

        int saved = askIndex;
        next();
        return objs[saved];
    }

    const std::string& object(int index) {
        return objs[index];
    }

    size_t nObjects() {
        return objs.size();
    }

    int64_t dataIndex() {
        if (parsed())
            return _dataIndex;
        else return -1;
    }

    ssize_t totalSize() {
        if (parsed())
            return loc;
        else return -1;
    }
};


#endif /* defined(__boss_synch__serializer__) */
