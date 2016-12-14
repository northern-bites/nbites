//
//  RobotConnection.m
//  SocketTest
//


#import "RobotConnection.h"

#include <string>

//Excessive c library and system includes.  Yay!
#include <netinet/in.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <err.h>
#include <errno.h>
#include <pthread.h>
#include <stdio.h>
#include <netdb.h>
#include <sys/time.h>
#include <math.h>

/* All this code basically uses the existing c++ code to do the work inside objective c code */

/* stuff like nbl::SharedConstants:: is pulling constants from the NBites lib so that we don't have to hardcode them. */

#include "nblogio.h"
#include "Log.hpp"

@interface Log ()
/*nbl::logptr is what NBites uses to talk about logs.  It manages its own memory*/
-(id) initWithNBL: (nbl::logptr) lp;
@end

@implementation Log {
@public
    nbl::logptr internal;
}

/*yay factory methods*/
+(id) blankLog {
    return [[Log alloc] init];
}

//http://rypress.com/tutorials/objective-c/classes
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

//Need to convert the c-style string from the log to objective c NSString instance.
//assume .c_str() of std::string returns a utf8 encoded string
-(NSString*) logClass {
    NSString * ret = [NSString stringWithUTF8String:internal->logClass.c_str()];
    return ret;
}

//java code uses ascii string encoding, maybe this should be utf8?
-(void) setClass: (NSString *) theClass {
    const char * cstr = [theClass cStringUsingEncoding:NSASCIIStringEncoding];
    internal->logClass = std::string(cstr);
}

//setup 'internal' to represent a 'command log' of function 'theCommand'
-(void) setCommand: (NSString *) theCommand {
    //the key is used by the tool to track who should get output of a command.
    //we don't really care, all the output is going to the same place.
    //so we can just hardcode the key.
    std::string key = "NULLKEY";

    std::string fname = std::string([theCommand cStringUsingEncoding:NSASCIIStringEncoding]);

    internal->logClass = nbl::SharedConstants::LogClass_RPC_Call();
    internal->topLevelDictionary[nbl::SharedConstants::RPC_NAME()] = json::String(fname);

//    printf("name set as: %s\n",
//           internal->topLevelDictionary[nbl::SharedConstants::RPC_NAME()].asString().c_str());

    internal->topLevelDictionary[nbl::SharedConstants::RPC_KEY()] = json::String(key);
}

/* Ok.  This code is from the calibration functions.  basically, we create an empty log of null class.  Then, we give its JSON dictionary a bunch of key/value pairs.  Then we attach the log to the existing 'internal' log.  The c++ code on the robot will parse it for us. */
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
    /* These are the only internal fields of every RobotConnection instance. */
    NSString * _address;
    nbl::io::client_socket_t socket;
    nbl::logptr ping;
}

/*factory method to make a connection to robot at 'address'*/
+(id) connectionTo:(NSString*) address {
    return [[RobotConnection alloc] initTo:address];
}

-(id) initTo:(NSString*) address {
    self = [super init];
    if (self) {
        _address = address;

        /*ping should be an empty log.  this is from the nbcross code (nbcross.cpp) */
        ping = nbl::Log::explicitLog({}, json::Object(),
                                     nbl::SharedConstants::LogClass_Null(), 0);

        const char * addr = [address cStringUsingEncoding: NSASCIIStringEncoding];
        if (!addr) {
            NSLog(@"ERROR: could not encode host string!");
        }

        /*try to connect (on current thread.  So this blocks.)*/
        if ( nbl::io::connect_to(socket, nbl::SharedConstants::ROBOT_PORT(), addr) ) {
            NSLog(@"could not connect to '%@'!",
                  _address);
            socket = -1;
        } else {

            /*make socket nonblocking, i.e. input/output won't block the calling thread.  it needs to be nonblocking for the nbl::io:: functions to work right*/
            nbl::io::sock_opt_mask mask = static_cast<nbl::io::sock_opt_mask>(nbl::io::NONBLOCKING);

            nbl::io::config_socket(socket,
                                   mask);
        }
    }

    return self;
}

