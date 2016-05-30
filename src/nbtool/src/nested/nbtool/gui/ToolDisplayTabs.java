package nbtool.gui;

import java.awt.Color;
import java.awt.Component;
import java.awt.Font;
import java.lang.reflect.Method;
import java.util.List;

import javax.swing.JLabel;
import javax.swing.SwingUtilities;

import nbtool.data.ViewProfile;
import nbtool.data.group.Group;
import nbtool.data.log.Log;
import nbtool.gui.logviews.misc.GroupView;
import nbtool.gui.logviews.misc.ViewParent;
import nbtool.util.Debug;

public class ToolDisplayTabs {
	private final ToolDisplay display;
	private Log current;
	
	public ToolDisplayTabs(ToolDisplay display) {
		this.display = display;
	}
	
	protected void setContents(Group group) {
		current = null;
		display.displayTabs.removeAll();
		display.displayTabs.add(group.toString(), new GroupView(group));
	}

	protected void setContents(ViewProfile profile, Log first, List<Log> also) {
		Class<? extends ViewParent>[] list = profile.viewsForLog(first);
		display.displayTabs.removeAll();
		
		this.current = first;
		
		for (int i = 0; i < list.length; ++i) {
			Class<? extends ViewParent> ttype = list[i];

			boolean slip = false;
			boolean wrap = false;
			
			try {
				Method m = ttype.getMethod("shouldLoadInParallel");
				slip = (Boolean) m.invoke(null);
				m = ttype.getMethod("shouldShowInScrollPane");
				wrap = (Boolean) m.invoke(null);
			} catch (Exception e) {
				e.printStackTrace();
				Debug.error("error calling method on ViewParent subclass %s: %s",
						ttype.getName(), e.getMessage());
				continue;
			}
			
			if (slip) {
				JLabel waitLabel = new JLabel("Loading...");
				waitLabel.setFont(waitLabel.getFont().deriveFont(Font.BOLD));
				waitLabel.setForeground(Color.BLUE);
				
				display.displayTabs.addTab(ttype.getSimpleName(), waitLabel);
				CreateViewRunnable cvr = new CreateViewRunnable(waitLabel, ttype, first, also);
				Thread thr = new Thread(cvr);
				thr.start();
			} else {
				CreateViewRunnable cvr = new CreateViewRunnable(null, ttype, first, also);
				cvr.run();
			}	
		}
	}
	
	private final class CreateViewRunnable implements Runnable {
		boolean ran = false;
		Component standIn;
		Class<? extends ViewParent> vClass;
		ViewParent created = null;
		Log first;
		List<Log> also;
		
		public CreateViewRunnable(Component standIn, Class<? extends ViewParent> vClass, Log first, List<Log> also) {
			this.standIn = standIn;
			this.vClass = vClass;
			this.first = first;
			this.also = also;
		}
		
		private void create() {
			created = ViewParent.instantiate(vClass);
			created.internal(first, also);
		}
		
		private void add() {
			assert(created != null);
			if (current != first) {
				Debug.warn("CreateViewRunnable found current Log != given Log!");
				return;
			}
			
			int index = display.displayTabs.indexOfComponent(standIn);
			if (index >= 0) {
				display.displayTabs.remove(index);
			} 
			
			display.displayTabs.insertTab(vClass.getSimpleName(), null, created, 
					vClass.getName(), index >= 0 ? index : display.displayTabs.getTabCount());
			created.repaint();
		}
		
		@Override
		public void run() {
			if (!ran) {
				create();
				ran = true;
				SwingUtilities.invokeLater(this);
			} else {
				add();
			}
		}
	}
	
}
