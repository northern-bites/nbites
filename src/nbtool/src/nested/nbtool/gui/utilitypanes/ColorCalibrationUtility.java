package nbtool.gui.utilitypanes;

import java.awt.Dimension;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.io.IOException;
import java.nio.charset.StandardCharsets;
import java.nio.file.Files;
import java.nio.file.Path;

import javax.swing.JFrame;
import javax.swing.JSlider;
import javax.swing.JSpinner;
import javax.swing.event.ChangeEvent;
import javax.swing.event.ChangeListener;

import nbtool.data.calibration.ColorParam;
import nbtool.data.calibration.ColorParam.Set;
import nbtool.data.json.Json;
import nbtool.data.json.JsonParser.JsonParseException;
import nbtool.gui.ToolMessage;
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
 
	/* GUI code */
	/****************************/
	
	private static class Display extends JFrame {

		private final ColorCalibrationPanel panel = new ColorCalibrationPanel();
		
		private JSlider[] sliders = {
				panel.Y0USliderWT,
				panel.Y0VSliderWT,
				panel.Y255USliderWT,
				panel.Y255VSliderWT,
				panel.FuzzyUSliderWT,
				panel.FuzzyVSliderWT,
				panel.Y0USliderWB,
				panel.Y0VSliderWB,
				panel.Y255USliderWB,
				panel.Y255VSliderWB,
				panel.FuzzyUSliderWB,
				panel.FuzzyVSliderWB,
				panel.Y0USliderGT,
				panel.Y0VSliderGT,
				panel.Y255USliderGT,
				panel.Y255VSliderGT,
				panel.FuzzyUSliderGT,
				panel.FuzzyVSliderGT,
				panel.Y0USliderGB,
				panel.Y0VSliderGB,
				panel.Y255USliderGB,
				panel.Y255VSliderGB,
				panel.FuzzyUSliderGB,
				panel.FuzzyVSliderGB
				
		};
		
		private JSpinner[] spinners = {
				panel.Y0USpinnerWT,
				panel.Y0VSpinnerWT,
				panel.Y255USpinnerWT,
				panel.Y255VSpinnerWT,
				panel.FuzzyUSpinnerWT,
				panel.FuzzyVSpinnerWT,
				panel.Y0USpinnerWB,
				panel.Y0VSpinnerWB,
				panel.Y255USpinnerWB,
				panel.Y255VSpinnerWB,
				panel.FuzzyUSpinnerWB,
				panel.FuzzyVSpinnerWB,
				panel.Y0USpinnerGT,
				panel.Y0VSpinnerGT,
				panel.Y255USpinnerGT,
				panel.Y255VSpinnerGT,
				panel.FuzzyUSpinnerGT,
				panel.FuzzyVSpinnerGT,
				panel.Y0USpinnerGB,
				panel.Y0VSpinnerGB,
				panel.Y255USpinnerGB,
				panel.Y255VSpinnerGB,
				panel.FuzzyUSpinnerGB,
				panel.FuzzyVSpinnerGB
		};
		
		private void updateNB(ChangeEvent e) {
			
			ColorParam.Set set = new ColorParam.Set();
			
			set.getTop().white.uAtY0 = (double) panel.Y0USliderWT.getValue();
			set.getTop().white.vAtY0 = (double) panel.Y0VSliderWT.getValue();
			set.getTop().white.uAtY255 = (double) panel.Y255USliderWT.getValue();
			set.getTop().white.vAtY255 = (double) panel.Y255VSliderWT.getValue();
			set.getTop().white.u_fuzzy_range = (double) panel.FuzzyUSliderWT.getValue();
			set.getTop().white.v_fuzzy_range = (double) panel.FuzzyVSliderWT.getValue();
			set.getBot().white.uAtY0 = (double) panel.Y0USliderWB.getValue();
			set.getBot().white.vAtY0 = (double) panel.Y0VSliderWB.getValue();
			set.getBot().white.uAtY255 = (double) panel.Y255USliderWB.getValue();
			set.getBot().white.vAtY255 = (double) panel.Y255VSliderWB.getValue();
			set.getBot().white.u_fuzzy_range = (double) panel.FuzzyUSliderWB.getValue();
			set.getBot().white.v_fuzzy_range = (double) panel.FuzzyVSliderWB.getValue();
			set.getTop().green.uAtY0 = (double) panel.Y0USliderGT.getValue();
			set.getTop().green.vAtY0 =  (double) panel.Y0VSliderGT.getValue();
			set.getTop().green.uAtY255 = (double) panel.Y255USliderGT.getValue();
			set.getTop().green.vAtY255 = (double) panel.Y255VSliderGT.getValue();
			set.getTop().green.u_fuzzy_range = (double) panel.FuzzyUSliderGT.getValue();
			set.getTop().green.v_fuzzy_range = (double) panel.FuzzyVSliderGT.getValue();
			set.getBot().green.uAtY0 = (double) panel.Y0USliderGB.getValue();
			set.getBot().green.vAtY0 = (double) panel.Y0VSliderGB.getValue();
			set.getBot().green.uAtY255 = (double) panel.Y255USliderGB.getValue();
			set.getBot().green.vAtY255 = (double) panel.Y255VSliderGB.getValue();
			set.getBot().green.u_fuzzy_range = (double) panel.FuzzyUSliderGB.getValue();
			set.getBot().green.v_fuzzy_range = (double) panel.FuzzyVSliderGB.getValue();
	
			set.getTop().white.uAtY0 = (Integer) panel.Y0USpinnerWT.getValue();
			set.getTop().white.vAtY0 = (Integer) panel.Y0VSpinnerWT.getValue();
			set.getTop().white.uAtY255 = (Integer) panel.Y255USpinnerWT.getValue();
			set.getTop().white.vAtY255 = (Integer) panel.Y255VSpinnerWT.getValue();
			set.getTop().white.u_fuzzy_range = (Integer) panel.FuzzyUSpinnerWT.getValue();
			set.getTop().white.v_fuzzy_range = (Integer) panel.FuzzyVSpinnerWT.getValue();
			set.getBot().white.uAtY0 = (Integer) panel.Y0USpinnerWB.getValue();
			set.getBot().white.vAtY0 = (Integer) panel.Y0VSpinnerWB.getValue();
			set.getBot().white.uAtY255 = (Integer) panel.Y255USpinnerWB.getValue();
			set.getBot().white.vAtY255 = (Integer) panel.Y255VSpinnerWB.getValue();
			set.getBot().white.u_fuzzy_range = (Integer) panel.FuzzyUSpinnerWB.getValue();
			set.getBot().white.v_fuzzy_range = (Integer) panel.FuzzyVSpinnerWB.getValue();
			set.getTop().green.uAtY0 = (Integer) panel.Y0USpinnerGT.getValue();
			set.getTop().green.vAtY0 =  (Integer) panel.Y0VSpinnerGT.getValue();
			set.getTop().green.uAtY255 = (Integer) panel.Y255USpinnerGT.getValue();
			set.getTop().green.vAtY255 = (Integer) panel.Y255VSpinnerGT.getValue();
			set.getTop().green.u_fuzzy_range = (Integer) panel.FuzzyUSpinnerGT.getValue();
			set.getTop().green.v_fuzzy_range = (Integer) panel.FuzzyVSpinnerGT.getValue();
			set.getBot().green.uAtY0 = (Integer) panel.Y0USpinnerGB.getValue();
			set.getBot().green.vAtY0 = (Integer) panel.Y0VSpinnerGB.getValue();
			set.getBot().green.uAtY255 = (Integer) panel.Y255USpinnerGB.getValue();
			set.getBot().green.vAtY255 = (Integer) panel.Y255VSpinnerGB.getValue();
			set.getBot().green.u_fuzzy_range = (Integer) panel.FuzzyUSpinnerGB.getValue();
			set.getBot().green.v_fuzzy_range = (Integer) panel.FuzzyVSpinnerGB.getValue();
	
			Debug.print("Changing parameter: %s",e);
		}
		
		private ChangeListener slideListener = new ChangeListener() {
			
			@Override
			public void stateChanged(ChangeEvent e) {
				for (int i = 0; i < spinners.length; ++i) {
					spinners[i].setValue( (Integer) sliders[i].getValue());
				}
				updateNB(e);
			}
		};
		
		private ChangeListener spinnerListener = new ChangeListener() {

			@Override
			public void stateChanged(ChangeEvent e) {
				for (int i = 0; i < sliders.length; ++i) {
					sliders[i].setValue( (Integer) spinners[i].getValue());
				}
				updateNB(e);
			}
			
		};
		
		Display() {
			super("color calibration utility");
			
			Debug.plain("getting initial color parameters");
			Path colorPath = ColorParam.getPath();
			assert(Files.exists(colorPath) && Files.isRegularFile(colorPath));
			
			ColorParam.Set set = null;
			
			try {
				String contents = new String(Files.readAllBytes(colorPath));
				set = ColorParam.Set.parse(Json.parse(contents).asObject());
			} catch (IOException ie) {
				ie.printStackTrace();
				Debug.error("IOExceptionL %s", ie.getMessage());
				ToolMessage.displayError("error parsing json file %s", colorPath);
				return;
			} catch (JsonParseException e1) {
				e1.printStackTrace();
				Debug.error("JsonParseException: %s", e1.getMessage());
            	ToolMessage.displayError("error parsing json file %s !",colorPath);
            	return;
			}
			
			assert(set != null);
			
			Debug.print("setting");
						
			panel.Y0USliderWT.setValue((int) set.getTop().white.uAtY0);
			panel.Y0VSliderWT.setValue((int) set.getTop().white.vAtY0);
			panel.Y255USliderWT.setValue((int) set.getTop().white.uAtY255);
			panel.Y255VSliderWT.setValue((int) set.getTop().white.vAtY255);
			panel.FuzzyUSliderWT.setValue((int) set.getTop().white.u_fuzzy_range);
			panel.FuzzyVSliderWT.setValue((int) set.getTop().white.v_fuzzy_range);
			panel.Y0USliderWB.setValue((int) set.getBot().white.uAtY0);
			panel.Y0VSliderWB.setValue((int) set.getBot().white.vAtY0);
			panel.Y255USliderWB.setValue((int) set.getBot().white.uAtY255);
			panel.Y255VSliderWB.setValue((int) set.getBot().white.vAtY255);
			panel.FuzzyUSliderWB.setValue((int) set.getBot().white.u_fuzzy_range);
			panel.FuzzyVSliderWB.setValue((int) set.getBot().white.v_fuzzy_range);
			panel.Y0USliderGT.setValue((int) set.getTop().green.uAtY0);
			panel.Y0VSliderGT.setValue((int) set.getTop().green.vAtY0);
			panel.Y255USliderGT.setValue((int) set.getTop().green.uAtY255);
			panel.Y255VSliderGT.setValue((int) set.getTop().green.vAtY255);
			panel.FuzzyUSliderGT.setValue((int) set.getTop().green.u_fuzzy_range);
			panel.FuzzyVSliderGT.setValue((int) set.getTop().green.v_fuzzy_range);
			panel.Y0USliderGB.setValue((int) set.getBot().green.uAtY0);
			panel.Y0VSliderGB.setValue((int) set.getBot().green.vAtY0);
			panel.Y255USliderGB.setValue((int) set.getBot().green.uAtY255);
			panel.Y255VSliderGB.setValue((int) set.getBot().green.vAtY255);
			panel.FuzzyUSliderGB.setValue((int) set.getBot().green.u_fuzzy_range);
			panel.FuzzyVSliderGB.setValue((int) set.getBot().green.v_fuzzy_range);			
			
			
			panel.Y0USpinnerWT.setValue((int) set.getTop().white.uAtY0);
			panel.Y0VSpinnerWT.setValue((int) set.getTop().white.vAtY0);
			panel.Y255USpinnerWT.setValue((int) set.getTop().white.uAtY255);
			panel.Y255VSpinnerWT.setValue((int) set.getTop().white.vAtY255);
			panel.FuzzyUSpinnerWT.setValue((int) set.getTop().white.u_fuzzy_range);
			panel.FuzzyVSpinnerWT.setValue((int) set.getTop().white.v_fuzzy_range);
			panel.Y0USpinnerWB.setValue((int) set.getBot().white.uAtY0);
			panel.Y0VSpinnerWB.setValue((int) set.getBot().white.vAtY0);
			panel.Y255USpinnerWB.setValue((int) set.getBot().white.uAtY255);
			panel.Y255VSpinnerWB.setValue((int) set.getBot().white.vAtY255);
			panel.FuzzyUSpinnerWB.setValue((int) set.getBot().white.u_fuzzy_range);
			panel.FuzzyVSpinnerWB.setValue((int) set.getBot().white.v_fuzzy_range);
			panel.Y0USpinnerGT.setValue((int) set.getTop().green.uAtY0);
			panel.Y0VSpinnerGT.setValue((int) set.getTop().green.vAtY0);
			panel.Y255USpinnerGT.setValue((int) set.getTop().green.uAtY255);
			panel.Y255VSpinnerGT.setValue((int) set.getTop().green.vAtY255);
			panel.FuzzyUSpinnerGT.setValue((int) set.getTop().green.u_fuzzy_range);
			panel.FuzzyVSpinnerGT.setValue((int) set.getTop().green.v_fuzzy_range);
			panel.Y0USpinnerGB.setValue((int) set.getBot().green.uAtY0);
			panel.Y0VSpinnerGB.setValue((int) set.getBot().green.vAtY0);
			panel.Y255USpinnerGB.setValue((int) set.getBot().green.uAtY255);
			panel.Y255VSpinnerGB.setValue((int) set.getBot().green.vAtY255);
			panel.FuzzyUSpinnerGB.setValue((int) set.getBot().green.u_fuzzy_range);
			panel.FuzzyVSpinnerGB.setValue((int) set.getBot().green.v_fuzzy_range);			
			

			
			this.setContentPane(panel);
			
			this.setMinimumSize(new Dimension(600,200));
			
			this.panel.SendButtonGB = new JButton("Send")
			this.panel.SendButtonGB.addActionListener("Send");
			
					
					
					new ActionListener(){

				@Override
				public void actionPerformed(ActionEvent e) {
					//TODO do
					Debug.notRefactored();
				}
				
			});
			
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
