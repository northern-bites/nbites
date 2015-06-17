package nbtool.io;

import java.io.BufferedInputStream;
import java.io.BufferedOutputStream;
import java.io.DataInputStream;
import java.io.DataOutputStream;
import java.io.File;
import java.io.FileInputStream;
import java.io.FileOutputStream;
import java.io.FilenameFilter;
import java.io.IOException;
import java.util.LinkedList;
import java.util.Queue;

import javax.swing.JFileChooser;

import nbtool.data.Log;
import nbtool.data.Log.SOURCE;
import nbtool.io.CommonIO.GIOFirstResponder;
import nbtool.io.CommonIO.IOFirstResponder;
import nbtool.io.CommonIO.IOState;
import nbtool.util.Events.GFileIOStatus;
import nbtool.util.Logger;
import nbtool.util.Utility;

public class FileIO {
	
	//Should only be accessed from the EDT (since it's a GUI element, this 
	//is logical on several levels.)
	public static final JFileChooser fileChooser = initFileChooser();
	private static JFileChooser initFileChooser() {
		JFileChooser r = new JFileChooser();
		r.setFileSelectionMode(JFileChooser.FILES_ONLY);
		r.setCurrentDirectory(new File(Utility.localizePath("~/")));
		r.setMultiSelectionEnabled(false);
		r.setFileHidingEnabled(true);
		
		return r;
	}
	
	public static final JFileChooser dirChooser = initDirChooser();
	private static JFileChooser initDirChooser() {
		JFileChooser r = new JFileChooser();
		r.setFileSelectionMode(JFileChooser.DIRECTORIES_ONLY);
		r.setCurrentDirectory(new File(Utility.localizePath("~/")));
		r.setMultiSelectionEnabled(false);
		r.setFileHidingEnabled(true);
		
		return r;
	}
		
	public static boolean checkLogFolder(String log_folder) {
		if(log_folder == null || log_folder.isEmpty()) {
			Logger.logf(Logger.ERROR, "path string null or empty!\n");
			return false;
		}
			
		File f = new File(log_folder);
		if (!f.exists()) {
			Logger.logf(Logger.ERROR, "file does not exist!\n");
			return false;
		}
		if (!f.isDirectory()) {
			Logger.logf(Logger.ERROR, "file is not a directory!\n");
			return false;
		}
		if (!f.canRead() || !f.canWrite()) {
			Logger.logf(Logger.ERROR, "permissions errors!\n");
			return false;
		}
		
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
			Logger.logf(Logger.WARN, "WARNING: log [%s] did not follow log format – %d bytes left, CORRUPTION LIKELY\n", logf.getCanonicalPath(),
					dis.available());
			/*
			int av = dis.available();
			byte[] left = new byte[av];
			dis.readFully(left);
			String text = U.bytesToHexString(left);
			
			U.wf("%d bytes left, hex:\n%s\n", av, text); */
		}
		
		//Can't check description on old logs – their tree hasn't been created yet.
		if ((lg._olddesc_ == null) && !(lg.description().equals(full.description()))) {
			Logger.logf(Logger.WARN, "WARNING: log description found to be different upon load:\n\t%s\n\t%s\n",
					lg.description(), full.description());
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
		
		dis.close();
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
				e.printStackTrace();
				return null;
			}
			
			assert(desc != null);
			if (!Utility.isv6Description(desc)) {
				logs[i] = new Log();
				logs[i]._olddesc_ = desc;
				logs[i].name = files[i].getName();
				
				try {
					FileIO.loadLog(logs[i], location);
				} catch (IOException e) {
					e.printStackTrace();
					continue;
				}
				
				assert(Utility.v6Convert(logs[i]));
			} else {
				logs[i] = new Log(desc, null);
			}
			
			logs[i].name = files[i].getName();
			logs[i].source = SOURCE.FILE;
		}
		
		Logger.logf(Logger.INFO, "FileIO: found " + logs.length + " logs in filesystem.");
		
		return logs;
	}
	
	
	/**
	 * ----------------------------------------------------------------------------
	 * Asynchronous writing of logs via a FileIO object.
	 * */
	
	public static FileInstance makeFileWriter(String path, IOFirstResponder ifr) {
		FileInstance fi = new FileInstance();
		fi.path = path;
		fi.ifr = ifr;
		fi.state = IOState.RUNNING;
		
		Thread fithrd = new Thread(fi, String.format("nbtool-%s", fi.name()));
		fithrd.setDaemon(true);
		fithrd.start();
		
		return fi;
	}
	
	public static class FileInstance extends CommonIO.IOInstance {
		
		protected FileInstance() { super(); }
		protected String path;
		
		private final Queue<Log> toWrite = new LinkedList<Log>();
		
		public void add(Log log) {
			synchronized(toWrite) {
				toWrite.add(log);
			}
		}
		
		private Log remove() {
			synchronized(toWrite) {
				if (toWrite.isEmpty())
					return null;
				else return toWrite.remove();
			}
		}

		@Override
		public void run() {
			GFileIOStatus.generate(this, true);
			
			while(this.state == IOState.RUNNING) {
				Log lg  = remove();
				
				if (lg != null) {
					if (lg.name == null) {
						lg.setNameFromDesc();
					} else if (!lg.name.endsWith(".nblog"))
						lg.name = lg.name + ".nblog";
					try {		 
						writeLogToPath(lg, this.path + File.separator + lg.name);
						Logger.log(Logger.EVENT, "FileIO: thread wrote log: " + lg.name);
					} catch (IOException e) {
						Logger.log(Logger.ERROR, "Error writing file to: " + this.path);
						Logger.log(Logger.ERROR, "msg: " + e.getMessage());
						e.printStackTrace();
					}
					
				} else
					try {
						Thread.sleep(200);
					} catch (InterruptedException e) {
						e.printStackTrace();
					}
			}
			
			Logger.logf(Logger.INFO, "%s finishing.", this.name());			
			this.finish();
			GIOFirstResponder.generateFinished(this, ifr);
			GFileIOStatus.generate(this, false);
		}

		@Override
		public String name() {
			return String.format("FileInstance{%s}", path);
		}
		
	}
}
