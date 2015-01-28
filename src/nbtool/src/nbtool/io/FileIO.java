package nbtool.io;

import java.io.BufferedInputStream;
import java.io.BufferedOutputStream;
import java.io.DataInputStream;
import java.io.DataOutputStream;
import java.io.File;
import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.FilenameFilter;
import java.io.IOException;
import java.nio.charset.StandardCharsets;
import java.util.ArrayList;
import java.util.LinkedList;
import java.util.Queue;

import javax.swing.JFileChooser;

import nbtool.data.Log;
import nbtool.data.Log.SOURCE;
import nbtool.util.N;
import nbtool.util.N.EVENT;
import nbtool.util.U;

public class FileIO implements Runnable {
	
	//Should only be accessed from the EDT (since it's a GUI element, this 
	//is logical on several levels.)
	public static final JFileChooser chooser = initChooser();
	private static JFileChooser initChooser() {
		JFileChooser r = new JFileChooser();
		r.setFileSelectionMode(JFileChooser.FILES_ONLY);
		r.setCurrentDirectory(new File(U.localizePath("~/")));
		r.setMultiSelectionEnabled(false);
		r.setFileHidingEnabled(false);
		
		return r;
	}
		
	public static boolean checkLogFolder(String log_folder) {
		if(log_folder == null || log_folder.isEmpty())
			return false;
			
		File f = new File(log_folder);
		if (!f.exists())
			return false;
		if (!f.isDirectory())
			return false;
		if (!f.canRead() || !f.canWrite())
			return false;
		
		return true;
	}
	
	public static void loadLog(Log lg, String log_folder) throws IOException {
		assert(checkLogFolder(log_folder));
		assert(lg!=null);
		assert(lg.name != null);
		
		File logf = new File(log_folder.concat(lg.name));
		assert(logf.exists());
		
		FileInputStream fis = new FileInputStream(logf);
		BufferedInputStream bis = new BufferedInputStream(fis);
		DataInputStream dis = new DataInputStream(bis);
		
		Log full = CommonIO.readLog(dis);
		
		if (dis.available() != 0) {
			U.wf("ERROR: log [%s] did not follow log format, CORRUPTION LIKELY\n");
		}
		
		if (!(lg.description.equals(full.description))) {
			U.wf("WARNING: log description found to be different upon load:\n\t%s\n\t%s\n",
					lg.description, full.description);
		}
		
		lg.bytes = full.bytes;
		
		dis.close();
	}
	
	public static void writeLogToPath(Log lg, String path) throws IOException {
		assert(path.endsWith(".nblog"));
		assert(lg.bytes != null);
		
		File logf = new File(path);
		if (!logf.exists())
			logf.createNewFile();
		
		FileOutputStream fos = new FileOutputStream(logf);
		BufferedOutputStream bos = new BufferedOutputStream(fos);
		DataOutputStream dos = new DataOutputStream(bos);
		
		CommonIO.writeLog(dos, lg);
		
		dos.close();
	}
	
	public static String readDescriptionFromFile(File f) throws IOException {
		assert(f.exists());
		assert(f.getName().endsWith(".nblog"));
		
		FileInputStream fis = new FileInputStream(f);
		BufferedInputStream bis = new BufferedInputStream(fis);
		DataInputStream dis = new DataInputStream(bis);
		
		String r = CommonIO.readLogDescription(dis);
		
		dis.close();bis.close();fis.close();
		return r;
	}
	
	public static Log[] fetchLogs(String location) {
		assert(checkLogFolder(location));
		
		File logd = new File(location);
		File[] files = logd.listFiles(new FilenameFilter(){
			public boolean accept(File dir, String name) {
				return name.endsWith(".nblog");
			}
		});
		
		Log[] logs = new Log[files.length];
		
		for (int i = 0; i < files.length; ++i) {
			String desc = null;
			try {
				desc = readDescriptionFromFile(files[i]);
			} catch (IOException e) {
				// TODO Auto-generated catch block
				e.printStackTrace();
				return null;
			}
			
			logs[i] = new Log(desc, null);
			logs[i].name = files[i].getName();
			logs[i].source = SOURCE.FILE;
		}
		
		U.w("FileIO: found " + logs.length + " logs in filesystem.");
		
		return logs;
	}
	
	
	/**
	 * ----------------------------------------------------------------------------
	 * Asynchronous writing of logs via a FileIO object.
	 * */
	
	public static Queue<Log> logsToWrite = null;
	
	public static synchronized void addObject(Log lg) {
		if (logsToWrite != null) logsToWrite.add(lg);
	}
	
	public static synchronized Log getObject() {
		if (logsToWrite != null && logsToWrite.size() > 0) 
			return logsToWrite.remove();
		else return null;
	}

	private String log_folder;
	private volatile boolean running;
	private Boss boss;
	
	public FileIO(Boss b, String folder) {
		boss = b; log_folder = folder;
		running = false;
	}
	
	public void stop() {
		running = false;
	}
	
	public void run() {		
		running = true;
		logsToWrite = new LinkedList<Log>();
		N.notifyEDT(EVENT.FIO_THREAD, this, true);
		
		while(running) {
			Log lg  = getObject();
			if (lg != null) {
				if (lg.name == null) {
					lg.setNameFromDesc();
				} else if (!lg.name.endsWith(".nblog"))
					lg.name = lg.name + ".nblog";
				
				try {		 
					writeLogToPath(lg, log_folder + File.separator + lg.name);
					U.w("FileIO: thread wrote log: " + lg.name);
				} catch (IOException e) {
					U.w("Error writing file to: " + log_folder);
					U.w("msg: " + e.getMessage());
					e.printStackTrace();
				}
				
			} else
				try {
					Thread.sleep(500);
				} catch (InterruptedException e) {
					e.printStackTrace();
				}
		}
		
		logsToWrite = null;
		U.w("FileIO thread finishing because !running.");
		N.notifyEDT(EVENT.FIO_THREAD, this, false);
		boss.fileioThreadExiting();
	}
	
	public interface Boss {
		public void fileioThreadExiting();
	}
}
