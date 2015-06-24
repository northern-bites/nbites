package nbtool.gui.logdnd;

import java.awt.datatransfer.Clipboard;
import java.awt.datatransfer.ClipboardOwner;
import java.awt.datatransfer.DataFlavor;
import java.awt.datatransfer.Transferable;
import java.awt.datatransfer.UnsupportedFlavorException;
import java.awt.dnd.DnDConstants;
import java.awt.dnd.DragGestureEvent;
import java.awt.dnd.DragGestureListener;
import java.awt.dnd.DragSource;
import java.awt.dnd.DropTarget;
import java.awt.dnd.DropTargetAdapter;
import java.awt.dnd.DropTargetDropEvent;
import java.io.IOException;
import java.util.HashSet;
import java.util.Set;

import javax.swing.JComponent;
import javax.swing.JTree;
import javax.swing.TransferHandler;

import nbtool.data.Log;
import nbtool.util.Logger;

public class LogDND {
	//Uses the Log class but represents Log[]
	public static final DataFlavor LOG_DATA_FLAVOR = new DataFlavor(LogTransferable.class,
			LogTransferable.class.getName());
	public static final DragSource LOG_DRAG_SOURCE = DragSource.getDefaultDragSource();
	
	public static interface LogDNDSource {
		public Log[] supplyLogsForDrag();
	}
	
	public static interface LogDNDTarget {
		public void takeLogsFromDrop(Log log[]);
	}
	
	public static void makeComponentSource(JComponent comp, LogDNDSource source) {
		LOG_DRAG_SOURCE.createDefaultDragGestureRecognizer(comp, DnDConstants.ACTION_COPY_OR_MOVE,
				new LogDragListener(comp, source));
	}
	
	public static void makeComponentTarget(JComponent comp, LogDNDTarget dest) {
		DropTarget dt = new DropTarget(comp, DnDConstants.ACTION_COPY_OR_MOVE, new LogDropListener(dest), true);
		comp.setDropTarget(dt);
	}

	
	
	/*
	public static class LogTransferHandler extends TransferHandler {
		JComponent eventSource;
		LogDNDSource logSource;
		
		public LogTransferHandler(JComponent es, LogDNDSource ds) {
			super();
			
			eventSource = es;
			logSource = ds;
		}
		
		@Override
		public int getSourceActions(JComponent c) {
		    return TransferHandler.COPY_OR_MOVE;
		}
		
		@Override
		public Transferable createTransferable(JComponent c) {
			if (c != eventSource)
				return null;
			Log[] data = logSource.supplyLogsForDrag();
			if (data == null)
				return null;
			return new LogTransferable(data);
		}
	} */
}
