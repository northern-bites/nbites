//
//  ControlViewController.swift
//  nbControl
//
//  Created by Cory Alini on 11/2/16.
//  Copyright © 2016 gis.coryalini. All rights reserved.
//

import UIKit



class ControlViewController: UIViewController,UIPopoverPresentationControllerDelegate {

    var mainCenter: CGPoint! = CGPoint(x: 0.0, y: 0.0)
    var newPoint: CGPoint = CGPoint(x: 0.0, y: 0.0)

    var controlBrain = ControlBrain()
    
    @IBOutlet var controlView: ControlView! {
        didSet {
            controlView.addGestureRecognizer(UIPanGestureRecognizer(target: self, action: #selector(doPanGestureFunctions(_:))))
            controlView.addGestureRecognizer(UILongPressGestureRecognizer(target: controlView, action:  #selector(controlView.removeSnap(_:))))
            controlView.realGravity = true
            mainCenter = controlView.circleCenter
        }
    }
    
    func doPanGestureFunctions(_ sender: UIPanGestureRecognizer) {
        controlView.grabDrop(sender)
        setNewDistance(gesture: sender)
    }
    
    func setNewDistance(gesture: UIPanGestureRecognizer) {
        let target = gesture.view!
        
        switch gesture.state {
        case .began:
            break
        case .changed:
            let translation = gesture.translation(in: controlView)
            newPoint = CGPoint(x:translation.x, y: translation.y)
            //newPoint = CGPoint(x: mainCenter!.x + translation.x, y: mainCenter!.y + translation.y)
        case .ended:
            print("Translation: [\(newPoint.x),\(newPoint.y)]")
            let currentPlace = CGPoint(x: controlView.currentPosition.x - target.bounds.midX, y: controlView.currentPosition.y - target.bounds.midY )
            controlBrain.sendNewWalk(currPosition: currentPlace,transition: newPoint)
        default: break
        }
    }
    override func viewDidAppear(_ animated: Bool) {
        super.viewDidAppear(animated)
        controlView.animating = true
    }
    
    override func viewWillDisappear(_ animated: Bool) {
        super.viewWillDisappear(animated)
        controlView.animating = false
    }
    
    
    
}



















//class ControlViewController: UIViewController {
//    
//    @IBInspectable
//    var lineWidth: CGFloat = 5.0
//    
//    @IBInspectable
//    var radius: CGFloat = 75.0
//    
//    
//    var circleCenter: CGPoint!
//    
//    internal var centerPoint: CGPoint {
//        return CGPoint(x: view.bounds.midX, y: view.bounds.midY)
//    }
//    
//    
//    var circleAnimator: UIViewPropertyAnimator?
//    
//    var anchorView: UIView!
//    var circle: UIView!
//    var animator: UIDynamicAnimator!
//    var gravity: UIGravityBehavior!
//    
//    var attachment: UIAttachmentBehavior!
//    
//    
//    override func viewDidLoad() {
//        super.viewDidLoad()
//        
//        circle = UIView(frame: CGRect(x: 0.0, y: 0.0, width: 60.0, height: 60.0))
//        circle.center = self.view.center
//        circle.layer.cornerRadius = 30.0
//        circle.backgroundColor = UIColor.blue
//        
//        circle.addGestureRecognizer(UIPanGestureRecognizer(target: self, action: #selector(self.dragCircle)))
//        
//        self.view.addSubview(circle)
//        
//        let base = CAShapeLayer()
//        
//        base.path = UIBezierPath(
//            arcCenter: view.center,
//            radius: radius,
//            startAngle: CGFloat(0),
//            endAngle:CGFloat(M_PI * 2),
//            clockwise: true).cgPath
//        
//        base.lineWidth = lineWidth
//        base.fillColor = UIColor.clear.cgColor
//        base.strokeColor = UIColor.red.cgColor
//        view.layer.addSublayer(base)
//        
//    }
//    
//        func dragCircle(gesture: UIPanGestureRecognizer) {
//            let target = gesture.view!
//    
//            switch gesture.state {
//            case .began:
//                if circleAnimator != nil && circleAnimator!.isRunning {
//                    circleAnimator!.stopAnimation(false)
//                }
//                circleCenter = target.center
//            case .changed:
//                let translation = gesture.translation(in: self.view)
//                target.center = CGPoint(x: circleCenter!.x + translation.x, y: circleCenter!.y + translation.y)
//            case .ended:
//                let v = gesture.velocity(in: target)
//    
//                let velocity = CGVector(dx: v.x / 500, dy: v.y / 500)
//                let springParameters = UISpringTimingParameters(mass: 2.5, stiffness: 70, damping: 55, initialVelocity: velocity)
//                circleAnimator = UIViewPropertyAnimator(duration: 0.0, timingParameters: springParameters)
//    
//                circleAnimator!.addAnimations({
//                    target.center = self.view.center
//                })
//                circleAnimator!.startAnimation()
//            default: break
//            }
//        }
//
