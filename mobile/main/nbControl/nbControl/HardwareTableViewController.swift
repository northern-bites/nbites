//
//  CoreDataTableViewController.swift
//
//  Created by CS193p Instructor.
//  Copyright © 2015-16 Stanford University. All rights reserved.
//

import UIKit
import CoreData

class HardwareTableViewController: UITableViewController, NSFetchedResultsControllerDelegate
{
    var fetchedResultsController: NSFetchedResultsController<NSFetchRequestResult>? {
        didSet {
            do {
                if let frc = fetchedResultsController {
                    frc.delegate = self
                    try frc.performFetch()
                }
                tableView.reloadData()
            } catch let error {
                print("NSFetchedResultsController.performFetch() failed: \(error)")
            }
        }
    }

    // MARK: UITableViewDataSource

//    override func numberOfSectionsInTableView(tableView: UITableView) -> Int {
//        return fetchedResultsController?.sections?.count ?? 1
//    }

    override func tableView(_ tableView: UITableView, numberOfRowsInSection section: Int) -> Int {
        if let sections = fetchedResultsController?.sections, sections.count > 0 {
            return sections[section].numberOfObjects
        } else {
            return 0
        }
    }
    
    override func tableView(_ tableView: UITableView, titleForHeaderInSection section: Int) -> String? {
        if let sections = fetchedResultsController?.sections, sections.count > 0 {
            return sections[section].name
        } else {
            return nil
        }
    }
//    
//    override func sectionIndexTitlesForTableView(tableView: UITableView) -> [String]? {
//        return fetchedResultsController?.sectionIndexTitles
// }
    
    override func tableView(_ tableView: UITableView, sectionForSectionIndexTitle title: String, at index: Int) -> Int {
        return fetchedResultsController?.section(forSectionIndexTitle: title, at: index) ?? 0
    }
    
    // MARK: NSFetchedResultsControllerDelegate
    
    private func controllerWillChangeContent(controller: NSFetchedResultsController<NSFetchRequestResult>) {
        tableView.beginUpdates()
    }
    
    func controller(controller: NSFetchedResultsController<NSFetchRequestResult>, didChangeSection sectionInfo: NSFetchedResultsSectionInfo, atIndex sectionIndex: Int, forChangeType type: NSFetchedResultsChangeType) {
        switch type {
            case .insert: tableView.insertSections(NSIndexSet(index: sectionIndex) as IndexSet, with: .fade)
            case .delete: tableView.deleteSections(NSIndexSet(index: sectionIndex)as IndexSet, with: .fade)
            default: break
        }
    }
    
    func controller(controller: NSFetchedResultsController<NSFetchRequestResult>, didChangeObject anObject: Any, atIndexPath indexPath: IndexPath?, forChangeType type: NSFetchedResultsChangeType, newIndexPath: IndexPath?) {
        switch type {
            case .insert:
                tableView.insertRows(at: [newIndexPath! as IndexPath], with: .fade)
            case .delete:
                tableView.deleteRows(at: [indexPath! as IndexPath], with: .fade)
            case .update:
                tableView.reloadRows(at: [indexPath! as IndexPath], with: .fade)
            case .move:
                tableView.deleteRows(at: [indexPath! as IndexPath], with: .fade)
                tableView.insertRows(at: [newIndexPath! as IndexPath], with:
                    .fade)
        }
    }
    
    private func controllerDidChangeContent(controller: NSFetchedResultsController<NSFetchRequestResult>) {
        tableView.endUpdates()
    }
}

