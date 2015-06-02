package nbtool.gui.logviews.proto;

import java.awt.Dimension;
import java.util.List;
import java.util.Map;

import javax.swing.JScrollPane;
import javax.swing.JTree;
import javax.swing.tree.DefaultMutableTreeNode;

import com.google.protobuf.Descriptors.FieldDescriptor;
import com.google.protobuf.Message;

import nbtool.data.Log;
import nbtool.util.Logger;
import nbtool.util.Utility;

public final class ProtoBallView extends nbtool.gui.logviews.misc.ViewParent {
	private static final long serialVersionUID = -541524730464912737L;

	public static Boolean shouldLoadInParallel() {return true;}
	public void setLog(Log newlog) {
	}

	public ProtoBallView() {
		super();

	}

	protected void useSize(Dimension s) {


	}


}
