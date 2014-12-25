package nbclient.io;

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

import javax.swing.JFileChooser;

import nbclient.data.OpaqueLog;
import nbclient.data.OpaqueLog.SOURCE;
import nbclient.util.U;

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
		if(!(log_folder != null && !log_folder.isEmpty()))
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
	
	public static String readDescription(File f) throws IOException {
		assert(f.exists());
		assert(f.getName().endsWith(".nblog"));
		
		FileInputStream fis = new FileInputStream(f);
		BufferedInputStream bis = new BufferedInputStream(fis);
		DataInputStream dis = new DataInputStream(bis);
		
		int dlen = dis.readInt();
		byte[] bytes = new byte[dlen];
		dis.readFully(bytes);
		
		String r = new String (bytes);
		
		dis.close();bis.close();fis.close();
		return r;
	}
	
	public static void loadLog(OpaqueLog lg, String log_folder) throws IOException {
		assert(checkLogFolder(log_folder));
		assert(lg!=null);
		assert(lg.name != null);
		
		File logf = new File(log_folder.concat(lg.name));
		assert(logf.exists());
		long flen = logf.length();
		
		FileInputStream fis = new FileInputStream(logf);
		BufferedInputStream bis = new BufferedInputStream(fis);
		DataInputStream dis = new DataInputStream(bis);
		
		int dlen = dis.readInt();
		dis.skip(dlen);
		lg.bytes = new byte[(int)flen - dlen - 4]; //Extra 4 for the desc len int.
		
		dis.readFully(lg.bytes);
		
		dis.close();
		bis.close();
		fis.close();
	}
	
	public static void writeLog(OpaqueLog lg, String log_folder) throws IOException {
		assert(checkLogFolder(log_folder)); 
		assert(lg!=null);
		assert(lg.name != null);
		assert(lg.bytes != null);
		
		File logf;
		if (!lg.name.endsWith(".nblog"))
			logf = new File(log_folder.concat(lg.name) + ".nblog");
		else logf = new File(log_folder.concat(lg.name));
		
		U.w("wl: " + logf.getAbsolutePath());
		
		if(!logf.exists())
			logf.createNewFile();

		_wlINTERNAL(logf, lg);
	}
	
	public static void writeLogToPath(OpaqueLog lg, String path) throws IOException {
		assert(path.endsWith(".nblog"));
		assert(lg.bytes != null);
		
		File logf = new File(path);
		if (!logf.exists())
			logf.createNewFile();
		
		_wlINTERNAL(logf, lg);
	}
	
	public static void _wlINTERNAL(File logf, OpaqueLog lg) throws IOException {
		FileOutputStream fos = new FileOutputStream(logf);
		BufferedOutputStream bos = new BufferedOutputStream(fos);
		DataOutputStream dos = new DataOutputStream(bos);
		
		byte[] dbytes = lg.description.getBytes(StandardCharsets.UTF_8);
		dos.writeInt(dbytes.length + 1);
		dos.write(dbytes);
		dos.writeByte(0);
		dos.write(lg.bytes);
		
		dos.close();
		bos.close();
		fos.close();
	}
	
	public static OpaqueLog[] fetchLogs(String location) {
		assert(checkLogFolder(location));
		File logd = new File(location);
		File[] files = logd.listFiles(new FilenameFilter(){
			public boolean accept(File dir, String name) {
				return name.endsWith(".nblog");
			}
		});
		
		OpaqueLog[] logs = new OpaqueLog[files.length];
		
		for (int i = 0; i < files.length; ++i) {
			String desc = null;
			try {
				desc = readDescription(files[i]);
			} catch (IOException e) {
				// TODO Auto-generated catch block
				e.printStackTrace();
				return null;
			}
			
			logs[i] = new OpaqueLog(desc, null);
			logs[i].name = files[i].getName();
			logs[i].source = SOURCE.FILE;
		}
		
		U.w("FileIO: found " + logs.length + " logs in filesystem.");
		
		return logs;
	}
	
	public static ArrayList<OpaqueLog> logsToWrite;
	public static synchronized void addObject(OpaqueLog lg) {
		if (logsToWrite != null) logsToWrite.add(lg);
	}
	
	public static synchronized OpaqueLog getObject() {
		if (logsToWrite != null && logsToWrite.size() > 0) 
			return (OpaqueLog) logsToWrite.remove(0);
		else return null;
	}

	public String log_folder;
	public volatile boolean running;
	
	public FileIO(Boss b, String folder) {
		boss = b; log_folder = folder;
	}
	
	private Boss boss;
	public void run() {		
		logsToWrite = new ArrayList<OpaqueLog>();
		
		while(true) {
			if (!running) {
				U.w("FileIO thread finishing because !running.");
				boss.fileioThreadExiting();
				return;
			}
			
			OpaqueLog lg = null;
			if ((lg = getObject()) != null) {
				if (lg.name == null) {
					lg.setNameFromDesc();
				} else if (!lg.name.endsWith(".nblog"))
					lg.name = lg.name + ".nblog";
				
				try {		 
					writeLog(lg, log_folder);
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
	}
	
	public interface Boss {
		public void fileioThreadExiting();
	}
}
