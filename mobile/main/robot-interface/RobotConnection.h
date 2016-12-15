//
//  RobotConnection.h
//  nbControl
//
//  Created by Cory Alini on 11/2/16.
//  Copyright © 2016 gis.coryalini. All rights reserved.
//


/* This file was based on a file called RobotConnection.java from nbtool*/

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

+(BOOL) canConnectTo: (NSString *) host;

@end
