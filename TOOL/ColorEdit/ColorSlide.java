package TOOL.ColorEdit;

import java.awt.*;
import java.awt.event.*;
import javax.swing.*;
import javax.swing.event.*;

/*
 * General purpose slider for the ColorEditor.  Based on the SliderDemo.java from Sun's website.  The slider
 * adopts to take the range of whatever color is currently being displayed.
 */
public class ColorSlide extends JPanel
                        implements ChangeListener {

    public static final int STEP_SIZE = 1;
    public static final int BUTTON_WIDTH = 40;
    public static final int BUTTON_HEIGHT = BUTTON_WIDTH;
    ColorEdit editor;
    JSlider slider;
    JLabel sliderLabel;
    JButton plus, minus;
    
    public ColorSlide(ColorEdit _calibrate, int min, int max) {

	editor = _calibrate;
        
        setLayout(new BorderLayout());

        
        JPanel sliderStuff = new JPanel();
        sliderStuff.setLayout(new BoxLayout(sliderStuff, BoxLayout.LINE_AXIS));
        
        //Create the label.
        sliderLabel = new JLabel("Values", JLabel.CENTER);
        sliderLabel.setAlignmentX(Component.CENTER_ALIGNMENT);


        //Create the slider.
        slider = new JSlider(JSlider.HORIZONTAL,
                             min, max, 10);
        slider.addChangeListener(this);
        slider.setFocusable(false);

        //Turn on labels at major tick marks.
        slider.setMajorTickSpacing(20);
        //slider.setMinorTickSpacing(1);
        //slider.setPaintTicks(true);
        slider.setPaintLabels(true);
        slider.setBorder(BorderFactory.createEmptyBorder(0,0,10,0));


        // Create buttons on either side to step through
        // set focusable false so they do not consume keyboard focus
        minus = new JButton("-");
        minus.setFocusable(false);
        minus.addActionListener(new ActionListener() {
                public void actionPerformed(ActionEvent e) {
                    decrement(STEP_SIZE);
                }
            });
        minus.setPreferredSize(new Dimension(BUTTON_WIDTH, BUTTON_HEIGHT));
        minus.setMaximumSize(new Dimension(BUTTON_WIDTH, BUTTON_HEIGHT));



        plus = new JButton("+");
        plus.setFocusable(false);
        plus.addActionListener(new ActionListener() {
                public void actionPerformed(ActionEvent e) {
                    increment(STEP_SIZE);
                }
            });
        plus.setPreferredSize(new Dimension(BUTTON_WIDTH, BUTTON_HEIGHT));
        plus.setMaximumSize(new Dimension(BUTTON_WIDTH, BUTTON_HEIGHT));



        

        //Put everything together.
        // label goes to the top 
        add(sliderLabel, BorderLayout.PAGE_START);
        sliderStuff.add(minus);
        sliderStuff.add(slider);
        sliderStuff.add(plus);
        // all the rest goes right underneath
        add(sliderStuff, BorderLayout.CENTER);
        
      
        setBorder(BorderFactory.createEmptyBorder(10,10,10,10));
        setOpaque(true); //content panes must be opaque
    }

    /** Listen to the slider. */
    public void stateChanged(ChangeEvent e) {
        JSlider source = (JSlider)e.getSource();
        if (!source.getValueIsAdjusting()) {
            int val = (int)source.getValue();
	    editor.setValue(val);
        }
    }

    /* Set the range of values on the slider.
     * @param min - the new minimum value
     * @param max - the new maximum value
     */
    public void setRange(int min, int max) {
	slider.setMinimum(min);
	slider.setMaximum(max);
    }

    public void increment(int x) {
        int val = slider.getValue();
        if (val + x > slider.getMaximum()) { return; }
        // It's an acceptable value
        slider.setValue(val + x);

        // sliderLabel.setText(""+val + x);
    }

    public void decrement(int x) {
        int val = slider.getValue();
        if (val - x < slider.getMinimum()) { return; }
        // It's an acceptable value
        slider.setValue(val - x);
    }

    public void setText(String s) {
        sliderLabel.setText(s);
    }
    

    /* Give the slider a starting value.
     * @param n - the new value.
     */
    public void setValue(int n) {
	slider.setValue(n);
    }

    public void setEnabled(boolean choice) {
        slider.setEnabled(choice);
        plus.setEnabled(choice);
        minus.setEnabled(choice);
    }

}
