package nbtool.term;

import java.io.IOException;
import java.nio.file.FileSystem;
import java.nio.file.FileSystems;
import java.nio.file.Files;
import java.nio.file.Path;

import javax.swing.SwingUtilities;
import javax.swing.UIManager;
import javax.swing.UIManager.LookAndFeelInfo;
import javax.swing.UnsupportedLookAndFeelException;

import nbtool.data.SessionMaster;
import nbtool.data.ToolStats;
import nbtool.gui.Display;
import nbtool.io.CrossIO;
import nbtool.util.Center;
import nbtool.util.ClassFinder;
import nbtool.util.Debug;
import nbtool.util.ToolSettings;
import nbtool.util.UserSettings;
import nbtool.util.test.Tests;

public class NBTool_v8 {
	
	private static boolean run_tests = false;
	
	public static void main(String[] args) {
		
		if (!Display.class.desiredAssertionStatus()) {
			System.out.println("nbtool should always be run with assertions ON (vm argument -ea)");
			System.out.println("if you want to disable this, you'll have to edit the source code.");
			return;
		}
		
		if (!check_NBITES_DIR()) {
			System.out.println("nbtool requires valid NBITES_DIR environment variable");
			System.out.println("ensure variable exists, and path is readable/writable");
			return;
		}
						
		System.out.printf("\n\tnbtool version %d.%d\n\tdevelopment tool for Bowdoin's Northern Bites team\n\n",
				ToolSettings.VERSION, ToolSettings.MINOR_VERSION);
		
		parse_args(args);
		Debug.lbreak();
		
		if (run_tests) {
			Tests.findAllTests();
			assert(Tests.runAll());
		}
		
		Debug.print("Finding required static initialization methods...");
		ClassFinder.callAllInstancesOfStaticMethod(ToolSettings.staticRequiredStartMethodName);
		Debug.print("Static init done.");
				
		Debug.info("Generating Center instance..."); Center.startCenter();
		System.out.println("Generating ToolStats instance: " + ToolStats.INST.toString());
		System.out.println("Generating SessionMaster instance: " + SessionMaster.get().toString());
		System.out.println("Generating CrossServer instance ...");
		CrossIO.startCrossServer();
		
		try {
			Debug.info("loading preferences...");
			UserSettings.loadPreferences();
			Debug.level = UserSettings.logLevel;
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
				Debug.info( "Finding best LookAndFeel...");
				LookAndFeelInfo info = UIManager.getInstalledLookAndFeels()[0];
				
				for (LookAndFeelInfo installed : UIManager.getInstalledLookAndFeels()) {
					if (installed.getClassName().startsWith("com.apple")) {
						info = installed;
						break;
					}
					
					if ("metal".equalsIgnoreCase(installed.getName())) {
						info = installed;
					}
				}
				
				Debug.info( "Using LookAndFeel %s", info.getClassName());
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
				
				Debug.info( "Creating Display instance...");
				@SuppressWarnings("unused")
				final Display disp = new Display();
			}
			
		});

	}
	
	private static void parse_args(String[] args) {
		for (int i = 0; i < args.length; ++i) {
			if (args[i].equalsIgnoreCase("test")) {
				Debug.plain("  %d: %s ----> %s", 
						i, args[i], "running tests");
				run_tests = true;
			}
		}
	}
	
	private static boolean check_NBITES_DIR() {
		String sp = System.getenv("NBITES_DIR");
		if (sp == null)
			return false;
		Path pathed = FileSystems.getDefault().getPath(sp);
		return Files.isReadable(pathed) && Files.isWritable(pathed) && Files.isDirectory(pathed);
	}
	
}
