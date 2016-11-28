//
//  MenuViewController.swift
//  nbControl
//
//  Created by Cory Alini on 11/20/16.
//  Copyright © 2016 gis.coryalini. All rights reserved.
//

import UIKit

var robot: String?

class MenuViewController: UITableViewController {
    
//    struct RobotName {
//        static let shehulk = 23
//        static let batman = 22
//        static let buzz = 21
//        static let wasp = 20
//        static let elektra = 19
//        static let blt = 18
//        static let zoe = 17
//        static let mal = 16
//        static let vera = 15
//        static let kaylee = 14
//        static let inara = 13
//        static let simon = 12
//        static let jayne = 11
//        static let river = 10
//        static let wash = 9
//    }

    let sections = ["Online", "Offline"]
    
    let baseIPaddress = "139.140.192."
    
    var listOfV5Robots = ["shehulk": "23","batman":"22","buzz":"21","wasp":"20","elektra":"19","blt":"18"]
    
    var listOfV4Robots = ["zoe":17, "mal":16, "vera":15, "kaylee":14, "inara":13, "simon":12, "jayne":11, "river":10, "wash":9]
    
    var online: String?
    var offline: String?
    
    override func viewDidLoad() {
        super.viewDidLoad()
        // Uncomment the following line to preserve selection between presentations
        // self.clearsSelectionOnViewWillAppear = false
        
        // Uncomment the following line to display an Edit button in the navigation bar for this view controller.
        // self.navigationItem.rightBarButtonItem = self.editButtonItem()
    }
    
    override func didReceiveMemoryWarning() {
        super.didReceiveMemoryWarning()
        // Dispose of any resources that can be recreated.
    }
    
    // MARK: - Table view data source
    
    override func numberOfSections(in tableView: UITableView) -> Int {
        return 2
    }
    
    override func tableView(_ tableView: UITableView, numberOfRowsInSection section: Int) -> Int {
        if(section==0) {
            return online.count;
        }
        return offline.count;
    }
    
    
    override func tableView(_ tableView: UITableView, cellForRowAt indexPath: IndexPath) -> UITableViewCell {
        let cell = tableView.dequeueReusableCell(withIdentifier: "LabelCell", for: indexPath)

        if(indexPath.section == 0) {
            cell.textLabel?.text = "\(online[indexPath.row]):"
        } else {
            cell.textLabel?.text = "\(offline[indexPath.row]):"
        }
        return cell
    }
    
    override func tableView(_ tableView: UITableView, willDisplayHeaderView view: UIView, forSection section: Int) {
        //tableView.backgroundColor = UIColor.black
        
        guard view is UITableViewHeaderFooterView else { return }
        }
    
    
    override func tableView(_ tableView: UITableView, titleForHeaderInSection section: Int) -> String? {
        return sections[section]
    }

    override func tableView(_ tableView: UITableView, didSelectRowAt indexPath: IndexPath) {
        // check index path
    }
    
    
    private func connectToRobots() {
        for (robotName, id) in listOfV5Robots {
            let ipaddress = baseIPaddress + id
            let currConnection = RobotManagerProtocol.currentAddress(ipaddress as! RobotManagerProtocol)
            if (currConnection != nil) {
                online?.append(robotName)
            } else {
                offline?.append(robotName)
            }
        }
        
        
        
    }
    
}

