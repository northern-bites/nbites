//
//  InfoTableViewController.swift
//  nbControl
//
//  Created by Cory Alini on 11/14/16.
//  Copyright © 2016 gis.coryalini. All rights reserved.
//

import UIKit

class InfoTableViewController: UITableViewController {

    var battery_detail = ["100%","17"]
    var player_detail = ["pBrunswick","Game Playing", "Chase Ball"]
    
    var sections = ["Battery","Player"]
    var battery_rows = ["Battery Level", "Battery Number"]
    var player_rows = ["Player","Game State","Playing State"]
    
    
    
    
    
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
            return 2;
        }
        return 3;
    }

    

    
    override func tableView(_ tableView: UITableView, cellForRowAt indexPath: IndexPath) -> UITableViewCell {
        let cell = tableView.dequeueReusableCell(withIdentifier: "LabelCell", for: indexPath)

        cell.backgroundColor = UIColor(displayP3Red: 0.06, green: 0.31, blue: 0.32, alpha: 1.0)
        
        
        cell.textLabel?.textColor = UIColor(displayP3Red: 0.75, green: 0.75, blue: 0.80, alpha: 1.0)
        cell.textLabel?.font = UIFont.systemFont(ofSize: 20)
        
        cell.detailTextLabel?.textColor = UIColor(displayP3Red: 0.75, green: 0.75, blue: 0.80, alpha: 1.0)
        cell.detailTextLabel?.font = UIFont.systemFont(ofSize: 20)

        
        if(indexPath.section == 0) {
            cell.textLabel?.text = "\(battery_rows[indexPath.row]):"
            cell.detailTextLabel?.text = battery_detail[indexPath.row]
        } else {
            cell.textLabel?.text = "\(player_rows[indexPath.row]):"
            cell.detailTextLabel?.text = player_detail[indexPath.row]
        }
        return cell
    }
    
    override func tableView(_ tableView: UITableView, willDisplayHeaderView view: UIView, forSection section: Int) {
        tableView.backgroundColor = UIColor.black

        guard let header = view as? UITableViewHeaderFooterView else { return }
        //header.backgroundColor = UIColor.white
        header.textLabel?.textColor = UIColor.black
        header.textLabel?.font = UIFont.boldSystemFont(ofSize: 25)
        header.textLabel?.frame = header.frame
        header.textLabel?.textAlignment = .center
    }
    
    
    override func tableView(_ tableView: UITableView, titleForHeaderInSection section: Int) -> String? {
        return sections[section]
    }
    
}
