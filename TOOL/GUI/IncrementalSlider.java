package TOOL.GUI;

import java.awt.*;
import java.awt.event.*;
import javax.swing.*;
import javax.swing.event.*;

/**
 * General purpose slider.  Includes buttons on either side that allow you to
 * increment the slider
 */
public class IncrementalSlider extends JPanel implements ChangeListener,
                                                         ActionListener{
    public static final int INIT_VAL = 10;    
    public static final int DEFAULT_STEP_SIZE = 1;
    IncrementalSliderParent parent;
    protected JSlider slider;
    protected JLabel sliderLabel;
    protected JButton plus, minus;
    
    int stepSize;

    public static final int BORDER_SIZE = 5;

    public IncrementalSlider(IncrementalSliderParent parent, String title, 
                             int min, int max, int initVal) {
        this.parent = parent;
        stepSize = DEFAULT_STEP_SIZE;

        setLayout(new BorderLayout());

        
        JPanel sliderStuff = new JPanel();
        sliderStuff.setLayout(new BoxLayout(sliderStuff, BoxLayout.LINE_AXIS));
        
        //Create the label.
        sliderLabel = new JLabel(title, JLabel.CENTER);
        sliderLabel.setAlignmentX(Component.CENTER_ALIGNMENT);


        //Create the slider.
        slider = new JSlider(JSlider.HORIZONTAL,
                             min, max, initVal);
        slider.addChangeListener(this);
        slider.setFocusable(false);

        //Turn on labels at major tick marks.
        slider.setMajorTickSpacing(20);
        slider.setMinorTickSpacing(5);
        slider.setPaintTicks(true);
        slider.setPaintLabels(true);
        slider.setBorder(BorderFactory.createEmptyBorder(0,0,10,0));


        // Create buttons on either side to step through
        // set focusable false so they do not consume keyboard focus
        minus = new JButton("-");
        minus.setFocusable(false);
        minus.addActionListener(this);
        
        plus = new JButton("+");
        plus.setFocusable(false);
        plus.addActionListener(this);
        

        //Put everything together.
        // label goes to the Bottom 
        add(sliderLabel, BorderLayout.PAGE_START);
        sliderStuff.add(minus);
        sliderStuff.add(slider);
        sliderStuff.add(plus);
        // all the rest goes right underneath
        add(sliderStuff, BorderLayout.CENTER);
      
        setBorder(BorderFactory.createEmptyBorder(BORDER_SIZE,
                                                  BORDER_SIZE,
                                                  BORDER_SIZE,
                                                  BORDER_SIZE));
        setOpaque(true); //content panes must be opaque
        
    }

    public IncrementalSlider(IncrementalSliderParent parent, String title, 
                      int min, int max) {
        this(parent, title, min, max, INIT_VAL);
    }



    public void setPlusLabel(String s) {
        plus.setText(s);
    }

    public void setMinusLabel(String s) {
        minus.setText(s);
    }



    /** Listen to the plus/minus clicks */
    public void actionPerformed(ActionEvent e) {
        JButton source = (JButton) e.getSource();
        if (source.equals(minus)) {
            decrement(stepSize);
        }
        else if (source.equals(plus)) {
            increment(stepSize);
        }
    }

    // Programmatically press the minus button without physically clicking on it
    public void pressMinus() {
        minus.doClick();
    }

    // Programmatically press the plus button without physically clicking on it
    public void pressPlus() {
        plus.doClick();
    }


    /** Listen to the slider. */
    public void stateChanged(ChangeEvent e) {
        JSlider source = (JSlider)e.getSource();
        if (!source.getValueIsAdjusting()) {
            int val = (int)source.getValue();
	    parent.setValue(val);
        }
    }

    /** Allows parent to change the step size of a plus/minus click */
    public void setStepSize(int n) {
        stepSize = n;
    }



    /** Set the range of values on the slider.
     * @param min - the new minimum value
     * @param max - the new maximum value
     */
    public void setRange(int min, int max) {
	slider.setMinimum(min);
	slider.setMaximum(max);
    }

    /** Give the slider a starting value.
     * @param n - the new value.
     */
    public void setValue(int n) {
	slider.setValue(n);
    }


    /** Adds x to the current value of the slider, or if that would cause it
     * to go over its max, puts it to the max.
     */
    public void increment(int x) {
        int val = slider.getValue();
        // Clamp to be the max
        if (val + x > slider.getMaximum()) {
            slider.setValue(slider.getMaximum());
        }
        // It's an acceptable value
        slider.setValue(val + x);
    }

    /**
     * Subtracts x from the current value of the slider.  If that would cause it
     * to drop below the minimum, sets the slider to minimum.
     */
    public void decrement(int x) {
        int val = slider.getValue();
        // Clamp to be the minimum
        if (val - x < slider.getMinimum()) {
            slider.setValue(slider.getMinimum());
        }
        // It's an acceptable value
        slider.setValue(val - x);
    }

    /**
     * Changes the text in the label to string s
     */
    public void setText(String s) {
        sliderLabel.setText(s);
    }

    /**
     * Turns on or off all of the components (greys them out and renders them
     * unclickable)
     */
    public void setEnabled(boolean choice) {
        minus.setEnabled(choice);
        slider.setEnabled(choice);
        plus.setEnabled(choice);
        sliderLabel.setEnabled(choice);
    }


    public void setPlusEnabled(boolean choice) {
        plus.setEnabled(choice);
    }

    public void setMinusEnabled(boolean choice) {
        minus.setEnabled(choice);
    }
  
    public int getValue() { return slider.getValue(); }
   
}
