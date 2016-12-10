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
        tableView.separatorStyle = .none
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

    let ivory = UIColor(colorLiteralRed: 255.0/255.0, green: 255.0/255.0, blue: 242.0/255.0, alpha: 1.0)
    let cambridge_blue = UIColor(colorLiteralRed: 163.0/255.0, green: 187.0/255.0, blue: 173.0/255.0, alpha: 1.0)
    let myrtle_green = UIColor(colorLiteralRed: 53.0/255.0, green: 114.0/255.0, blue: 102.0/255.0, alpha: 1.0)
    let warm_black = UIColor(colorLiteralRed: 14.0/255.0, green: 59.0/255.0, blue: 67.0/255.0, alpha: 1.0)
    let jet = UIColor(colorLiteralRed: 46.0/255.0, green: 53.0/255.0, blue: 50.0/255.0, alpha: 1.0)
    let honeydew = UIColor(colorLiteralRed: 237.0/255.0, green: 255.0/255.0, blue: 236.0/255.0, alpha: 1.0)
    let light_slate_gray = UIColor(colorLiteralRed: 200.0/255.0, green: 199.0/255.0, blue: 200.0/255.0, alpha: 1.0)
    
    
    override func tableView(_ tableView: UITableView, cellForRowAt indexPath: IndexPath) -> UITableViewCell {
        let cell = tableView.dequeueReusableCell(withIdentifier: "LabelCell", for: indexPath)

        cell.backgroundColor = jet
        
        
        cell.textLabel?.textColor = cambridge_blue
        cell.textLabel?.font = UIFont.systemFont(ofSize: 20)
        
        cell.detailTextLabel?.textColor = cambridge_blue
        cell.detailTextLabel?.font = UIFont.systemFont(ofSize: 20)
        cell.selectionStyle = UITableViewCellSelectionStyle.none
        
        if(indexPath.section == 0) {
            cell.textLabel?.text = "\(battery_rows[indexPath.row]):"
            cell.detailTextLabel?.text = battery_detail[indexPath.row]
        } else {
            cell.textLabel?.text = "\(player_rows[indexPath.row]):"
            cell.detailTextLabel?.text = player_detail[indexPath.row]
        }
        return cell
    }
    
//    override func tableView(_ tableView: UITableView, willDisplayHeaderView view: UIView, forSection section: Int) {
//        tableView.backgroundColor = jet
//
//        guard let header = view as? UITableViewHeaderFooterView else { return }
//        //header.backgroundColor = warm_black
//        header.textLabel?.textColor = myrtle_green
//        header.textLabel?.font = UIFont.boldSystemFont(ofSize: 25)
//        header.textLabel?.frame = header.frame
//        header.textLabel?.textAlignment = .center
//    }
//    
    
    override func tableView(_ tableView: UITableView, titleForHeaderInSection section: Int) -> String? {
        
        return sections[section]
    }
    override func tableView(_ tableView: UITableView, viewForHeaderInSection section: Int) -> UILabel? {
        tableView.backgroundColor = jet
        let label : UILabel = UILabel()
        label.backgroundColor = light_slate_gray
        label.text = sections[section]
        label.textColor = jet
        label.font = UIFont.boldSystemFont(ofSize: 25)
        label.textAlignment = .center
        return label
    }
    func tableView(tableView: UITableView, heightForHeaderInSection section: Int) -> CGFloat {
        
        return 100
    
    }


}
