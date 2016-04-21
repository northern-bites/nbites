package nbtool.data.log;

import java.io.File;

public class LogFile {
	
	public long createdWhen = 0;
	public String logClass = "";
	
	public String host_type = null;
	public String host_name = null;
	public String host_addr = null;
	
	public long savedID = -1;
	
	private File logInFilesystem;
	
	public Log get() {
		return null;
	}
	
	public void update(Log same) {
		if (same.jvm_unique_id != savedID) {
			
		}
	}
	
	public LogFile(File readFrom) {
		
	}
	
	public LogFile(Log makeFrom) {
		
	}
}
