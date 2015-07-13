package nbtool.gui.utilitypanes;

import javax.swing.JFrame;

import nbtool.data.Log;
import nbtool.io.CommonIO.IOFirstResponder;
import nbtool.io.CommonIO.IOInstance;
import nbtool.io.StreamIO;
import nbtool.io.StreamIO.StreamInstance;
import nbtool.util.Events;
import nbtool.util.NBConstants;


public class MultiStreamUtility extends UtilityParent implements IOFirstResponder {

	private JFrame disp = new JFrame();
	
	@Override
	public JFrame supplyDisplay() {
		
		StreamInstance io = StreamIO.create(this, "avril", NBConstants.STREAM_PORT);
		StreamInstance io2 = StreamIO.create(this, "mal", NBConstants.STREAM_PORT);
		StreamInstance io3 = StreamIO.create(this, "zoe", NBConstants.STREAM_PORT);
		StreamInstance io4 = StreamIO.create(this, "simon", NBConstants.STREAM_PORT);
		
		return null;
	}

	@Override
	public String purpose() {
		// TODO Auto-generated method stub
		return null;
	}

	@Override
	public char preferredMemnonic() {
		// TODO Auto-generated method stub
		return 0;
	}

	@Override
	public void ioFinished(IOInstance instance) {		
	}

	@Override
	public void ioReceived(IOInstance inst, int ret, Log... out) {
		Events.GLogsFound.generate(this, out);
	}

	@Override
	public boolean ioMayRespondOnCenterThread(IOInstance inst) {
		// TODO Auto-generated method stub
		return false;
	}
	
}