package nbtool.gui;

import java.awt.Color;
import java.awt.Component;
import java.awt.Font;
import java.lang.reflect.Method;
import java.util.List;

import javax.swing.JLabel;
import javax.swing.JTabbedPane;
import javax.swing.SwingUtilities;

import nbtool.data.ViewProfile;
import nbtool.data.group.Group;
import nbtool.data.log.Log;
import nbtool.gui.logviews.misc.GroupView;
import nbtool.gui.logviews.misc.ViewParent;
import nbtool.util.Center;
import nbtool.util.Center.NBToolShutdownListener;
import nbtool.util.Debug;

public class ToolDisplayTabs {
	private final ToolDisplay display;
	private Log current;
	
	private static final Debug.DebugSettings debug = Debug.createSettings(Debug.WARN);
	
	public ToolDisplayTabs(final ToolDisplay _display) {
		this.display = _display;
		
		Center.listen(new NBToolShutdownListener(){

			@Override
			public void nbtoolShutdownCallback() {
				warnViews();
			}
			
		});
	}
	
	private void warnViews() {
		for (int i = 0; i < display.displayTabs.getTabCount(); ++i) {
			Component cmp = display.displayTabs.getComponentAt(i);
			if (cmp instanceof ViewParent) {
				((ViewParent) cmp).disappearing();
			}
		}
	}
	
	protected void setContents(Group group) {
		current = null;
		warnViews();
		display.displayTabs.removeAll();
		display.displayTabs.add(group.toString(), new GroupView(group));
	}

	protected void setContents(ViewProfile profile, Log first, List<Log> also) {
		Class<? extends ViewParent>[] list = profile.viewsForLog(first);
//		display.displayTabs.removeAll();
		
		this.current = first;
		warnViews();
		
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
				debug.error("error calling method on ViewParent subclass %s: %s",
						ttype.getName(), e.getMessage());
				continue;
			}
			
			if (slip) {
				JLabel waitLabel = new JLabel("Loading...");
				waitLabel.setFont(waitLabel.getFont().deriveFont(Font.BOLD));
				waitLabel.setForeground(Color.BLUE);
				
//				display.displayTabs.addTab(ttype.getSimpleName(), waitLabel);
				replace(i, ttype.getSimpleName(), ttype.getName(), waitLabel);
				
				CreateViewRunnable cvr = new CreateViewRunnable(i, ttype, first, also);
				Thread thr = new Thread(cvr);
				thr.start();
			} else {
				CreateViewRunnable cvr = new CreateViewRunnable(i, ttype, first, also);
				cvr.run();
			}	
		}
		
		for (int j = list.length; j < display.displayTabs.getTabCount(); ++j) {
			display.displayTabs.remove(j);
		}
	}
	
	private final class CreateViewRunnable implements Runnable {
		boolean ran = false;
		int showIndex;
		Class<? extends ViewParent> vClass;
		ViewParent created = null;
		Log first;
		List<Log> also;
		
		public CreateViewRunnable(int showIndex, Class<? extends ViewParent> vClass, Log first, List<Log> also) {
			this.showIndex = showIndex;
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
				debug.info("CreateViewRunnable found current Log != given Log!");
				return;
			}
			
//			if (index >= 0) {
//				display.displayTabs.remove(index);
//			} 
//			
//			display.displayTabs.insertTab(vClass.getSimpleName(), null, created, 
//					vClass.getName(), index >= 0 ? index : display.displayTabs.getTabCount());
			
			replace(showIndex, vClass.getSimpleName(), vClass.getName(), created);
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
	
	private void replace(int index, String name, String tip, Component cmp) {
		JTabbedPane tabs = display.displayTabs;
		int used;
		
		if (index >= 0 && index < tabs.getTabCount()) {
			debug.info("replacing!");
			tabs.setComponentAt(index, cmp);
			used = index;
		} else {
			debug.info("adding!");
			used = tabs.getTabCount();
			tabs.add(cmp);
		}
		
		tabs.setTitleAt(used, name);
		tabs.setToolTipTextAt(used, tip);
	}
	
}
