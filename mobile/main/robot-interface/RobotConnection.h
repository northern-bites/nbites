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

struct RobotCommandStruct {
    BOOL adjustHead;
    float adjustedHeadZ,adjustedHeadY;

    BOOL walkCommand, walkStop;
    float walkHeading,walkX,walkY;

    BOOL doSweetMove;
    int sweetMoveID;

    BOOL logInfo,logImage;
};

@interface Log : NSObject

+(id) blankLog;
-(id) init;

-(NSString*) logClass;
-(void) setClass: (NSString *) theClass;

-(void) setCommand: (NSString *) theCommand;


-(void) addCommandArguments: (struct RobotCommandStruct) args;

@end

@interface RobotConnection : NSObject

+(id) connectionTo:(NSString*) address;

-(id) initTo:(NSString*) address;

-(NSString *) getAddress;

-(Log *) poll: (Log *) command;

-(BOOL) active;

-(void) close;

+(int) timeoutInMicroseconds;

@end
