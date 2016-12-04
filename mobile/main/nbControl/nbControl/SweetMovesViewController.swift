//
//  SweetMovesViewController.swift
//  nbControl
//
//  Created by Cory Alini on 12/4/16.
//  Copyright © 2016 gis.coryalini. All rights reserved.
//

//Source http://stackoverflow.com/questions/31735228/how-to-make-a-simple-collection-view-with-swift

import UIKit
class SweetMovesViewController: UIViewController, UICollectionViewDataSource, UICollectionViewDelegate,UICollectionViewDelegateFlowLayout{
    
    let reuseIdentifier = "cell" // also enter this string as the cell identifier in the storyboard
    var items = ["Goalie Squat", "Short Straight Kick", "Straight Kick","Side Kick", "Big Kick", "Cute Kick"]
    
    
    // MARK: - UICollectionViewDataSource protocol
    
    // tell the collection view how many cells to make
    func collectionView(_ collectionView: UICollectionView, numberOfItemsInSection section: Int) -> Int {
        return self.items.count
    }
    
    // make a cell for each cell index path
    func collectionView(_ collectionView: UICollectionView, cellForItemAt indexPath: IndexPath) -> UICollectionViewCell {
        
        // get a reference to our storyboard cell
        let cell = collectionView.dequeueReusableCell(withReuseIdentifier: reuseIdentifier, for: indexPath as IndexPath) as! SweetMovesCollectionViewCell
        
        // Use the outlet in our custom class to get a reference to the UILabel in the cell
        cell.SweetMovesLabel.text = self.items[indexPath.item]
        cell.backgroundColor = UIColor.cyan // make cell more visible in our example project
        
        return cell
    }
    
    // MARK: - UICollectionViewDelegate protocol
    
    func collectionView(_ collectionView: UICollectionView, didSelectItemAt indexPath: IndexPath) {
        // handle tap events
        print("You selected cell #\(indexPath.item)!")
    }

}