/*All of the logic for one read and one write from the robot connection this instance wraps*/
/* checks and sends heartbeats if one or the other (or both) sides have nothing interesting to say but want to know the other is still there / alive / the connection is OK */
-(Log *) poll: (Log *) command {

    if (![self active]) {
        NSLog(@"ERROR! connection to '%@' is closed! (cannot poll)", _address);
        return NULL;
    }

    /*try to get/read a log from the socket*/
    nbl::logptr recvd = nbl::Log::recv(socket);

    if (!recvd) {
        /*couldn't read a log for whatever reason*/
        NSLog(@"connection to '%@' has died on recv!", _address);
        [self close];
        return NULL;
    }
    /*convert c++ log to string app can use*/
    NSLog(@"connection to '%@' read '%s' !", _address, recvd->logClass.c_str() );

    Log * ret = NULL;

    if (recvd->logClass != nbl::SharedConstants::LogClass_Null()) {
        /*We got something that *wasn't* just a heartbeat response.  yay!  Get ready to send it back*/
        ret = [[Log alloc] initWithNBL:recvd];
    }

    nbl::io::ioret send_status;
    if (command) {
        /*we have something to send to the robot*/
        printf("sending! (RobotConnection)\n");

        send_status = command->internal->send(socket);
    } else {
        /*we have nothing interesting to send to the robot, so send a heartbeat so that it knows we're still here*/
        ++(ping->createdWhen);
        send_status = ping->send(socket);
    }

    if (send_status) {
        /*non-zero send status means something went baaaadddd, and the connection is dead*/
        NSLog(@"connection to '%@' has died on send!", _address);
        [self close];
        return NULL;
    }

    /*might be NULL! that's ok*/
    return ret;
}

-(NSString *) getAddress {
    return _address;
}


-(BOOL) active {
    /*valid sockets are >= 0, but 0 should always already be taken by stdout, so we can just do '>'*/
    return (socket > 0);
}

-(void) close {
    if (socket > 0) close(socket);
    socket = -1;
}

+(int) timeoutInMicroseconds {
    return nbl::SharedConstants::REMOTE_HOST_TIMEOUT();
}

// create socket, set non blocking, connect, check select() return.
//FROM:

//http://stackoverflow.com/questions/2597608/c-socket-connection-timeout
//http://developerweb.net/viewtopic.php?id=3196

/* returns 1 if connection successful, 0 otherwise. THIS FUNCTION BLOCKS*/
int timedConnectTo(const char * addr, int port, int seconds) {

    struct sockaddr_in server;
    int sock;

    //Initialize to zero
    bzero(&server, sizeof(server));

    //Setup socket with basic internet socket attributes, check for system errors.
    sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (sock < 0) { NBL_ERROR("could not create socket"); return 0;}

    server.sin_family = AF_INET ;
    server.sin_port = htons(port);

    //Convert string 'addr' (which may be ip (172.152.213.5) or host name (mal) to host entity structure.
    //This may involve a DNS lookup, but let's hope that's quick... (it may not be!)
    struct hostent * hent = gethostbyname(addr);
    if (hent == NULL) { NBL_ERROR("could not get host ip %s", addr); return 0;}

    //copy the data we got from lookup
    bcopy(hent->h_addr, &server.sin_addr.s_addr, hent->h_length);

    fd_set fdset;
    struct timeval tv;

    //make this socket non-blocking so connect returns immediately (while the actual connection continues to occur, or not occur)
    fcntl(sock, F_SETFL, O_NONBLOCK);

    //start connection.
    connect(sock, (struct sockaddr *)&server, sizeof(server));

    //init arguments to select
    FD_ZERO(&fdset);
    FD_SET(sock, &fdset);
    tv.tv_sec = seconds;             /* set timeout in seconds */
    tv.tv_usec = 0;

    //call select, ensure it returns success status (1 socket == 1 ret)
    /* THIS CALL MAY BLOCK UP TO 'seconds' */
    if (select(sock + 1, NULL, &fdset, NULL, &tv) == 1)
    {
        int so_error;
        socklen_t len = sizeof so_error;

        //check socket is alive
        getsockopt(sock, SOL_SOCKET, SO_ERROR, &so_error, &len);

        if (so_error == 0) {
            printf("\t%s:%d is open\n", addr, port);
            close(sock);
            return 1;   //'connection!'
        }
    }

    //something went wrong, close and return 'no connection'
    close(sock);
    return 0;
}

/*check connection to the robot named 'host' on current thread*/
+(BOOL) canConnectTo: (NSString *) host {

    const char * addr = [host cStringUsingEncoding: NSASCIIStringEncoding];
    if (!addr) {
        NSLog(@"ERROR: could not encode host string!");
    }

    /* test connection with no timeout limit */
//    
//    BOOL ret = false;
//    nbl::io::client_socket_t clisock;
//    if (nbl::io::connect_to(clisock, nbl::SharedConstants::ROBOT_PORT(), addr) == nbl::io::SUCCESS) {
//        ret = true;
//        shutdown(clisock, SHUT_RDWR);   // try to shutdown the socket in case something is being written.
//        close(clisock);
//    }
//
//    return ret;

    //use a timed connection attempt.
    return timedConnectTo(addr, nbl::SharedConstants::ROBOT_PORT(), 1) > 0;
}

@end

