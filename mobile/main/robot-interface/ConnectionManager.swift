//
//  ConnectionManager.swift
//  nbControl
//
//  Created by Cory Alini on 11/2/16.
//  Copyright © 2016 gis.coryalini. All rights reserved.
//


/* This file was based on a file called RobotConnection.java from nbtool*/


import Foundation
import UIKit

let robotManager:RobotManagerProtocol = RobotManager()

func blankRobotCommand() -> RobotCommandStruct {
    return RobotCommandStruct(adjustHead: false, adjustedHeadZ: 0, adjustedHeadY: 0, walkCommand: false, walkStop: false, walkHeading: 0, walkX: 0, walkY: 0, doSweetMove: false, sweetMoveID: 0, logInfo: false, logImage: false)
}

protocol RobotManagerProtocol {
    func currentAddress() -> String?

    func connectTo(_ address:String )

    func disconnect()

    func send(_ aCommand: RobotCommandStruct)

    //set dispatchIncomingLog log callback
    //escaping makes the arguments closures
    func setIncomingHandler(callback: @escaping ((Log, String)->Void) )
    func setStatusHandler(callback: @escaping ((Bool, String)->Void) )

    /* does pinging on async thread, callback is called on main thread with args (online,offline) */
    func determineOnlineHosts(hosts: [String], callback: @escaping ([String],[String])->(Void))
}

class RobotManager: RobotManagerProtocol {

    let connectionQueue = DispatchQueue(label: "RobotConnectionQueue")

    var connection:RobotConnection? = nil
    var command:RobotCommandStruct? = nil

    var incomingLogHandler:((Log, String)->Void)? = nil
    var statusHandler:((Bool, String)->Void)? = nil

    private func runLoop(_ address: String) {
        connection = RobotConnection(to: address)

        if (connection!.active()) {
            dispatchStatusUpdate(true, address)
        }

        while(connection!.active()) {

            var toSend:Log? = nil

            if let cmnd = command {
                print("sending! (RobotManager)")

                toSend = Log()
                toSend?.setCommand("nbControlFunction")
                toSend?.addCommandArguments(cmnd)
                command = nil
            }

            let _got:Log? = connection?.poll(toSend)

            if let got = _got {
                dispatchIncomingLog(got, address)
            }

            //timeoutInMicros == 5000000 or 5 seconds.
            usleep(useconds_t( RobotConnection.timeoutInMicroseconds() / 5 ))
        }

        /*tell our managers we disconnected =( */
        dispatchStatusUpdate(false, address)
    }

    open func currentAddress() -> String? {
        return connection?.getAddress()
    }

    open func connectTo(_ address: String) {
        if (connection == nil) {
            connectionQueue.async {
                self.runLoop(address)
            }

        } else {
            print("****** already connected to a robot '\(currentAddress())', cannot connectTo() without disconnect()!")
        }
    }

    open func send(_ aCommand: RobotCommandStruct) {
        command = aCommand
    }

    open func disconnect() {
        if let robot = connection {
            if (robot.active()) {
                robot.close()
            }
        }
    }

    open func setIncomingHandler(callback: @escaping ((Log, String)->Void) ) {
        incomingLogHandler = callback
    }

    open func setStatusHandler(callback: @escaping ((Bool, String)->Void) ) {
        statusHandler = callback
    }

    private func dispatchStatusUpdate(_ up:Bool, _ addr:String) {
        print("dispatchStatusUpdate(\(up) , \(addr))")
        DispatchQueue.main.async {
            if let handler = self.statusHandler {
                handler(up, addr)
            }
        }
    }

    private func dispatchIncomingLog(_ log:Log, _ addr:String) {
         print("dispatchIncomingLog(\(addr))")
        DispatchQueue.main.async {
            if let handler = self.incomingLogHandler {
                handler(log, addr)
            }
        }
    }

    //http://commandshift.co.uk/blog/2014/03/19/using-dispatch-groups-to-wait-for-multiple-web-services/
    open func determineOnlineHosts(hosts: [String], callback: @escaping ([String],[String])->(Void)) {
        //This function returns immediately and the caller is notified of completeion with the callback.
        asyncDOH(hosts: hosts, callback: callback)

        /*!! Old code, this performs a serial check of all hosts rather than parallel */
//        DispatchQueue.global().async {
//
//            var online = [String]()
//            var offline = [String]()
//
//            for host in hosts {
//                print("testing \(host)")
//
//                if (RobotConnection.canConnect(to: host)) {
//                    online.append(host)
//                } else {
//                    offline.append(host)
//                }
//            }
//
//
//            DispatchQueue.main.async {
//                callback(online, offline)
//            }
//        }
    }

    private func asyncDOH(hosts: [String], callback: @escaping ([String],[String])->(Void)) {
        var online = [String]()
        var offline = [String]()
        let group = DispatchGroup()

        for host in hosts {
            //associate with the group.  we do this *synchronously* to avoid the race condition where the block executes before the code after the schedueling of the block can run.
            group.enter()

            DispatchQueue.global().async() {
                print("testing: \(host)")
                let status = RobotConnection.canConnect(to: host)

                //make sure we're on the main thread so that the modification of our lists is only done on one thread
                DispatchQueue.main.async() {
                    if (status) {
                        online.append(host)
                    } else {
                        offline.append(host)
                    }

                    group.leave()
                }
            }
        }
        group.notify(queue: DispatchQueue.main) {
            print("all hosts have been tested!")
            callback(online, offline)
        }
    }
}
