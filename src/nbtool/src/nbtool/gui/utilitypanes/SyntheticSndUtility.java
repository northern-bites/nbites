package nbtool.gui.utilitypanes;

import javax.swing.JFrame;

public class SyntheticSndUtility extends UtilityParent {
	
	int nframes = 1024;

	private Snd_Frame display = null;
	@Override
	public JFrame supplyDisplay() {
		if (display != null)
			return display;
		return (display = new Snd_Frame());
	}

	@Override
	public String purpose() {
		return "generating sound of specific frequencies";
	}

	@Override
	public char preferredMemnonic() {
		return 'p';
	}
	
	private class Snd_Frame extends JFrame {
		public Snd_Frame() {
			super("snd maker!");
		}
	}

}
