package TOOL.Calibrate;

import java.awt.Component;
import javax.swing.JLabel;
import javax.swing.JPanel;
import java.awt.BorderLayout;
import java.awt.event.ActionListener;
import java.awt.event.ActionEvent;
import javax.swing.JCheckBox;
import TOOL.Image.ImageOverlay;

import TOOL.GUI.IncrementalSlider;

/**
 * Modifies a slider to work with our calibration module, mainly to change the
 * thresholding value.  Increasing the threshold value means that it takes a
 * bigger jump in YUV value in order to classify as an edge; thus there will
 * be fewer edges drawn the higher the slider.
 * Ported to TOOL by Nicholas Dunn.
 * Modified to have +/- stepthrough buttons
 */


public class ThreshSlider extends IncrementalSlider {
    public static final int THRESH_MIN = ImageOverlay.MIN_THRESH;
    public static final int THRESH_MAX = ImageOverlay.MAX_THRESH;
    public static final int THRESH_INIT = ImageOverlay.DEFAULT_THRESH;
    public static final int MAJOR_TICK_SPACING = 10;
    public static final int MINOR_TICK_SPACING = 1;

    protected Calibrate calibrate;
    protected JCheckBox enabled;


    public ThreshSlider(Calibrate _calibrate) {
        super(_calibrate, "Threshold", THRESH_MIN, THRESH_MAX);
        calibrate = _calibrate;

        enabled = new JCheckBox("Edge Thresholding Enabled (E)");

        //enabled.setAlignmentX(Component.RIGHT_ALIGNMENT);
	
        super.add(enabled, BorderLayout.PAGE_END);
        enabled.addActionListener(new ActionListener() {
                public void actionPerformed(ActionEvent e) {
                    setEnabled(enabled.isSelected());
                    calibrate.setThresholded(enabled.isSelected());
                }
            });
	

        // Don't allow the checkbox to consume focus, but start it selected
        // by default
        enabled.setFocusable(false);
        enabled.setSelected(true);
	
        // Make sure the ticks are drawn correctly
        slider.setMajorTickSpacing(MAJOR_TICK_SPACING);
        slider.setMinorTickSpacing(MINOR_TICK_SPACING);
        slider.setPaintTicks(true);
        slider.setPaintLabels(true);
        slider.setValue(THRESH_INIT);
    }
    
    public void clickEnabled() {
        enabled.doClick();
    }
    

}
