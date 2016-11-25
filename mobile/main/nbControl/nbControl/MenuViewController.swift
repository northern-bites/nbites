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
    
    var sections = ["Online", "Offline"]
    var online = ["Batman", "Shehulk", "Elektra"]
    var offline = ["BLT","Wasp", "Buzz"]
    
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
    
    override func prepare(for segue: UIStoryboardSegue, sender: Any?) {
//        var destinationvc = segue.destination
//        
//        if let navCon = destinationvc as? UINavigationController {
//            destinationvc = navCon.visibleViewController ?? destinationvc
//        }
//        if let graphvc = destinationvc as? GraphViewController {
//            graphvc.program = brain.program
//            graphvc.navigationItem.title = brain.turnStringListIntoString
//            
//        }
    }
    
}

