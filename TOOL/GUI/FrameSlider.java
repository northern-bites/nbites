package edu.bowdoin.robocup.TOOL.GUI;

/**
 * @author Nicholas Dunn
 * @date   05/10/08
 */
public class FrameSlider extends IncrementalSlider {

    public FrameSlider(IncrementalSliderParent parent, int min, int max) {
        super(parent, "Frame #", 0, 1, 0);
        setPlusLabel("Next");
        setMinusLabel("Previous");
        setToolTipText("Changes the currently displayed image");
    }
    

}