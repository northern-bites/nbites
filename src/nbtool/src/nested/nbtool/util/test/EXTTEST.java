package nbtool.util.test;

import java.io.IOException;
import java.lang.management.ManagementFactory;
import java.nio.file.FileStore;

import javax.swing.JLabel;

import nbtool.data.group.AllGroups;
import nbtool.util.Debug;
import nbtool.util.Utility;

public class EXTTEST {

	private static FileStore usedFileStore;

	private static JLabel progressBar = new JLabel();
	private static int val = 10;

	public static void main(String[] args) throws InterruptedException {

		double[] floats = new double[]{ -0.214802, 0.35, 1.57538, 0.131882, -1.56165, -0.0229681, -0.0475121, -0.0137641,
                -0.811444, 2.16443, -1.22111, 0.00771189,  0.0261199, -0.81613, 2.17986, -1.23023,
                -0.0352399, 1.58466, -0.046062, 1.5631, 0.0353239};
		Debug.print("%d\n", floats.length);

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

	    int jvmFrac =  (int) (100 * ((double) jvmUsed) / (jvmMax));
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

	    int diskFrac =  (int) (100 * ((double) diskUsed) / (diskSpace));
//	    display.diskMemoryBar.setValue(diskFrac);
	    Debug.print("disk %d", diskFrac);
	}

}
