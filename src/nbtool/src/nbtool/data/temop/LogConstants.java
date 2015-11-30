package nbtool.data.temop;

public class LogConstants {
	
	//Top-level keys
		public static final String LOG_FIRST_ATOM_S = "nblog";
		public static final String LOG_CONTENTS_S = "contents";
		public static final String LOG_CREATED_S = "created";
		public static final String LOG_VERSION_S = "version";
		public static final String LOG_CHECKSUM_S = "checksum";
		public static final String LOG_HOST_TYPE_S = "host_type";
		public static final String LOG_HOST_NAME_S = "host_name";
		public static final String LOG_FROM_ADDR_S = "from_address";
		
		//content item keys
		public static final String CONTENT_TYPE_S = "type";
		public static final String CONTENT_FROM_S = "from";
		public static final String CONTENT_WHEN_S = "when";
		public static final String CONTENT_IINDEX_S = "iindex";
		public static final String CONTENT_NBYTES_S = "nbytes";
		public static final String CONTENT_IMAGE_WIDTH_S = "width";
		public static final String CONTENT_IMAGE_HEIGHT_S = "height";
		public static final String CONTENT_IMAGE_ENCODING_S = "encoding";
		
		//command key
		public static final String COMMAND_FIRST_ATOM_S = "command";
		
		//nbcross wildcard
		public static final String NBCROSS_WILDCARD_TYPE = "__WILDCARD__";
	
	private static final Object jvmLogIndexLock = new Object();
	private static long jvmLogIndex = 0;
	protected static long getJvmLogID() {
		long ret;
		synchronized(jvmLogIndexLock) {
			ret = jvmLogIndex++;
		}
		
		return ret;
	}
}
