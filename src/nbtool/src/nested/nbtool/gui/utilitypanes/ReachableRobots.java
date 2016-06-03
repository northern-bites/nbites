package nbtool.gui.utilitypanes;

import java.awt.BorderLayout;
import java.awt.Dimension;
import java.awt.List;
import java.awt.event.ComponentAdapter;
import java.awt.event.ComponentEvent;
import java.net.InetAddress;
import java.util.Arrays;
import java.util.HashSet;
import java.util.Set;
import java.util.Vector;

import javax.swing.DefaultListModel;
import javax.swing.JFrame;
import javax.swing.JList;
import javax.swing.SwingUtilities;

import nbtool.util.Debug;
import nbtool.util.Robots;
import nbtool.util.Robots.Robot;

public class ReachableRobots extends UtilityParent {

	private static Display display = null;
	
	@Override
	public JFrame supplyDisplay() {
		if (display != null) {
			return display;
		} else {
			start();
			return (display = new Display());
		}
	}

	@Override
	public String purpose() {
		return "look for reachable robots";
	}

	@Override
	public char preferredMemnonic() {
		return 'r';
	}
	
	private static class Display extends JFrame {
		final Set<String> reachable = new HashSet<>();
		final JList<String> list;
		Display() {
			super("reachable robots");
			list = new JList<String>();
			this.getContentPane().add(list, BorderLayout.CENTER);
			this.setMinimumSize(new Dimension(300,600));
			pack();
			
			this.addComponentListener(new ComponentAdapter(){
				@Override
				public void componentHidden(ComponentEvent e) {
					Debug.print("reachable robots hidden.");
					running = false;
				}
				
				@Override
				public void componentShown(ComponentEvent e) {
					Debug.print("reachable robots shown.");
					running = true;
				}
			});
		}
		
		void doUpdate(String name, boolean up) {
			if (up) {
				reachable.add(name);
			} else {
				reachable.remove(name);
			}
			
			DefaultListModel<String> model = new DefaultListModel<String>();
			for (String n : reachable) model.addElement(n);
			list.setModel(model);
		}
	}
	
	protected static void update(final String name, final boolean up) {
		SwingUtilities.invokeLater(new Runnable(){
			@Override
			public void run() {
				if (display != null) {
					display.doUpdate(name, up);
				}
			}
		});
	}
	
	public static boolean running = false;
	private static String[] addresses = new String[Robots.ROBOT_HOSTNAMES.size()];
	private static Reach[] reachers = new Reach[addresses.length];
	private static Thread[] threads = new Thread[reachers.length];

	public static void start() {
		assert(!running);
		
		addresses = Robots.ROBOT_HOSTNAMES.toArray(new String[0]);
		
		synchronized(reachers) {
			for (int i = 0; i < threads.length; ++i) {
				String addr = addresses[i];
				reachers[i] = new Reach(i, addr);
				
				threads[i] = new Thread(reachers[i]);			
				threads[i].setDaemon(true);
				threads[i].setName("reach-" + addresses[i]);
			}
			
			for (Thread t : threads) {
				t.start();
			}
		}
		
		running = true;
	}
	
	private static class Reach implements Runnable {
		private final int loopTime = 1000;
		int index;
		String addr;
		
		protected Reach(int i, String a) {
			this.index = i; this.addr = a;
		}

		@Override
		public void run() {
			boolean last = false;
			long lastTime = System.currentTimeMillis();
			for(;;) {
				try {
					if (running) {
						boolean reached = false;
						try {
							reached = InetAddress.getByName(addr).isReachable(loopTime);
//							Debug.print("returned %B", reached);
						} catch (Exception e) { /* Debug.error("%s", e.getMessage()); */}
						
						if (reached != last) {
							Debug.print("reach (%B -> %B) now %B for %s",
									last, reached, reached, addr);
							update(addr, reached);
						}
						
						last = reached;
						long elapsed = System.currentTimeMillis() - lastTime;
//						Debug.print("%d elapsed", elapsed);
						if (elapsed < loopTime)
							Thread.sleep(loopTime - elapsed);
						lastTime = System.currentTimeMillis();
					} else {
						Thread.sleep(loopTime * 5);
					}
				} catch (Exception e) {  }
			}
		}
	}
	
	public static void main(String[] args) throws InterruptedException {
//		running = true;
//		new Reach(0, "10.211.55.2").run();
//		new Reach(0, "127.0.0.1").run();
		start();
		for(;;) Thread.sleep(1000);
	}
	
}
