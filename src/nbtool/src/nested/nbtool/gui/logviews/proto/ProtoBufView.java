package nbtool.gui.logviews.proto;

import java.awt.Dimension;
import java.util.List;
import java.util.Map;

import javax.swing.JScrollPane;
import javax.swing.JTree;
import javax.swing.tree.DefaultMutableTreeNode;

import com.google.protobuf.Descriptors.FieldDescriptor;
import com.google.protobuf.Message;

import nbtool.data.log.Log;
import nbtool.util.Debug;
import nbtool.util.Utility;

public final class ProtoBufView extends nbtool.gui.logviews.misc.ViewParent {
	private static final long serialVersionUID = 1;
	
	@Override
	public void setupDisplay() {
		String t = displayedLog.blocks.get(0).type;
		Class<? extends com.google.protobuf.Message> lClass = Utility.protobufClassFromType(t);
		Debug.info( "ProtoBufView: using class %s for type %s.\n", lClass.getName(), t);
		com.google.protobuf.Message msg = displayedLog.blocks.get(0).parseAsProtobufOfClass(lClass);
		
		Map<FieldDescriptor, Object> fields = msg.getAllFields();
		
		DefaultMutableTreeNode root = new DefaultMutableTreeNode(msg.getClass().getSimpleName());
		for (Map.Entry<FieldDescriptor, Object> entry : fields.entrySet()) {
			int index = 0;
			if (entry.getKey().isRepeated()) {
				List<Object> values = (List<Object>) entry.getValue();
				for (Object v : values) {
					root.add(treeNodeForFD(index++, entry.getKey(), v));
				}
			}
			else root.add(treeNodeForFD(index++, entry.getKey(), entry.getValue()));
		}
		
		tree = new JTree(root);
		
		sp = new JScrollPane(tree);
		sp.setVerticalScrollBarPolicy(JScrollPane.VERTICAL_SCROLLBAR_ALWAYS);
		sp.setHorizontalScrollBarPolicy(JScrollPane.HORIZONTAL_SCROLLBAR_ALWAYS);
		
		add(sp);
		/*
		try {
			messages.ParticleSwarmOuterClass.ParticleSwarm p = messages.ParticleSwarmOuterClass.ParticleSwarm.parseFrom(newlog.bytes);
		} catch (InvalidProtocolBufferException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		} */
	}

	@Override
	public String[] displayableTypes() {
		return new String[]{};
	}
	
	public static Boolean shouldLoadInParallel() {return true;}
	
	public ProtoBufView() {
		super();
		
		sp = null;
		tree = null;
	}

	protected void useSize(Dimension s) {
		if (sp != null && tree != null) {
			sp.setBounds(0, 0, s.width, s.height);
		}
	}
	
	private DefaultMutableTreeNode treeNodeForFD(int index, FieldDescriptor fd, Object val) {
		//Must assume not repeating.
		DefaultMutableTreeNode ret = null;
		
		if (fd.getJavaType() == FieldDescriptor.JavaType.MESSAGE) {
			ret = new DefaultMutableTreeNode(index +":" + fd.getName());
			Message msg = (Message) val;
			
			for (Map.Entry<FieldDescriptor, Object> entry : msg.getAllFields().entrySet()) {
				int i = 0;
				if (entry.getKey().isRepeated()) {
					List<Object> values = (List<Object>) entry.getValue();
					for (Object v : values) {
						ret.add(treeNodeForFD(i++, entry.getKey(), v));
					}
				}
				
				else ret.add(treeNodeForFD(i++, entry.getKey(), entry.getValue()));
			}
			
		} else {
			//leaf
			ret = new DefaultMutableTreeNode(index +":" + fd.getName() + ":" + val.toString());
		}
		
		return ret;
	}
	
	private JScrollPane sp;
	private JTree tree;
}
