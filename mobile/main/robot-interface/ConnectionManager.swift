//
//  Robot.swift
//  SocketTest
//


import Foundation
import UIKit

let robotManager:RobotManagerProtocol = RobotManager()

protocol RobotManagerProtocol {
    //address of current robot if connected, nil if not connected
    func currentAddress() -> String?

    //connect to robot at address 'address', if disconnect will first disconnect from any currently connected robot
    //returns success of connection
    func connectTo(_ address:String, disconnect:Bool) -> Bool

    func disconnect()

    //send command to robot (add more arguments!)
    func send(_ aCommand: RobotCommandStruct)

    //set dispatchIncomingLog log callback
    //escaping makes the arguments closures
    func setIncomingHandler(callback: @escaping ((Log, String)->Void) )
    func setStatusHandler(callback: @escaping ((Bool, String)->Void) )

    func determineOnlineHosts(hosts: [String], callback: @escaping (Void)->([String]))
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

            usleep(useconds_t( RobotConnection.timeoutInMicroseconds() / 2 ))
        }

        dispatchStatusUpdate(false, address)
    }

    open func currentAddress() -> String? {
        return connection?.getAddress()
    }

    open func connectTo(_ address: String, disconnect: Bool) -> Bool {
        if (connection != nil) {
            if (disconnect) {
                connection?.close()

                connectionQueue.async {
                    self.runLoop(address)
                }
                return true
            } else {
                return false
            }
        } else {
            connectionQueue.async {
                self.runLoop(address)
            }
            return true
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

    open func determineOnlineHosts(hosts: [String], callback: @escaping (Void)->([String])) {
        DispatchQueue.global().async {

            var online = [String]()
            var offline = [String]()

            for host in hosts {

            }

        }
    }
}
