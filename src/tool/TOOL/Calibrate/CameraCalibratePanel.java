
/****************************
 * Simple Panel to tweak camera calibration values
 * @author Octavian Neamtu
 ***************************/

package TOOL.Calibrate;

import TOOL.Calibrate.Calibrate;
import javax.swing.*;
import java.awt.*;
import java.text.NumberFormat;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.beans.PropertyChangeEvent;
import java.beans.PropertyChangeListener;
//TODO fix commenting and formatting on this
public class CameraCalibratePanel extends JFrame implements PropertyChangeListener, ActionListener{

    private float[] calibrateArray;
    private JFormattedTextField[] valueFields;
    private JLabel[] labels;
    private int calibrateArraySize = 2;
    private NumberFormat textFieldFormat;
    private Calibrate calibrate;
    
    private enum CameraCalibrateItem{
        CAMERA_ROLL_ANGLE(0, "Camera Roll"),
        CAMERA_PITCH_ANGLE(1, "Camera Pitch");
        
    
        public final int index;
        public final String id;
    
        CameraCalibrateItem(int index, String id){
        this.index = index; this.id = id;
        }
    }

    public CameraCalibratePanel( Calibrate calibrate){
    super("Camera Calibrate");

    this.calibrate = calibrate;
    
    setLayout(new GridLayout(2,1));
    JPanel topPanel = new JPanel();
    topPanel.setLayout (new GridLayout(2,2));

    
    valueFields = new JFormattedTextField[calibrateArraySize];
    labels = new JLabel[calibrateArraySize];
    
    for (CameraCalibrateItem i: CameraCalibrateItem.values()){
           labels[i.index] = new JLabel(i.id);
           topPanel.add(labels[i.index]);
           valueFields[i.index] = new JFormattedTextField(textFieldFormat);
           topPanel.add(valueFields[i.index]);
           //valueFields[i.index].setValue(new Float(calibrateArray[i.index]));
           valueFields[i.index].addPropertyChangeListener("value", this);
           //System.out.println(i.id);
        }
    
    add(topPanel);
    
    JPanel bottomPanel = new JPanel();
    
    JButton getButton = new JButton("getData");
    getButton.setActionCommand("get");
    getButton.addActionListener(this);
    
    bottomPanel.add(getButton);
    
    JButton setButton = new JButton("setData");
    setButton.setActionCommand("set");
    setButton.addActionListener(this);
    
    bottomPanel.add(setButton);
    
    add(bottomPanel);
    pack();
    setVisible(true);
    }
    
    public void propertyChange(PropertyChangeEvent e){
        for (CameraCalibrateItem i: CameraCalibrateItem.values()){
            if (e.getSource().equals(valueFields[i.index]))
                if (valueFields[i.index].getValue() != null)
                    calibrateArray[i.index] = ((Number)valueFields[i.index].getValue()).floatValue();
        }
    }
    
    public void actionPerformed(ActionEvent e){
    if (e.getActionCommand().equals("get")){
        calibrateArray = calibrate.getVisionState().getThreshImage().getVisionLink().getCameraCalibrate();
        for (CameraCalibrateItem i: CameraCalibrateItem.values()){
            valueFields[i.index].setValue(new Float(calibrateArray[i.index]));
        }
    }
    if (e.getActionCommand().equals("set")){
        calibrate.getVisionState().getThreshImage().getVisionLink().setCameraCalibrate(calibrateArray);
    }
    
    }

};