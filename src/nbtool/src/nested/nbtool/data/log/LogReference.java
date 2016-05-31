package nbtool.data.log;

import java.io.BufferedInputStream;
import java.io.BufferedOutputStream;
import java.io.File;
import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.lang.ref.WeakReference;
import java.nio.file.FileSystem;
import java.nio.file.FileSystems;
import java.nio.file.FileVisitResult;
import java.nio.file.Files;
import java.nio.file.Path;
import java.nio.file.SimpleFileVisitor;
import java.nio.file.StandardOpenOption;
import java.nio.file.attribute.BasicFileAttributes;

import nbtool.nio.FileIO;
import nbtool.util.Debug;
import nbtool.util.ToolSettings;
import nbtool.util.Utility;
import nbtool.util.test.TestBase;
import nbtool.util.test.Tests;
import nbtool.util.Debug.DebugSettings;
import nbtool.util.SharedConstants;

import nbtool.data.group.*;

public class LogReference {
	
	private static final DebugSettings debug =
			new DebugSettings(true, true, true, Debug.WARN, null);
	
	private static final Path TEMP_DIR = getTempDir();
	
	private static Path getTempDir() {
		return FileSystems.getDefault().getPath(ToolSettings.NBITES_DIR, ".nbtemp/nbtool/");
	}

	public static void _NBL_REQUIRED_START_() {
		debug.warn("clearing/creating log temp directory...");
		
		try {
			if (Files.exists(TEMP_DIR)) {
				Files.walkFileTree(TEMP_DIR, new SimpleFileVisitor<Path>(){
					@Override
					public FileVisitResult visitFile(Path file,
	                        BasicFileAttributes attrs)
	                          throws IOException {
						debug.info("deleting: %s", file.toString());
						Files.delete(file);
						return FileVisitResult.CONTINUE;
					}
					
					@Override
					public FileVisitResult postVisitDirectory(Path file,
	                        IOException e)
	                          throws IOException {
						if (e != null) {
							debug.error("error deleting temp dir!");
							e.printStackTrace();
							throw e;
						}
						
						debug.info("deleting dir: %s", file.toString());
						Files.delete(file);
						return FileVisitResult.CONTINUE;
					}
				});
			}
			
			assert(!Files.exists(TEMP_DIR));
			Files.createDirectories(TEMP_DIR);
		} catch (IOException e) {
			e.printStackTrace();
			debug.error("LogReference could not set up temporary directory.");
			System.exit(-1);
		}
		
		assert(Files.exists(TEMP_DIR));
		assert(Files.isReadable(TEMP_DIR));
		assert(Files.isWritable(TEMP_DIR));		
	}
	
	
	public long createdWhen = 0;
	public String logClass = "";
	
	public String host_type = null;
	public String host_name = null;
	public String host_addr = null;
	
	public String description = null;
	
	public long savedID = -1;
	public long thisID = Utility.getNextIndex(this);
	
	private WeakReference<Log> theLog = null;
	private Path tempPath = null;
	private Path loadPath = null;
	public Group container = null;
	
	public synchronized Log get() {
		if (theLog != null) {
			Log cur = theLog.get();
			if (cur != null) {
				assert(cur.logReference == this);
				return cur;
			} else {
				theLog = null;
			}
		}
		
		assert(tempPath != null);
		assert(Files.exists(tempPath));
		
		Log ret = null;
		try {
			ret = FileIO.readLogFromPath(tempPath);
		} catch (IOException e) {
			debug.error("LogReference(%d) could not load log from tempPath(%s)!", 
					savedID, tempPath.toString());
			e.printStackTrace();
			System.exit(-1);
		}
		assert(!manages(ret));
		this.savedID = ret.jvm_unique_id;
		updateInternal(ret);
		ret.logReference = this;
		
		theLog = new WeakReference<>(ret);
		return ret;
	}
	
	public synchronized void copyLogToPath(Path newFile) {
		Log refered = get();

		if (Files.isDirectory(newFile)) {
			Path actual = newFile.resolve(tempPath.getFileName());
			FileIO.queueWriteTask(actual, refered.serialize());
		} else {
			FileIO.queueWriteTask(newFile, refered.serialize());
		}	
	}
	
	public synchronized void pushToTempFileNow(Log same) throws IOException {
		assert(manages(same));
		assert(theLog != null && theLog.get() == same);
		
		updateInternal(same);
		FileIO.writeLogToPath(tempPath, same);
	}
	
	public synchronized void pushToLoadFileNow(Log same) throws IOException {
		assert(manages(same));
		assert(theLog != null && theLog.get() == same);
		
		if (loadPath == null) {
			String em = "cannot pushToLoadFile if loadFile == null";
			debug.error(em);
			throw new RuntimeException(em);
		}
		
		pushToTempFileNow(same);
		FileIO.writeLogToPath(loadPath, same);
	}
	
