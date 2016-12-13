//
//  ControlPopoverTableViewController.swift
//  nbControl
//
//  Created by Cory Alini on 12/13/16.
//  Copyright © 2016 gis.coryalini. All rights reserved.
//

import UIKit

class ControlPopoverTableViewController: UITableViewController {
    
    
    let cambridge_blue = UIColor(colorLiteralRed: 163.0/255.0, green: 187.0/255.0, blue: 173.0/255.0, alpha: 1)
    let jet = UIColor(colorLiteralRed: 46.0/255.0, green: 53.0/255.0, blue: 50.0/255.0, alpha: 1)
    let light_slate_gray = UIColor(colorLiteralRed: 200.0/255.0, green: 199.0/255.0, blue: 200.0/255.0, alpha: 1)
    
    var sections = ["\t Basics","\t How to make the robot walk", "How to make the robot stop"]
    var basics_rows = ["To make the robot move first make sure that the robot is in either game initial or game playing. Then please put your flick the ball in the center of the screen towards the direction you wish the robot move. To make the robot stop walking, please place the ball in the center of the screen."]
    var robot_walk_rows = ["To make the robot walk in a particular direction, please flick the ball in the direction you wish the robot to move."]
    var robot_stop_rows = ["To make the robot stop please put the ball in the center or put the robot in either game ready or penalized state"]
    
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
        if (indexPath.section == 0) {
            cell.textLabel?.text = "\(basics_rows[indexPath.row])"
        }else if(indexPath.section == 1) {
            cell.textLabel?.text = "\(robot_walk_rows[indexPath.row])"
        } else {
            cell.textLabel?.text = "\(robot_stop_rows[indexPath.row])"
            
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
