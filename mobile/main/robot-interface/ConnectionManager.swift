//
//  Robot.swift
//  SocketTest
//


import Foundation
import UIKit

//Creates an instance of RobotManager
let robotManager:RobotManagerProtocol = RobotManager()

//Initializes the Robot command struct
func blankRobotCommand() -> RobotCommandStruct {
    return RobotCommandStruct(adjustHead: false, adjustedHeadZ: 0, adjustedHeadY: 0, walkCommand: false, walkStop: false, walkHeading: 0, walkX: 0, walkY: 0, doSweetMove: false, sweetMoveID: 0, logInfo: false, logImage: false)
}

protocol RobotManagerProtocol {
    //address of current robot if connected, nil if not connected
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

    /* this function, which is not specified in the protocol, is a helper.
     
        it encapsulates the code for managing a connection to a robot.
        it should be run asynchronously, as it will block for the duration of the connection
     
        every hal
     */
    private func runLoop(_ address: String) {
        connection = RobotConnection(to: address)

        if (connection!.active()) {
            /*tell our managers (once) we connected! =) */
            dispatchStatusUpdate(true, address)
        }

        while(connection!.active()) {

            var toSend:Log? = nil

            if let cmnd = command {
                /* we have a command to send!
                    1) make a log
                    2) make it a 'command log' that calls the command 'nbControlFunction'
                        on the robot
                    3) add the actual command arguments we were given to the log
                 */

                print("sending! (RobotManager)")

                toSend = Log()
                toSend?.setCommand("nbControlFunction")
                toSend?.addCommandArguments(cmnd)
                command = nil
            }

            //try to read one log from the robot and send one log.
            let _got:Log? = connection?.poll(toSend)

            if let got = _got {
                //if we got something interesting, pass it to our handler
                dispatchIncomingLog(got, address)
            }

            //timeoutInMicros == 5000000 or 5 seconds.  adjust as necessary
            usleep(useconds_t( RobotConnection.timeoutInMicroseconds() / 5 ))
        }

        /*tell our managers we disconnected =( */
        dispatchStatusUpdate(false, address)
    }

    /* returns optional because we might not actually have an active connection */
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

    /* this doesn't actually send anything.  but it does schedule the command so that it is sent the next time the connection loop loops.  NOTE: if this method is called again before that happens, the original command is overwritten and lost! */
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

    /* on the main thread, tell our 'statusHandler' – whoever the App code has decided that should be that the status of our connection to robot 'addr' has changed.
    
        the app code might not *care* which robot it is, or they might care.  They are free to ignore either or both arguments in their response.
     */
    private func dispatchStatusUpdate(_ up:Bool, _ addr:String) {
        print("dispatchStatusUpdate(\(up) , \(addr))")
        DispatchQueue.main.async {
            if let handler = self.statusHandler {
                handler(up, addr)
            }
        }
    }

    /* on the main thread, tell our 'incomingLogHandler' – whoever the App code has decided that should be – that we got a log from a robot named 'addr' */
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

        /*
         Basic structure: create a bunch of asynchronous 'tasks.'  In each task, first enter the dispatch group synchronously (race condition otherwise, see website), then asynchronously check connection.
         
            When we get a result, go to MAIN thread and
                first update lists
                then leave dispatch group.
         
         Wait for all tasks in dispatch group to finish.
         */

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
        //this object associates all our asynchronous 'test connection' blocks
        let group = DispatchGroup()

        for host in hosts {
            //associate with the group.  we do this *synchronously* to avoid the race condition where the block executes before the code after the schedueling of the block can run.
            group.enter()

            DispatchQueue.global().async() {
                print("testing: \(host)")
                let status = RobotConnection.canConnect(to: host)

                //make sure we're on the main thread so that the modification of our lists is only done on one thread (concurrent modification is a bad no no)
                DispatchQueue.main.async() {
                    if (status) {
                        online.append(host)
                    } else {
                        offline.append(host)
                    }

                    //leave the group.  this doesn't need to be done on the main thread but it DOES need to be done after the append.  So it goes here.
                    group.leave()
                }
            }
        }

        //tell the group to call the following block when the group is done!
        group.notify(queue: DispatchQueue.main) {
            print("all hosts have been tested!")
            //and then this block calls the callback from our argument
            callback(online, offline)
        }
    }
}