	public synchronized void pushToTempFile(Log same) {
		assert(manages(same));
		assert(theLog != null && theLog.get() == same);
		
		updateInternal(same);
		FileIO.queueWriteTask(tempPath, same.serialize());
	}
	
	public synchronized void pushToLoadFile(Log same) {
		assert(manages(same));
		assert(theLog != null && theLog.get() == same);
		
		if (loadPath == null) {
			String em = "cannot pushToLoadFile if loadFile == null";
			debug.error(em);
			throw new RuntimeException(em);
		}
		
		pushToTempFile(same);
		FileIO.queueWriteTask(loadPath, same.serialize());
	}
	
	public boolean temporary() {
		return loadPath == null;
	}
	
	public boolean manages(Log log) {
		return log.jvm_unique_id == this.savedID;
	}
	
	private LogReference(Log makeFrom) {
		
		this.savedID = makeFrom.jvm_unique_id;
		updateInternal(makeFrom);
		theLog = new WeakReference<>(makeFrom);
	}
	
	private static class InitTask extends FileIO.Task {
		private Log log;
		protected InitTask(Path path, Log log) {
			super(path, null);
			this.log = log;
		}
		
		@Override
		public void executeWrite() throws IOException {
			Files.createFile(path);
			FileIO.writeLogToPath(path, log);
			this.log = null;
		}
	}
	
	private void initializeTemp() {
		String tempName = String.format("temp_log_id%d_u%s.nblog",
				this.savedID, Utility.getRandomHexString(10));
		tempPath = TEMP_DIR.resolve(tempName);
		
		assert(!Files.exists(tempPath));
		Log ourLog = null;
		assert(theLog != null && (ourLog = theLog.get()) != null);
		
		FileIO.addTaskToQueue(new InitTask(tempPath, ourLog));
		
//		try {
//			Files.createFile(tempPath);
//			FileIO.writeLogToPath(tempPath, ourLog);
//									
//		} catch (IOException e) {
//			e.printStackTrace();
//			debug.error("could not generate temp file{%s}: %s", tempPath.toString(), e.getMessage());
//			throw new RuntimeException(String.format("could not generate temp file: %s", e.getMessage()));
//		}
		
//		assert(Files.exists(tempPath));
	}
	
	public static LogReference referenceFromFile(Path readFrom) throws IOException {
		assert(Files.exists(readFrom));
		
		Log read = FileIO.readLogFromPath(readFrom);
		if (read == null) {
			debug.error("could not parse log from %s", readFrom);
			throw new IOException("could not parse Log from " + readFrom);
		}
		
		LogReference ref = new LogReference(read);
		ref.initializeTemp();
		ref.loadPath = readFrom;
		
		read.logReference = ref;
		
		return ref;
	}
	
	public static LogReference referenceFromLog(Log log) {
		assert(log.getReference() == null);
		
		LogReference ref = new LogReference(log);
		ref.initializeTemp();
		ref.loadPath = null;
		
		log.logReference = ref;
		
		return ref;
	}
	
	private synchronized void updateInternal(Log from) {
		assert(manages(from));
		
		this.createdWhen = from.createdWhen;
		this.logClass = from.logClass;
		this.host_type = from.host_type;
		this.host_name = from.host_name;
		this.host_addr = from.host_addr;
		this.description = from.getFullDescription();
	}
	
	@Override
	public String toString() {
		return String.format("LogReference(of:%d, temp@%s, load@%s ())",
				savedID, tempPath, loadPath, temporary() ? "TEMP" : "PERM");
	}
	
	public String guiString() {
		return String.format("log r%d: %s from %s@%s time %d %s", 
				thisID, logClass, host_name, host_addr, createdWhen, temporary() ? "TEMP" : "LOAD"); 
	}
	
	public static void _NBL_ADD_TESTS_() {
		_NBL_REQUIRED_START_();
		
		Tests.add("data.LogReference", new TestBase("basic"){

			@Override
			public boolean testBody() throws Exception {
				Path loadPath = TestBase.resourcePathAtClass(this, "testLog2");
				assert(Files.exists(loadPath));
				
				LogReference ref = FileIO.readRefFromPath(loadPath);
				Log refMng = ref.get();
				assert(refMng != null && refMng.logClass.equals(SharedConstants.LogClass_Tripoint()));
				assert(ref.manages(refMng));
				
				assert(ref.theLog.get() == refMng);
				
				assert(!ref.temporary());
				
				assert(ref.logClass.equals(refMng.logClass));
				
				assert(Files.exists(ref.tempPath));
				assert(Files.exists(ref.loadPath));
				
				String other = "OTHERTYPE";
				refMng.logClass = other;
				assert(ref.get().logClass.equals(other));
				ref.pushToTempFileNow(refMng);
				
				ref.theLog = null;
				
				Log reload = ref.get();
				assert(ref.manages(reload));
				assert(!ref.manages(refMng));
				assert(reload.logClass.equals(other));
				
				return true;
			}
			
		});
	}
}
