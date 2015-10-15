package nbtool.data.log;

import java.io.File;
import java.io.InputStream;
import java.io.OutputStream;

import nbtool.data.Block;
import nbtool.data.Session;
import nbtool.data.json.JsonObject;

public abstract class Log {
	
	/*
	 * CORE LOG FIELDS AND METHODS
	 * */
	
	public abstract JsonObject attributes();
	
	public abstract Block[] blocks();
	
	public enum SourceType {
		FILE, NETWORK, DERIVED, GENERATED;
	}
	
	public static class Source {
		private String strSource;
		private File fileSource;
		private SourceType type;
		
		protected Source (String s, File f, SourceType st) {
			strSource = s; fileSource = f; type = st;
		}
		
		public static Source network(String addr) {
			return new Source(addr, null, SourceType.NETWORK); 
		}
		
		public static Source filesystem(File file) {
			return new Source(null, file, SourceType.FILE);
		}
		
		public static Source created(String reason, SourceType st) {
			return new Source(reason, null, st);
		}
		
		public String address() {
			return (type == SourceType.NETWORK) ? strSource : null;
		}
		
		public String reason() {
			return (type == SourceType.GENERATED || type == SourceType.DERIVED)
					? strSource : null;
		}
		
		public File file() {
			return (type == SourceType.FILE) ? fileSource : null;
		}
	}
	
	private Source source;
	private Session parent;
	
	public final Source source() { return source; }
	public final Session parent() { return parent; }
	
	public final String filename() {
		if (source.file() != null) {
			return source.file().getName();
		} else {
			return generateFilenameFromAttributes();
		}
	}
	
	private static final Object jvmLogIndexLock = new Object();
	private static long jvmLogIndex = 0;
	private static long getJvmLogID() {
		long ret;
		synchronized(jvmLogIndexLock) {
			ret = jvmLogIndex++;
		}
		
		return ret;
	}
	
	/*
	 * Unique index for every log found in this process, starting from 0.
	 * */
	public final long jvmLogID = getJvmLogID();
	
	private final String generateFilenameFromAttributes() {
		return null;
	}
	
	/*
	 * DERIVED LOG METHODS (FROM ATTRIBUTES)
	 * */
	
	
	
	/*
	 * LOG IO (AND LEGACY PARSING)
	 * */
	
	public static Log readFromStream(InputStream is) {
		return null;
	}
	
	public void writeToStream(OutputStream os) {
		
	}
	
	public void writeToFile(File file) {
		
	}
	
	/*
	 * FACTORY METHODS
	 * */
}
