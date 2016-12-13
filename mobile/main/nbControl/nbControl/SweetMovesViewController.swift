//
//  SweetMovesViewController.swift
//  nbControl
//
//  Created by Cory Alini on 12/4/16.
//  Copyright © 2016 gis.coryalini. All rights reserved.
//

//Source http://stackoverflow.com/questions/31735228/how-to-make-a-simple-collection-view-with-swift

import UIKit
class SweetMovesViewController: UIViewController, UICollectionViewDataSource, UICollectionViewDelegate,UICollectionViewDelegateFlowLayout, UIPopoverPresentationControllerDelegate{
    
    let cambridge_blue = UIColor(colorLiteralRed: 163.0/255.0, green: 187.0/255.0, blue: 173.0/255.0, alpha: 1.0)
    let jet = UIColor(colorLiteralRed: 46.0/255.0, green: 53.0/255.0, blue: 50.0/255.0, alpha: 1.0)
    let light_slate_gray = UIColor(colorLiteralRed: 200.0/255.0, green: 199.0/255.0, blue: 200.0/255.0, alpha: 1.0)
    
    let reuseIdentifier = "cell" // also enter this string as the cell identifier in the storyboard
    var items = ["Goalie Squat", "Short Straight Kick", "Straight Kick","Side Kick", "Big Kick", "Cute Kick"]
    
    
    // MARK: - UICollectionViewDataSource protocol
    
    // tell the collection view how many cells to make
    func collectionView(_ collectionView: UICollectionView,
        numberOfItemsInSection section: Int) -> Int {
        return self.items.count
    }
    
    // make a cell for each cell index path
    func collectionView(_ collectionView: UICollectionView, cellForItemAt indexPath: IndexPath) -> UICollectionViewCell {
        // get a reference to our storyboard cell
        let cell = collectionView.dequeueReusableCell(withReuseIdentifier: reuseIdentifier, for: indexPath as IndexPath) as! SweetMovesCollectionViewCell
        
        // Use the outlet in our custom class to get a reference to the UILabel in the cell
        cell.SweetMovesLabel.text = self.items[indexPath.item]
        cell.SweetMovesLabel.textColor = light_slate_gray
        cell.backgroundColor = jet
        var name = String(items[indexPath.item])!
        
        let imageview:UIImageView=UIImageView(frame:CGRect(x: 0.0, y: 0.0, width: self.view.frame.width-250, height: self.view.frame.width-275));
        
        name = name + ".JPG"
        
        let image:UIImage = UIImage(named:name)!
        imageview.image = image
        cell.contentView.addSubview(imageview)
        
        return cell
    }
    
    // MARK: - UICollectionViewDelegate protocol
    
    func collectionView(_ collectionView: UICollectionView, didSelectItemAt indexPath: IndexPath) {
        // handle tap events
        //var items = ["Goalie Squat", "Short Straight Kick", "Straight Kick","Side Kick", "Big Kick", "Cute Kick"]
        
        var robot = blankRobotCommand()
        robot.doSweetMove = true
        robot.sweetMoveID = Int32(indexPath.item)
        if (robotManager.currentAddress() != nil) {
            robotManager.send(robot)
        }
    }

    override func prepare(for segue: UIStoryboardSegue, sender: Any?) {
        
        
        var destinationvc = segue.destination
        
        if let navCon = destinationvc as? UINavigationController {
            destinationvc = navCon.visibleViewController ?? destinationvc
        }
        
        if segue.identifier == "performPopover" {
            if let vc = destinationvc as? SweetMovesPopoverTableViewController {
                vc.modalPresentationStyle = UIModalPresentationStyle.popover
                vc.popoverPresentationController!.delegate = self
                vc.navigationItem.title = "Info"
            }
        }
    }
    func adaptivePresentationStyle(for controller: UIPresentationController) -> UIModalPresentationStyle {
        return UIModalPresentationStyle.none
    }

}
