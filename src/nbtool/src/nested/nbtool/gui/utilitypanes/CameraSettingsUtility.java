package nbtool.gui.utilitypanes;
import javax.swing.JFrame;


public class CameraSettingsUtility extends UtilityParent {
	
	private CSU_Frame display = null;

	@Override
	public JFrame supplyDisplay() {
		 if (display == null) {
	            return (display = new CSU_Frame());
	        }
		 
	        return display;
	}

	@Override
	public String purpose() {
		return "configure robot camera settings";
	}

	@Override
	public char preferredMemnonic() {
		return 's';
	}

	private class CSU_Frame extends JFrame {
		
		private final CameraStreamPanel panel = new CameraStreamPanel();
		
		public CSU_Frame() {
			super();
			setSize(400,800);
			this.setContentPane(panel);
		}
	}
}
