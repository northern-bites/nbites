package nbtool.term;

import static nbtool.util.Logger.INFO;

import java.io.IOException;

import javax.swing.SwingUtilities;
import javax.swing.UIManager;
import javax.swing.UIManager.LookAndFeelInfo;
import javax.swing.UnsupportedLookAndFeelException;

import nbtool.data.SessionMaster;
import nbtool.data.ToolStats;
import nbtool.gui.Display;
import nbtool.io.CrossIO;
import nbtool.util.Center;
import nbtool.util.Logger;
import nbtool.util.NBConstants;
import nbtool.util.Prefs;

public class NBTool_v6 {
	
	public static void main(String[] args) {
		
		if (!Display.class.desiredAssertionStatus()) {
			System.out.println("nbtool should always be run with assertions ON (vm argument -ea)");
			System.out.println("if you want to disable this, you'll have to edit the source code.");
			return;
		}
		
		System.out.printf("\n\tnbtool version %d.%d\n\tdevelopment tool for Bowdoin's Northern Bites team\n\n",
				NBConstants.VERSION, NBConstants.MINOR_VERSION);
		
		System.out.println("Generating Center instance..."); Center.startCenter();
		System.out.println("Generating ToolStats instance: " + ToolStats.INST.toString());
		System.out.println("Generating SessionMaster instance: " + SessionMaster.get().toString());
		System.out.println("Generating CrossServer instance ...");
		CrossIO.startCrossServer();
		
		try {
			Logger.log(Logger.INFO, "loading preferences...");
			Prefs.loadPreferences();
			Logger.level = Prefs.logLevel;
		} catch (ClassNotFoundException e) {
			e.printStackTrace();
			return;
		} catch (IOException e) {
			e.printStackTrace();
			return;
		}
		
		SwingUtilities.invokeLater(new Runnable(){

			@Override
			public void run() {
				Logger.logf(INFO, "Finding best LookAndFeel...");
				LookAndFeelInfo info = null;
				
				for (LookAndFeelInfo installed : UIManager.getInstalledLookAndFeels()) {
					if (installed.getClassName().startsWith("com.apple")) {
						info = installed;
						break;
					}
					
					if ("Nimbus".equals(installed.getName())) {
						info = installed;
					}
				}
				
				Logger.logf(INFO, "Using LookAndFeel %s", info.getClassName());
				try {
					UIManager.setLookAndFeel(info.getClassName());
				} catch (ClassNotFoundException e) {
					e.printStackTrace();
					return;
				} catch (InstantiationException e) {
					e.printStackTrace();
					return;
				} catch (IllegalAccessException e) {
					e.printStackTrace();
					return;
				} catch (UnsupportedLookAndFeelException e) {
					e.printStackTrace();
					return;
				}
				
				Logger.logf(INFO, "Creating Display instance...");
				@SuppressWarnings("unused")
				final Display disp = new Display();
			}
			
		});
		
	}
}
