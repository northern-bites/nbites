//
//  RobotConnection.m
//  SocketTest
//


#import "RobotConnection.h"

#include <string>
#include "nblogio.h"
#include "Log.hpp"

@interface Log ()
-(id) initWithNBL: (nbl::logptr) lp;
@end

@implementation Log {
@public
    nbl::logptr internal;
}

+(id) blankLog {
    return [[Log alloc] init];
}

-(id) init {
    self = [super init];
    if (self) {
        internal = nbl::Log::emptyLog();
    }

    return self;
}

-(id) initWithNBL: (nbl::logptr) lp {
    if ([self init]) {
        internal = lp;
    }

    return self;
}

-(NSString*) logClass {
    NSString * ret = [NSString stringWithUTF8String:internal->logClass.c_str()];
    return ret;
}

-(void) setClass: (NSString *) theClass {
    const char * cstr = [theClass cStringUsingEncoding:NSASCIIStringEncoding];
    internal->logClass = std::string(cstr);
}

-(void) setCommand: (NSString *) theCommand {
    std::string key = "NULLKEY";
    std::string fname = std::string([theCommand cStringUsingEncoding:NSASCIIStringEncoding]);

    internal->logClass = nbl::SharedConstants::LogClass_RPC_Call();
    internal->topLevelDictionary[nbl::SharedConstants::RPC_NAME()] = json::String(fname);

//    printf("name set as: %s\n",
//           internal->topLevelDictionary[nbl::SharedConstants::RPC_NAME()].asString().c_str());

    internal->topLevelDictionary[nbl::SharedConstants::RPC_KEY()] = json::String(key);
}

-(void) addCommandArguments: (struct RobotCommandStruct) args {
    nbl::logptr argument = nbl::Log::explicitLog({}, json::Object{}, nbl::SharedConstants::LogClass_Null() );

    argument->topLevelDictionary["adjustHead"] = json::Boolean(args.adjustHead);
    argument->topLevelDictionary["adjustedHeadZ"] = json::Number(args.adjustedHeadZ);
    argument->topLevelDictionary["adjustedHeadY"] = json::Number(args.adjustedHeadY);

    argument->topLevelDictionary["walkCommand"] = json::Boolean(args.walkCommand);
    argument->topLevelDictionary["walkStop"] = json::Boolean(args.walkStop);
    argument->topLevelDictionary["walkHeading"] = json::Number(args.walkHeading);
    argument->topLevelDictionary["walkX"] = json::Number(args.walkX);
    argument->topLevelDictionary["walkY"] = json::Number(args.walkY);

    argument->topLevelDictionary["doSweetMove"] = json::Boolean(args.doSweetMove);
    argument->topLevelDictionary["sweetMoveID"] = json::Number(args.sweetMoveID);

    argument->topLevelDictionary["logInfo"] = json::Boolean(args.logInfo);
    argument->topLevelDictionary["logImage"] = json::Boolean(args.adjustHead);

    internal->addBlockFromLog(*argument);
}

@end

@implementation RobotConnection {
    NSString * _address;
    nbl::io::client_socket_t socket;
    nbl::logptr ping;
}

+(id) connectionTo:(NSString*) address {
    return [[RobotConnection alloc] initTo:address];
}

-(id) initTo:(NSString*) address {
    self = [super init];
    if (self) {
        _address = address;
        
        ping = nbl::Log::explicitLog({}, json::Object(),
                                     nbl::SharedConstants::LogClass_Null(), 0);

        const char * addr = [address cStringUsingEncoding: NSASCIIStringEncoding];
        if (!addr) {
            NSLog(@"ERROR: could not encode host string!");
        }

        if ( nbl::io::connect_to(socket, nbl::SharedConstants::ROBOT_PORT(), addr) ) {
            NSLog(@"could not connect to '%@'!",
                  _address);
            socket = -1;
        } else {

            nbl::io::sock_opt_mask mask = static_cast<nbl::io::sock_opt_mask>(nbl::io::NONBLOCKING);

            nbl::io::config_socket(socket,
                                   mask);
        }
    }

    return self;
}

-(Log *) poll: (Log *) command {

    if (![self active]) {
        NSLog(@"ERROR! connection to '%@' is closed! (cannot poll)", _address);
        return NULL;
    }

    nbl::logptr recvd = nbl::Log::recv(socket);

    if (!recvd) {
        NSLog(@"connection to '%@' has died on recv!", _address);
        [self close];
        return NULL;
    }

    NSLog(@"connection to '%@' read '%s' !", _address, recvd->logClass.c_str() );

    Log * ret = NULL;

    if (recvd->logClass != nbl::SharedConstants::LogClass_Null()) {
        ret = [[Log alloc] initWithNBL:recvd];
    }

    nbl::io::ioret send_status;
    if (command) {
        printf("sending! (RobotConnection)\n");

        send_status = command->internal->send(socket);
    } else {
        ++(ping->createdWhen);
        send_status = ping->send(socket);
    }

    if (send_status) {
        NSLog(@"connection to '%@' has died on send!", _address);
        [self close];
        return NULL;
    }

    return ret;
}

-(NSString *) getAddress {
    return _address;
}


-(BOOL) active {
    return (socket > 0);
}

-(void) close {
    if (socket > 0) close(socket);
    socket = -1;
}


+(int) timeoutInMicroseconds {
    return nbl::SharedConstants::REMOTE_HOST_TIMEOUT();
}

+(BOOL) canConnectTo: (NSString *) host {

    const char * addr = [host cStringUsingEncoding: NSASCIIStringEncoding];
    if (!addr) {
        NSLog(@"ERROR: could not encode host string!");
    }

    BOOL ret = false;
    nbl::io::client_socket_t clisock;
    if (nbl::io::connect_to(clisock, nbl::SharedConstants::ROBOT_PORT(), addr) == nbl::io::SUCCESS) {
        ret = true;
        shutdown(clisock, SHUT_RDWR);   // try to shutdown the socket in case something is being written.
        close(clisock);
    }

    return ret;
}

@end

