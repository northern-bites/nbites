//
//  DropItBehavior.swift
//  DropIt
//
//  Created by Eric Chown on 11/9/16.
//  Copyright © 2016 edu.bowdoin.cs2505.chown. All rights reserved.
//

import UIKit

class DropItBehavior: UIDynamicBehavior {
    
    let gravity = UIGravityBehavior()


    
    lazy var collider: UICollisionBehavior = {
        let lazyCollider = UICollisionBehavior()
        lazyCollider.translatesReferenceBoundsIntoBoundary = true
        return lazyCollider
    }()

    
    lazy var dropBehavior: UIDynamicItemBehavior = {
        let lazyDropBehavior = UIDynamicItemBehavior()
        lazyDropBehavior.allowsRotation = false
        lazyDropBehavior.elasticity = 0.0
        return lazyDropBehavior
    }()
    
    override init() {
        super.init()
        addChildBehavior(collider)
        addChildBehavior(dropBehavior)
    }
    
    func addBarrier(path: UIBezierPath, named name: String) {
        collider.removeBoundary(withIdentifier: name as NSCopying)
        collider.addBoundary(withIdentifier: name as NSCopying, for: path)
    }
    func removeBarrier(path: UIBezierPath, named name: String) {
        collider.removeBoundary(withIdentifier: name as NSCopying)
    }
    
    func changeGravity(path: UIBezierPath, named name: String) {
        gravity.gravityDirection = CGVector(dx: -path.bounds.midX, dy: -path.bounds.midY)
    }
    
    
    func addDrop(drop: UIDynamicItem) {
        //gravity.addItem(drop)
        collider.addItem(drop)
        dropBehavior.addItem(drop)
    }
    
    func removeDrop(drop: UIDynamicItem) {
        //gravity.removeItem(drop)
        collider.removeItem(drop)
        dropBehavior.removeItem(drop)
    }
}
