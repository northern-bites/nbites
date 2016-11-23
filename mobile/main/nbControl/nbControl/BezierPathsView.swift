//
//  BezierPathsView.swift
//  DropIt
//
//  Created by Eric Chown on 11/9/16.
//  Copyright © 2016 edu.bowdoin.cs2505.chown. All rights reserved.
//

import UIKit

class BezierPathsView: UIView {

    var bezierPaths = [String:UIBezierPath]() {
        didSet {
            setNeedsDisplay()
        }
    }
    
    func setPath(path: UIBezierPath?, named name: String) {
        bezierPaths[name] = path
        setNeedsDisplay()
    }
    
    
    // Only override drawRect: if you perform custom drawing.
    // An empty implementation adversely affects performance during animation.
    override func draw(_ rect: CGRect) {
        for (_, path) in bezierPaths {
            path.stroke()
        }
    }
 

}
