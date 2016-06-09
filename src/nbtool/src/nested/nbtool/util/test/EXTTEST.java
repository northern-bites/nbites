package nbtool.util.test;

import java.awt.KeyboardFocusManager;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.awt.event.FocusEvent;
import java.awt.event.FocusListener;
import java.awt.event.KeyEvent;
import java.awt.event.KeyListener;
import java.io.IOException;
import java.lang.management.ManagementFactory;
import java.nio.file.FileStore;
import java.nio.file.Files;
import java.nio.file.Path;
import java.nio.file.Paths;
import java.util.Collections;

import javax.swing.JFormattedTextField;
import javax.swing.JLabel;
import javax.swing.JOptionPane;
import javax.swing.JProgressBar;
import javax.swing.JTextField;
import javax.swing.Timer;

import nbtool.data.group.AllGroups;
import nbtool.gui.PathChooser;
import nbtool.util.Debug;
import nbtool.util.ToolSettings;
import nbtool.util.Utility;

public class EXTTEST {
	
	private static FileStore usedFileStore;
	
	private static JLabel progressBar = new JLabel();
	private static int val = 10;
	
	public static void main(String[] args) throws InterruptedException {
		
//		try {
//			usedFileStore = Files.getFileStore(ToolSettings.NBITES_DIR_PATH);
//		} catch(Exception e) {
//			e.printStackTrace();;
//			throw new Error(e);
//		}
//		
//		while(true) {
//			Thread.sleep(4000);
//			Debug.print("loop");
//			footerJvmAction();
//			footerDiskAction();
//		}
		
//		progressBar.setText(Utility.progressString(100, 0.5f));		
//		
//		Utility.display(progressBar);
//		Timer t = new Timer(5000, new ActionListener(){
//
//			@Override
//			public void actionPerformed(ActionEvent e) {
//				val += 10;
//				val %= 100;
//				float v2 = val;
//				progressBar.setText(Utility.progressString(100, v2 / 100));
//			}
//			
//		});
//		t.start();
		
//		JTextField field = new JTextField();
//		field.setFocusTraversalKeys(
//                KeyboardFocusManager.FORWARD_TRAVERSAL_KEYS, Collections.EMPTY_SET);
//        
//		field.addActionListener(new ActionListener(){
//
//			@Override
//			public void actionPerformed(ActionEvent e) {
//				Debug.print("action %s", e);
//			}
//			
//		});
//		field.addKeyListener(new KeyListener(){
//
//			@Override
//			public void keyTyped(KeyEvent e) {
//				Debug.print(" %c", e.getKeyChar());
//			}
//
//			@Override
//			public void keyPressed(KeyEvent e) {
//				// TODO Auto-generated method stub
//				
//			}
//
//			@Override
//			public void keyReleased(KeyEvent e) {
//				// TODO Auto-generated method stub
//				
//			}
//			
//		});
//		
//		field.addFocusListener(new FocusListener(){
//
//			@Override
//			public void focusGained(FocusEvent e) {
//				Debug.print("focus gained");
//			}
//
//			@Override
//			public void focusLost(FocusEvent e) {
//				Debug.print("focus lost");
//			}
//			
//		});
//				
//		Utility.display(field);
		
//		PathChooser.chooseLogPath(null);
//		
//		Path test = Paths.get("/Users/pkoch/Robotics/nbites");
//		Debug.print("%s exists %B", test.toString(), Files.exists(test));
	}
	
	private static void footerJvmAction() {
		long jvmUsed = ManagementFactory.getMemoryMXBean().getHeapMemoryUsage().getUsed();
	    long jvmMax = ManagementFactory.getMemoryMXBean().getHeapMemoryUsage().getMax();
	    
	    int jvmFrac =  (int) (100 * ((double) jvmUsed) / ((double) jvmMax));
//	    display.jvmMemoryBar.setValue(jvmFrac);
	    Debug.print("jvm %d %s", jvmFrac,
	    		String.format("%d logs, %s used memory, %s max", 
	    				AllGroups.getLogCount(), Utility.byteString(jvmUsed, true, true, false, false),
	    				Utility.byteString(jvmMax, true, true, false, false))
	    		);	    
	}
	
	private static void footerDiskAction() {
	    long diskSpace;
	    long diskUsed;
		try {
			diskSpace = usedFileStore.getTotalSpace();
			diskUsed = diskSpace - usedFileStore.getUnallocatedSpace();
		} catch (IOException e) {
			e.printStackTrace();
			throw new Error(e);
		}
	     
	    int diskFrac =  (int) (100 * ((double) diskUsed) / ((double) diskSpace));
//	    display.diskMemoryBar.setValue(diskFrac);   
	    Debug.print("disk %d", diskFrac);
	}

}
