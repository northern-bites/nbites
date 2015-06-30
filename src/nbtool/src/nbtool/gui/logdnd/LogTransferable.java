package nbtool.gui.logdnd;

import java.awt.datatransfer.Clipboard;
import java.awt.datatransfer.ClipboardOwner;
import java.awt.datatransfer.DataFlavor;
import java.awt.datatransfer.Transferable;
import java.awt.datatransfer.UnsupportedFlavorException;
import java.io.IOException;

import nbtool.data.Log;
import static nbtool.gui.logdnd.LogDND.*;

public class LogTransferable implements Transferable, ClipboardOwner {
	private Log[] data;
	public LogTransferable(Log[] data) {
		this.data = data;
	}

	@Override
	public DataFlavor[] getTransferDataFlavors() {
		return new DataFlavor[]{LOG_DATA_FLAVOR};
	}

	@Override
	public boolean isDataFlavorSupported(DataFlavor flavor) {
		return flavor.equals(LOG_DATA_FLAVOR);
	}

	@Override
	public Object getTransferData(DataFlavor flavor)
			throws UnsupportedFlavorException, IOException {
		if (flavor.equals(LOG_DATA_FLAVOR)) {
			return data;
		} else {
			throw new UnsupportedFlavorException(flavor);
		}
	}

	@Override
	public void lostOwnership(Clipboard clipboard, Transferable contents) {}
}