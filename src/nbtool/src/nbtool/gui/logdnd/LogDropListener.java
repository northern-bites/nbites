package nbtool.gui.logdnd;
import static nbtool.gui.logdnd.LogDND.*;

import java.awt.datatransfer.Transferable;
import java.awt.dnd.DropTargetAdapter;
import java.awt.dnd.DropTargetDropEvent;

import nbtool.data.Log;
import nbtool.gui.logdnd.LogDND.LogDNDTarget;
import nbtool.util.Logger;

public class LogDropListener extends DropTargetAdapter {
	LogDNDTarget source;
	public LogDropListener(LogDNDTarget source) {
		this.source = source;
	}

	@Override
	public void drop(DropTargetDropEvent dtde) {
		try {
			Transferable tr = dtde.getTransferable();

			if (tr.isDataFlavorSupported(LOG_DATA_FLAVOR)) {
				Log[] data = (Log[]) tr.getTransferData(LOG_DATA_FLAVOR);
				source.takeLogsFromDrop(data);
				dtde.dropComplete(true);
			} else {
				dtde.rejectDrop();
			}
		} catch (Exception e) {
			Logger.errorf("COULD NOT ACCEPT DROP %s", dtde);
			e.printStackTrace();
			dtde.rejectDrop();
		}
	}
	
}