//
//  ControlBrain.swift
//  nbControl
//
//  Created by Cory Alini on 11/26/16.
//  Copyright © 2016 gis.coryalini. All rights reserved.
//

import Foundation
import UIKit

//BOOL adjustHead;
//float adjustedHeadZ,adjustedHeadY;
//
//BOOL walkCommand, walkStop;
//float walkHeading,walkX,walkY;
//
//BOOL doSweetMove;
//int sweetMoveID;
//
//BOOL logInfo,logImage;



class ControlBrain {
    
    //BOOL walkCommand, walkStop;
    //float walkHeading,walkX,walkY;
    
    var circleCenter: CGPoint = CGPoint(x: 0.0, y: 0.0)
    var center: CGPoint {
        get {
            return circleCenter
        }
        set {
            circleCenter = newValue
        }
    }
    
    func sendNewWalk(currPosition: CGPoint, transition:CGPoint) {
        print("Curr position: [\(currPosition.x),\(currPosition.y)]")
        var robot = RobotCommandStruct.init()
        robot.walkCommand = true
        
        if (currPosition.x < 20.0 && currPosition.x > -20.0 && currPosition.y < 20.0 && currPosition.y > -20.0) {
                robot.walkStop = true
            print("robot not walking")
        } else {
            robot.walkY = 1
            
            let baseVector = CGPoint(x: 0.0, y: -100.0)
            //let baseMag = sqrt(baseVector)
            let dotProduct = Double(transition.x * baseVector.x + transition.y + baseVector.y)
            let transitionMag = Double(sqrt(transition.x * transition.x + transition.y + transition.y))
            let heading = acos(dotProduct/(transitionMag))
            
            robot.walkHeading = Float(heading)
            print("Heading: \(robot.walkHeading)) ")
        }
    }
    
    
}



