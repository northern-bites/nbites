package nbtool.gui.utilitypanes;

import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.nio.ByteBuffer;
import java.nio.ByteOrder;

import javax.swing.JButton;
import javax.swing.JFrame;

import nbtool.data.SExpr;
import nbtool.gui.logviews.sound2.Target;
import nbtool.util.Events;
import nbtool.util.Debug;

public class SyntheticSndUtility extends UtilityParent {
	
	int nframes = 4096;

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
		public Snd_Frame() {
			super("snd maker!");
			
			JButton b = new JButton("new");
			b.addActionListener(this);
			this.add(b);
		}

		@Override
		public void actionPerformed(ActionEvent e) {
			Debug.plain("generating...");
            
			byte[] internal = new byte[2 * 2 * nframes];
			ByteBuffer bb = ByteBuffer.wrap(internal);

			bb.order(ByteOrder.LITTLE_ENDIAN);
			Target t0 = new Target(8, Short.MAX_VALUE, 0.0);
			Target t1 = new Target(8, Short.MAX_VALUE, 1.0);
			for (int i = 0; i < nframes; ++i) {
				bb.putShort((short) t0.sin(i));
				bb.putShort((short) t1.sin(i));
			}
			
//			_Log newSound = _Log.logWithTypePlus("sound", bb.array(), 
//					SExpr.pair("channels", 2),
//					SExpr.pair("frames", nframes),
//					SExpr.pair("rate", 16000)
//					);
//			generated.addLog(newSound, true);
//			Events.GLogsFound.generate(this, newSound);
			Debug.notRefactored();
		}
	}
}
