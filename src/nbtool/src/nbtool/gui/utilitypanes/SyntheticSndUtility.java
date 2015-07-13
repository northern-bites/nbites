package nbtool.gui.utilitypanes;

import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.nio.ByteBuffer;
import java.nio.ByteOrder;

import javax.swing.JButton;
import javax.swing.JFrame;

import nbtool.data.Log;
import nbtool.data.SExpr;
import nbtool.data.Session;
import nbtool.data.SessionMaster;
import nbtool.gui.logviews.sound2.ShortBuffer;
import nbtool.util.Events;
import nbtool.util.Logger;

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
	
	private class Snd_Frame extends JFrame implements ActionListener {
		private Session generated = null;
		public Snd_Frame() {
			super("snd maker!");
			generated = SessionMaster.get().requestSession("synth-sound");
			
			JButton b = new JButton("new");
			b.addActionListener(this);
			this.add(b);
		}

		@Override
		public void actionPerformed(ActionEvent e) {
			Logger.println("generating...");
			ByteBuffer bb = ByteBuffer.allocate(ShortBuffer.SHORT_BYTES * 2 * nframes);
			bb.order(ByteOrder.LITTLE_ENDIAN);
			Signal s = new Signal(Short.MAX_VALUE, 0, 1024);
			for (int i = 0; i < nframes; ++i) {
				Logger.printf("%d for %d", s.value(i), i);
				//bb.putShort(s.value(i));
				//bb.putShort(s.value(i));
				
				bb.putShort( (short) (i * 100));
				bb.putShort( (short) (i * 10));
			}
			
			Log newSound = Log.logWithTypePlus("sound", bb.array(), SExpr.pair("channels", 2), SExpr.pair("frames", nframes));
			generated.addLog(newSound, true);
			Events.GLogsFound.generate(this, newSound);
		}
	}

	private class Signal {
		int amplitude;
		int offset;
		int wavelength;
		
		Signal(int a, int o, int w) {
			this.amplitude = a;
			this.offset = o;
			this.wavelength = w;
		}
		
		short value(int t) {
			double rad_o = ((double) offset / wavelength)  * 2 * Math.PI;
			double rad_p = ((double) t	    / wavelength)  * 2 * Math.PI;
			return (short) (Math.sin(rad_o + rad_p) * amplitude);
		}
	}
}
