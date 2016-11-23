//
//  InfoView.swift
//  nbControl
//
//  Created by Cory Alini on 11/7/16.
//  Copyright © 2016 gis.coryalini. All rights reserved.
//

import Foundation

class InfoBrain {
    
    var batLevel: Double {
        get {
            return batteryLevel
        }
    }
    
    var playingState: String {
        get {
            return state
        }
    }
    
    var batNum: Double {
        get {
            return batteryNumber
        }
    }
    
    var currPlayer: String {
        get {
            return player
        }
    }
    var gameState: String {
        get {
            return gamestate
        }
    }
    
    
    var batteryLevel = 10.0;
    var state = "Chase Ball"
    var batteryNumber = 17.0
    var player = "pBrunswick"
    var gamestate = "Game playing"
    
    func setVariables() {
        
        
    }

    
    
    
}
