//
//  ControlViewController.swift
//  nbControl
//
//  Created by Cory Alini on 11/2/16.
//  Copyright © 2016 gis.coryalini. All rights reserved.
//

import UIKit



class ControlViewController: UIViewController,UIPopoverPresentationControllerDelegate {

    
    @IBOutlet var controlView: ControlView! {
        didSet {
           //controlView.addGestureRecognizer(UITapGestureRecognizer(target: self, action: #selector(addDrop(_:))))
            controlView.addGestureRecognizer(UIPanGestureRecognizer(target: controlView, action: #selector(controlView.grabDrop(_:))))
            controlView.addGestureRecognizer(UILongPressGestureRecognizer(target: controlView, action:  #selector(controlView.removeSnap(_:))))
            controlView.realGravity = true
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
    
    @IBAction func barPopover(_ sender: AnyObject) {
        //self.performSegueWithIdentifier("performPopover", sender: self)
        let popoverVC = UIStoryboard(name: "Main", bundle: nil).instantiateViewController(withIdentifier: "sbPopup") as! MenuViewController
        
        self.addChildViewController(popoverVC)
        popoverVC.view.frame = self.view.frame
        self.view.addSubview(popoverVC.view)
        popoverVC.didMove(toParentViewController: self)
        
        
    }
    override func prepare(for segue: UIStoryboardSegue, sender: Any?) {
        
        
        var destinationvc = segue.destination
        
        if let navCon = destinationvc as? UINavigationController {
            destinationvc = navCon.visibleViewController ?? destinationvc
        }
        
        if segue.identifier == "performMenu" {
            print("beep")
            if let popvc = destinationvc as? MenuViewController {
                let controller = popvc.popoverPresentationController
                if controller != nil {
                    controller?.delegate = self
                }
            }
            
        }
    }
    func adaptivePresentationStyle(for controller: UIPresentationController) -> UIModalPresentationStyle {
        return .none
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
//    
//    
//    
//    
//    
//}
