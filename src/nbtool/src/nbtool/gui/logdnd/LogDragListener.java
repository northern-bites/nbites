package nbtool.gui.logdnd;
import static nbtool.gui.logdnd.LogDND.*;

import java.awt.dnd.DragGestureEvent;
import java.awt.dnd.DragGestureListener;
import java.awt.dnd.DragSource;

import javax.swing.JComponent;

import nbtool.data.Log;
import nbtool.gui.logdnd.LogDND.LogDNDSource;

public class LogDragListener implements DragGestureListener {
	JComponent eventSource;
	LogDNDSource logSource;
	
	public LogDragListener(JComponent es, LogDNDSource ds) {			
		eventSource = es;
		logSource = ds;
	}
	
	@Override
	public void dragGestureRecognized(DragGestureEvent dge) {
		assert(dge.getComponent() == eventSource);
		Log[] data = logSource.supplyLogsForDrag();
		if (data == null)
			return;
		
		dge.startDrag(DragSource.DefaultCopyDrop, new LogTransferable(data));
	}
	
}

