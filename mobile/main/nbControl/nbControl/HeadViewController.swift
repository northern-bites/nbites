//
//  HeadViewController.swift
//  nbControl
//
//  Created by Cory Alini on 11/20/16.
//  Copyright © 2016 gis.coryalini. All rights reserved.
//
import UIKit

class HeadViewController: UIViewController {
    @IBOutlet var headPlaceholder: UIView!
    @IBOutlet var valueLabel: UILabel!
    @IBOutlet var valueSlider: UISlider!
    
    @IBOutlet weak var robotHead: UIImageView!
    
    
    let cambridge_blue = UIColor(colorLiteralRed: 163.0/255.0, green: 187.0/255.0, blue: 173.0/255.0, alpha: 1.0)
    let jet = UIColor(colorLiteralRed: 46.0/255.0, green: 53.0/255.0, blue: 50.0/255.0, alpha: 1.0)
    let light_slate_gray = UIColor(colorLiteralRed: 200.0/255.0, green: 199.0/255.0, blue: 200.0/255.0, alpha: 1.0)
    
    
    override func viewDidLoad() {
        super.viewDidLoad()
        self.view.backgroundColor = jet;
        valueSlider.tintColor = light_slate_gray
        valueLabel.textColor = light_slate_gray
        view.tintColor = cambridge_blue
        robotHead.addGestureRecognizer(RotationGestureRecognizer(target: self, action: #selector(handleRotation(_:))))

        value = valueSlider.value
        update()
    }
    
    @IBAction func finishedSliding(_ sender: Any) {
        sendNewValue(value)
    }
    
    func headValueChanged() {
        valueSlider.value = value
        update()
    }
    
    @IBAction func sliderValueChanged(_ slider: UISlider) {
        value = slider.value
        update()
    }
    
    func update() {
        valueLabel.text = NumberFormatter.localizedString(from: NSNumber(floatLiteral: Double(value)), number: .decimal)
    }
    func sendNewValue(_ value: Float) {
        var robot = blankRobotCommand()
        
        robot.adjustHead = true
        robot.adjustedHeadY = value * 240 - 120
        if (robotManager.currentAddress() != nil) {
            print("Sending head move to robot")
            robotManager.send(robot)
        }
    }

    private var backingValue: Float = 0.0
    var value: Float {
        get { return backingValue }
        set { setValue(newValue, animated: false) }
    }
    
    let startAngle:CGFloat = -CGFloat(M_PI * 11.0 / 8.0);
    let endAngle:CGFloat = CGFloat(M_PI * 3.0 / 8.0);
    let minimumValue: Float = 0.0
    let maximumValue: Float = 1.0
    func handleRotation(_ sender: AnyObject) {
        print("Dealing with rotations")
        let gr = sender as! RotationGestureRecognizer
        
        // 1. Mid-point angle
        let midPointAngle = (2.0 * CGFloat(M_PI) + self.startAngle - self.endAngle) / 2.0 + self.endAngle
        
        // 2. Ensure the angle is within a suitable range
        var boundedAngle = gr.rotation
        if boundedAngle > midPointAngle {
            boundedAngle -= 2.0 * CGFloat(M_PI)
        } else if boundedAngle < (midPointAngle - 2.0 * CGFloat(M_PI)) {
            boundedAngle += 2 * CGFloat(M_PI)
        }
        
        // 3. Bound the angle to within the suitable range
        boundedAngle = min(self.endAngle, max(self.startAngle, boundedAngle))
        
        // 4. Convert the angle to a value
        let angleRange = endAngle - startAngle
        let valueRange = maximumValue - minimumValue
        let valueForAngle = Float(boundedAngle - startAngle) / Float(angleRange) * valueRange + minimumValue
        
        // 5. Set the control to this value
        self.value = valueForAngle
    
    }
    private func setValue(_ currValue: Float, animated: Bool) {
        var value = currValue
        if(value != self.value) {
            let midPointAngle = (2.0 * CGFloat(M_PI) + self.startAngle - self.endAngle) / 2.0 + self.endAngle
            if CGFloat(value) > midPointAngle {
                value -= Float(2.0 * M_PI)
            } else if CGFloat(value) < (midPointAngle - 2.0 * CGFloat(M_PI)) {
                value += Float(2 * M_PI)
            }
            self.backingValue = min(self.maximumValue, max(self.minimumValue, value))
            let angleRange = endAngle - startAngle
            let valueRange = CGFloat(maximumValue - minimumValue)
            let angle = CGFloat(value - minimumValue) / valueRange * angleRange + startAngle
            setmyHeadAngle(angle)
            //print("backingVal: \(backingValue) angle: \(angle) valueRange: \(valueRange) angleRange: \(angleRange)")
            
        }
    }
    var backingHeadAngle: CGFloat = 0.0
    var headAngle: CGFloat {
        get { return backingHeadAngle }
        set { setmyHeadAngle(newValue) }
    }
    
    func setmyHeadAngle(_ myheadAngle: CGFloat) {
        CATransaction.begin()
        CATransaction.setDisableActions(true)
        
        robotHead.layer.transform = CATransform3DMakeRotation(myheadAngle, 0.0, 0.0, 0.1)
        
        let midAngle = (max(myheadAngle, self.headAngle) - min(myheadAngle, self.headAngle) ) / 2.0 + min(myheadAngle, self.headAngle)
            let animation = CAKeyframeAnimation(keyPath: "transform.rotation.z")
            animation.duration = 0.25
            
            animation.values = [self.headAngle, midAngle, myheadAngle]
            animation.keyTimes = [0.0, 0.5, 1.0]
            animation.timingFunction = CAMediaTimingFunction(name: kCAMediaTimingFunctionEaseInEaseOut)
            robotHead.layer.add(animation, forKey: nil)

        CATransaction.commit()
        
        self.backingHeadAngle = myheadAngle

    }
    

    
}

    
    
import UIKit.UIGestureRecognizerSubclass

private class RotationGestureRecognizer: UIPanGestureRecognizer {
    var rotation: CGFloat = 0.0
    
    override init(target: Any?, action: Selector?) {
        super.init(target: target, action: action)
        
        minimumNumberOfTouches = 1
        maximumNumberOfTouches = 1
    }
    override func touchesBegan(_ touches: Set<UITouch>, with event: UIEvent) {
        super.touchesBegan(touches , with: event)
        updateRotationWithTouches(touches)
        
    }
    
    override func touchesMoved(_ touches: Set<UITouch>, with event: UIEvent) {
        super.touchesMoved(touches as! Set<UITouch>, with: event)
        updateRotationWithTouches(touches)
    }
    
    func updateRotationWithTouches(_ touches: Set<NSObject>) {
        if let touch = touches[touches.startIndex] as? UITouch {
            self.rotation = rotationForLocation(touch.location(in: self.view))
        }
    }
    
    func rotationForLocation(_ location: CGPoint) -> CGFloat {
        let offset = CGPoint(x: location.x - view!.bounds.midX, y: location.y - view!.bounds.midY)
        return atan2(offset.y, offset.x)
    }
}
