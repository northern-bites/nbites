//
//  HelpTableViewController.swift
//  nbControl
//
//  Created by Cory Alini on 12/12/16.
//  Copyright © 2016 gis.coryalini. All rights reserved.
//

import UIKit

class HelpTableViewController: UITableViewController {

    let ivory = UIColor(colorLiteralRed: 255.0/255.0, green: 255.0/255.0, blue: 242.0/255.0, alpha: 1.0)
    let cambridge_blue = UIColor(colorLiteralRed: 163.0/255.0, green: 187.0/255.0, blue: 173.0/255.0, alpha: 1.0)
    let myrtle_green = UIColor(colorLiteralRed: 53.0/255.0, green: 114.0/255.0, blue: 102.0/255.0, alpha: 1.0)
    let warm_black = UIColor(colorLiteralRed: 14.0/255.0, green: 59.0/255.0, blue: 67.0/255.0, alpha: 1.0)
    let jet = UIColor(colorLiteralRed: 46.0/255.0, green: 53.0/255.0, blue: 50.0/255.0, alpha: 1.0)
    let honeydew = UIColor(colorLiteralRed: 237.0/255.0, green: 255.0/255.0, blue: 236.0/255.0, alpha: 1.0)
    let light_slate_gray = UIColor(colorLiteralRed: 200.0/255.0, green: 199.0/255.0, blue: 200.0/255.0, alpha: 1.0)
    
        var sections = ["\tThe robot is not moving!","\tThe robot disconnected!", "\tThe robot is falling over!", "\tThere are no robots available."]
        var not_moving_rows = ["This could be because the robot has disconnected. To check if the robot has disconnected, please view the manlog and read the yellow printouts"]
        var disconnected_rows = ["if you cannot connect to a robot that appears to be running, check the status of the logging system on that robot with nbtool or by ssh’ing into the robot"]
        var unstable_rows = ["Please design a new kick that works and Cory will add it to the app."]
        var no_robots_rows = ["First, refresh the robot menu. If your robot still has not appeared please make sure that you are able to ping the robot on terminal. The robot might still be turning on."]
        override func viewDidLoad() {
            super.viewDidLoad()
            tableView.separatorStyle = .none
        }
        
        // MARK: - Table view data source
        
        override func numberOfSections(in tableView: UITableView) -> Int {
            
            return sections.count
        }
        
        override func tableView(_ tableView: UITableView, numberOfRowsInSection section: Int) -> Int {
            return 1;
        }
        
        
        override func tableView(_ tableView: UITableView, cellForRowAt indexPath: IndexPath) -> UITableViewCell {
            tableView.estimatedRowHeight = 50
            tableView.rowHeight = UITableViewAutomaticDimension
            let cell = tableView.dequeueReusableCell(withIdentifier: "LabelCell", for: indexPath)
            
            cell.backgroundColor = jet
            
            
            cell.textLabel?.textColor = cambridge_blue
            cell.textLabel?.font = UIFont.systemFont(ofSize: 20)
            
            cell.detailTextLabel?.textColor = cambridge_blue
            cell.detailTextLabel?.font = UIFont.systemFont(ofSize: 15)
            cell.selectionStyle = UITableViewCellSelectionStyle.none
            cell.textLabel?.numberOfLines = 0
            if(indexPath.section == 0) {
                cell.textLabel?.text = "\(not_moving_rows[indexPath.row]):"
            } else if (indexPath.section == 1)  {
                cell.textLabel?.text = "\(disconnected_rows[indexPath.row]):"

            } else if (indexPath.section == 2){
                cell.textLabel?.text = "\(unstable_rows[indexPath.row]):"
            } else {
                cell.textLabel?.text = "\(no_robots_rows[indexPath.row]):"
                
            }
            return cell
        }
        
        override func tableView(_ tableView: UITableView, titleForHeaderInSection section: Int) -> String? {
            
            return sections[section]
        }
        override func tableView(_ tableView: UITableView, viewForHeaderInSection section: Int) -> UILabel? {
            
            tableView.backgroundColor = jet
            let label : UILabel = UILabel()
            label.backgroundColor = light_slate_gray
            label.text = sections[section]
            label.textColor = jet
            label.font = UIFont.boldSystemFont(ofSize: 20)
            label.textAlignment = .left
        
            return label
        }
        func tableView(tableView: UITableView, heightForHeaderInSection section: Int) -> CGFloat {
            return 100
            
        }

        
}
