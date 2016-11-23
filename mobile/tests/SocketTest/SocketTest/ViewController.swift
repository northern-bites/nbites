//
//  ViewController.swift
//  SocketTest
//


import UIKit

class ViewController: UIViewController {

    override func viewDidLoad() {
        super.viewDidLoad()
//        Robot.connect(host: "")

//        let conn = RobotConnection(to: "localhost")
//
//        let q1 = DispatchQueue(label: "q1")
//        let q2 = DispatchQueue(label: "q2")
//
//        q1.async {
//
//            print(":: connected ::")
//
//            while (conn!.active()) {
//                print("looooppp")
//                conn?.poll(nil)
//                usleep(useconds_t( RobotConnection.timeoutInMicroseconds() / 2 ))
//            }
//
//            print("conn died somehow!")
//        }

        robotManager.connectTo("localhost", disconnect: true)

        DispatchQueue.main.asyncAfter(deadline: DispatchTime.now() + 5) {

            robotManager.send(RobotCommandStruct())
        }

//        print("queueing second")
//
//        q2.asyncAfter(deadline: DispatchTime.now() + 5) {
//            print("closed that shit")
//            conn?.close()
//        }
    }

    override func didReceiveMemoryWarning() {
        super.didReceiveMemoryWarning()
        // Dispose of any resources that can be recreated.
    }


}

