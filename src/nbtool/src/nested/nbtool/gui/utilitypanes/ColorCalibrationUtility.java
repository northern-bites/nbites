package nbtool.gui.utilitypanes;

import java.awt.Dimension;

import javax.swing.JFrame;
import javax.swing.JSlider;
import javax.swing.JSpinner;
import javax.swing.event.ChangeEvent;
import javax.swing.event.ChangeListener;

import nbtool.util.Debug;


public class ColorCalibrationUtility extends UtilityParent {

	/* utility parent implementation code */
	/**************************************/
	
	private static Display display = null;
	
	@Override
	public JFrame supplyDisplay() {
		if (display != null) {
			return display;
		} else {
			return (display = new Display());
		}
	}

	@Override
	public String purpose() {
		return "calibrate the robot's vision color parameters";
	}

	@Override
	public char preferredMemnonic() {
		return 'c';
	}

	/* actual color code */
	/***************************/
	
	
	
	/* GUI code */
	/****************************/
	
	private static class Display extends JFrame {

		private final ColorCalibrationPanel panel = new ColorCalibrationPanel();
		
		private JSlider[] sliders = {
				panel.Y0USliderW,
				panel.Y0USliderG,
				panel.Y0VSliderW,
				panel.Y0VSliderG,
				panel.Y255USliderW,
				panel.Y255USliderG,
				panel.Y255VSliderW,
				panel.Y255VSliderG,
				panel.FuzzyUSliderW,
				panel.FuzzyUSliderG,
				panel.FuzzyVSliderW,
				panel.FuzzyVSliderG
				
		};
		
		private JSpinner[] spinners = {
				panel.Y0USpinnerW,
				panel.Y0USpinnerG,
				panel.Y0VSpinnerW,
				panel.Y0VSpinnerG,
				panel.Y255USpinnerW,
				panel.Y255USpinnerG,
				panel.Y255VSpinnerW,
				panel.Y255VSpinnerG,
				panel.FuzzyUSpinnerW,
				panel.FuzzyUSpinnerG,
				panel.FuzzyVSpinnerW,
				panel.FuzzyVSpinnerG
		};
		
		private void updateNB() {
			Debug.print("updating random");
		}
		
		private ChangeListener slideListener = new ChangeListener() {
			
			@Override
			public void stateChanged(ChangeEvent e) {
				for (int i = 0; i < spinners.length; ++i) {
					spinners[i].setValue( (Integer) sliders[i].getValue());
				}
				updateNB();
			}
		};
		
		private ChangeListener spinnerListener = new ChangeListener() {

			@Override
			public void stateChanged(ChangeEvent e) {
				for (int i = 0; i < sliders.length; ++i) {
					sliders[i].setValue( (Integer) spinners[i].getValue());
				}
				updateNB();
			}
			
		};
		
		Display() {
			super("color calibration utility");
			this.setContentPane(panel);
			
			this.setMinimumSize(new Dimension(600,200));
			
			for (JSlider slider : sliders) {
				slider.setMaximum(255);
				slider.setMinimum(0);
				slider.addChangeListener(slideListener);
			}
			for (JSpinner spinner : spinners) {
				spinner.addChangeListener(spinnerListener);
			}
		}
	}
	
}
