//
//  RobotConnection.h
//  SocketTest
//

/*
 The methods of this class are meant to be called in a dedicated thread – not any GUI thread.
 
 The thread should call poll() every (REMOTE_HOST_TIMEOUT / 5) microseconds – aka every second.
 
 poll()'s argument should be a command log if one is queued, or NULL.  If NULL is passed poll will
 send a heartbeat in place of a command log.
 
 poll() will return a log or NULL depending on what is read from the socket.  If NULL is returned, the socket's status should be queried with active().
 */

#import <Foundation/Foundation.h>

/* this structure represents all the fields we use to talk to the robot.  These fields are encoded in the log in addCommandArguments().  ***The camel-case is NOT reflected in the protobufs / python code *** (darn protobufs!) */
struct RobotCommandStruct {
    BOOL adjustHead;
    float adjustedHeadZ,adjustedHeadY;

    BOOL walkCommand, walkStop;
    float walkHeading,walkX,walkY;

    BOOL doSweetMove;
    int sweetMoveID;

    BOOL logInfo,logImage;
};

/*Log class so that swift can do some manipulation of the log we send*/
@interface Log : NSObject

+(id) blankLog;
-(id) init;

-(NSString*) logClass;
-(void) setClass: (NSString *) theClass;

-(void) setCommand: (NSString *) theCommand;

-(void) addCommandArguments: (struct RobotCommandStruct) args;

@end


/*This class encapsulates one connection to a robot (and a couple static functions)...*/
@interface RobotConnection : NSObject

/*Make a connection to address, calls initTo:address*/
+(id) connectionTo:(NSString*) address;

-(id) initTo:(NSString*) address;

/* get currently used address */
-(NSString *) getAddress;

/*do one read of a log and write of a log (command) on the current thread.*/
-(Log *) poll: (Log *) command;

/*is this connection alive?*/
-(BOOL) active;

/*close this connection (if its still alive)*/
-(void) close;

/*static function helper for swift*/
+(int) timeoutInMicroseconds;

/*static function helper for swift*/
+(BOOL) canConnectTo: (NSString *) host;

@end
