package nbtool.data.log;

import java.io.BufferedInputStream;
import java.io.BufferedOutputStream;
import java.io.File;
import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.lang.ref.WeakReference;
import java.nio.file.Files;
import java.nio.file.Path;
import java.nio.file.StandardOpenOption;

import nbtool.util.Debug;
import nbtool.util.Debug.DebugSettings;

public class LogReference {
	
	private static final DebugSettings debug =
			new DebugSettings(true, true, true, Debug.INFO, "LogFile");
	
	private static Path createTempDir() {
		Path test = null;
		try {
			test = Files.createTempDirectory("nbtool-tempLogs");
		} catch (IOException e) {
			e.printStackTrace();
			debug.error("LogFile could not set up temporary directory.");
			System.exit(-1);
		}
		
		assert(Files.exists(test));
		assert(Files.isReadable(test));
		assert(Files.isWritable(test));
		
		return test;
	}
	
	private static final Path TEMP_DIR = createTempDir();
	
	public long createdWhen = 0;
	public String logClass = "";
	
	public String host_type = null;
	public String host_name = null;
	public String host_addr = null;
	
	public String description = null;
	
	public long savedID = -1;
	
	private WeakReference<Log> theLog = null;
	private Path tempPath = null;
	private Path loadPath = null;
		
	public Log get() {
		if (theLog != null) {
			Log cur = theLog.get();
			if (cur != null)
				return cur;
			else {
				theLog = null;
			}
		}
		
		
		
		return null;
	}
	
	public void update(Log same) {
		assert(manages(same));
		
		
	}
	
	public void move(Path newFile) {
		
	}
	
	public void pushTemp(Log same) {
		
	}
	
	public void pushLoad(Log same) {
		
	}
	
	public boolean temporary() {
		return loadPath != null;
	}
	
	public boolean manages(Log log) {
		return log.jvm_unique_id == this.savedID;
	}
	
	public LogReference(Path readFrom) throws IOException {
		assert(Files.exists(readFrom));
		InputStream is = Files.newInputStream(readFrom, StandardOpenOption.READ);
		BufferedInputStream bis = new BufferedInputStream(is);
		Log read = Log.parseFromStream(bis);
		if (read == null) {
			debug.error("could not parse log from %s", readFrom);
			throw new IOException("could not parse Log from " + readFrom);
		}
		
		this.savedID = read.jvm_unique_id;
		updateInternal(read);
		
		loadPath = readFrom;
		String tempName = String.format("nbtool_tempLog_id%d_w%d.nblog",
				read.jvm_unique_id, read.createdWhen);
		tempPath = TEMP_DIR.resolve(tempName);
	}
	
	public LogReference(Log makeFrom) {
		
		this.savedID = makeFrom.jvm_unique_id;
		updateInternal(makeFrom);
				
		String tempName = String.format("nbtool_tempLog_id%d_w%d.nblog",
				makeFrom.jvm_unique_id, makeFrom.createdWhen);
		tempPath = TEMP_DIR.resolve(tempName);
		
		assert(!Files.exists(tempPath));
		
		try {
			Files.createFile(tempPath);
			
			OutputStream os = Files.newOutputStream(tempPath,
					StandardOpenOption.WRITE, StandardOpenOption.TRUNCATE_EXISTING);
			BufferedOutputStream bos = new BufferedOutputStream(os);
			
			makeFrom.writeTo(bos);
			
			bos.close();
			
		} catch (IOException e) {
			e.printStackTrace();
			debug.error("could not generate temp file: %s", e.getMessage());
			throw new RuntimeException(String.format("could not generate temp file: %s", e.getMessage()));
		}
		
		theLog = new WeakReference<>(makeFrom);
	}
	
	private void updateInternal(Log from) {
		assert(manages(from));
		
		this.createdWhen = from.createdWhen;
		this.logClass = from.logClass;
		this.host_type = from.host_type;
		this.host_name = from.host_name;
		this.host_addr = from.host_addr;
		this.description = from.getFullDescription();
	}
}
