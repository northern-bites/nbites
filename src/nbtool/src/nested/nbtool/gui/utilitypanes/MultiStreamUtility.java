//package nbtool.gui.utilitypanes;
//
//import javax.swing.JFrame;
//
//import nbtool.data._log._Log;
//import nbtool.io.CommonIO.IOFirstResponder;
//import nbtool.io.CommonIO.IOInstance;
//import nbtool.io.StreamIO;
//import nbtool.io.StreamIO.StreamInstance;
//import nbtool.util.Events;
//import nbtool.util.ToolSettings;
//
//
//public class MultiStreamUtility extends UtilityParent implements IOFirstResponder {
//
//	private JFrame disp = new JFrame();
//	
//	@Override
//	public JFrame supplyDisplay() {
//		
//		StreamInstance io = StreamIO.create(this, "avril", ToolSettings.STREAM_PORT);
//		StreamInstance io2 = StreamIO.create(this, "mal", ToolSettings.STREAM_PORT);
//		StreamInstance io3 = StreamIO.create(this, "zoe", ToolSettings.STREAM_PORT);
//		StreamInstance io4 = StreamIO.create(this, "simon", ToolSettings.STREAM_PORT);
//		
//		return null;
//	}
//
//	@Override
//	public String purpose() {
//		// TODO Auto-generated method stub
//		return null;
//	}
//
//	@Override
//	public char preferredMemnonic() {
//		// TODO Auto-generated method stub
//		return 0;
//	}
//
//	@Override
//	public void ioFinished(IOInstance instance) {		
//	}
//
//	@Override
//	public void ioReceived(IOInstance inst, int ret, _Log... out) {
//		Events.GLogsFound.generate(this, out);
//	}
//
//	@Override
//	public boolean ioMayRespondOnCenterThread(IOInstance inst) {
//		// TODO Auto-generated method stub
//		return false;
//	}
//	
//}