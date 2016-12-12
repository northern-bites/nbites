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
    
    let sections = ["Online", "Offline"]
    
    let baseIPaddress = "139.140.192."
    
    var listOfV5Robots = ["shehulk": "23","batman":"22","buzz":"21","wasp":"20","elektra":"19","blt":"18",
                          "localhost":"0"] //for testing
    
    var listOfV4Robots = ["zoe":"17", "mal":"16", "vera":"15", "kaylee":"14", "inara":"13", "simon":"12", "jayne":"11", "river":"10", "wash":"9"]
    
    var online: [String] = []
    var offline: [String] = []
    
    override func viewDidLoad() {
        super.viewDidLoad()
        
        findAndReloadRobots(nil)

        let refresher = UIRefreshControl()

        refresher.addTarget(self, action: #selector(refreshRobots(_:)), for: .valueChanged)

        if #available(iOS 10.0, *) {
            tableView.refreshControl = refresher
        } else {
            tableView.backgroundView = refresher
        }
    }
    
    // MARK: - Table view data source
    
    override func numberOfSections(in tableView: UITableView) -> Int {
        return sections.count
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
        guard view is UITableViewHeaderFooterView else { return }
        
    }

    override func tableView(_ tableView: UITableView, titleForHeaderInSection section: Int) -> String? {
        return sections[section]
    }

    override func tableView(_ tableView: UITableView, willSelectRowAt indexPath: IndexPath) -> IndexPath? {
        if (indexPath.section == 0) {
            return indexPath
        } else {
            return nil
        }
    }

    override func tableView(_ tableView: UITableView, didSelectRowAt indexPath: IndexPath) {
        // check index path

        assert(indexPath.section == 0)

        if (indexPath.row < online.count) {
            let host = online[indexPath.row]
            print("MenuViewController: connecting to \(host)")
            robotManager.connectTo(host)
        } else {
            print("ERROR indexPath.row \(indexPath.row) >= online.count \(online.count)")
        }
    }
    
    private func handleRefresh(refreshControl: UIRefreshControl) {
        //findConnectedRobots()
        self.tableView.reloadData() 
        refreshControl.endRefreshing()
    }

    @objc private func refreshRobots(_ refreshControl: UIRefreshControl) {
        print("refreshRobots()!")
        findAndReloadRobots(refreshControl)
    }

    private func findAndReloadRobots(_ refreshControl: UIRefreshControl?) {
        print("findAndReloadRobots()!")

        var allRobots = [String]()
        /* Comment for testing */
        allRobots.append(contentsOf: listOfV4Robots.keys)
        allRobots.append(contentsOf: listOfV5Robots.keys)

        /* Uncomment for testing */
//        allRobots.append("mal")


        /* Below this line should be uncommented for both testing and release */
        self.online = allRobots

        robotManager.determineOnlineHosts(hosts: allRobots) {
            (onlineHosts:[String], offlineHosts:[String])->Void in

            print("findAndReloadRobots callback() online:\(onlineHosts) offline:\(offlineHosts.count)")

            self.online = onlineHosts
            self.offline = offlineHosts

            self.tableView.reloadData()

            if let refresher = refreshControl {
                refresher.endRefreshing()
            }
        }
    }
    
    fileprivate struct StoryBoard {
        static let ShowGraphSegue = "help"
    }
    
    override func prepare(for segue: UIStoryboardSegue, sender: Any?) {
        var destinationvc = segue.destination
        
        if let navCon = destinationvc as? UINavigationController {
            destinationvc = navCon.visibleViewController ?? destinationvc
        }
        if segue.identifier == "help" {
            if let vc = destinationvc as? HelpTableViewController {
                vc.navigationItem.title = "Help"
            }
        }
    }
}

