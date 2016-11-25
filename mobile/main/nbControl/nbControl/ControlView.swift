//
//  ControlView.swift
//  nbControl
//
//  Created by Cory Alini on 11/17/16.
//  Copyright © 2016 gis.coryalini. All rights reserved.
//


import UIKit
import CoreMotion

class ControlView: BezierPathsView, UIDynamicAnimatorDelegate{
    
    private var snapBehaviour:UISnapBehavior!
    private var circleCenter: CGPoint!

    var realGravity = false
    private let circleSize = 10
    
    private let motionManager = CMMotionManager()
    
    private var dropSize: CGSize {
        let size = bounds.size.width / CGFloat(circleSize)
        return CGSize(width:size, height: size)
    }
    
    private lazy var animator: UIDynamicAnimator = {
        let lazyAnimator = UIDynamicAnimator(referenceView: self)
        lazyAnimator.delegate = self
        return lazyAnimator
    }()
    
    private let dropBehavior = DropItBehavior()
    
    private var attachment: UIAttachmentBehavior? {
        willSet {
            if attachment != nil {
                animator.removeBehavior(attachment!)
                bezierPaths[PathNames.Attachment] = nil
            }
        }
        didSet {
            if attachment != nil {
                animator.addBehavior(attachment!)
            }
        }
    }
    
    private var lastDropped : UIView?
    
    private struct PathNames {
        static let MiddleBarrier = "MiddleBarrier"
        static let Attachment = "Attachment"
    }
    
    override func layoutSubviews() {
        super.layoutSubviews()
        let barrierSize = dropSize
        let barrierOrigin = CGPoint(x: bounds.midX-barrierSize.width/2, y: bounds.midY - barrierSize.height/2)
        let path = UIBezierPath(ovalIn: CGRect(origin: barrierOrigin, size: barrierSize))
        dropBehavior.addBarrier(path: path, named: PathNames.MiddleBarrier)
        bezierPaths[PathNames.MiddleBarrier] = path
        addController()
    }
    
    func addController() {
        let drop = UIView(frame: CGRect(x: self.center.x - CGFloat(30), y: self.center.y - CGFloat(30), width: 60.0, height: 60.0))
        drop.layer.cornerRadius = 30.0
        drop.backgroundColor = UIColor.blue
        addSubview(drop)
        dropBehavior.addDrop(drop: drop)
        lastDropped = drop
    
        circleCenter = lastDropped?.center
    }
    
    var animating: Bool = false {
        didSet {
            if animating {
                animator.addBehavior(dropBehavior)
                //updateRealGravity()
                //changeGrav()
            } else {
                animator.removeBehavior(dropBehavior)
            }
        }
    }
    func removeSnap(_ sender: UILongPressGestureRecognizer ) {
        sender.minimumPressDuration = 0.5
        if snapBehaviour != nil {
            animator.removeBehavior(snapBehaviour)
        }
    }
    
    func grabDrop(_ sender: UIPanGestureRecognizer) {
        let gesturePoint = sender.location(in: self)
        
        switch sender.state {
        case .began:
            if let viewToAttachTo = lastDropped, viewToAttachTo.superview != nil {
                dropBehavior.removeBarrier(path: bezierPaths[PathNames.MiddleBarrier]!,named: PathNames.MiddleBarrier)
                if snapBehaviour != nil {
                    animator.removeBehavior(snapBehaviour)
                }
                attachment = UIAttachmentBehavior(item: viewToAttachTo, attachedToAnchor: gesturePoint)
            }
        case .changed:
            attachment?.anchorPoint = gesturePoint
        case .ended:
            attachment = nil
            dropBehavior.addBarrier(path: bezierPaths[PathNames.MiddleBarrier]!,named: PathNames.MiddleBarrier)
            snapBehaviour = UISnapBehavior(item: lastDropped!, snapTo: circleCenter)
            snapBehaviour.damping = 5
            animator.addBehavior(snapBehaviour)

            
        default: break
        }
    }
    
}

