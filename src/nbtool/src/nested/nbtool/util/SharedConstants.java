package nbtool.util;

import java.io.File;
import java.lang.reflect.Method;
import java.lang.reflect.Modifier;
import java.nio.file.Files;
import java.nio.file.Path;
import java.util.HashSet;
import java.util.Set;

import nbtool.data.json.Json;
import nbtool.data.json.JsonNumber;
import nbtool.data.json.JsonObject;
import nbtool.data.json.JsonString;
import nbtool.util.test.TestBase;
import nbtool.util.test.Tests;

public class SharedConstants {
		
	private static final String getObjectPath() throws Exception {
		final String suffix = "/src/share/logshare/LOGGING_CONSTANTS.json";
		String nbdir = ToolSettings.NBITES_DIR;
		
		if (nbdir == null) {
			throw new NullPointerException("cannot load SharedConstants object without NBITES_DIR env variable!");
		}
		
		return nbdir + suffix;
	}
	
	private static JsonObject getObject() {
		try {
			Path objectFile = (new File(getObjectPath())).toPath();
			JsonObject obj = Json.parse(new String(Files.readAllBytes(objectFile))).<JsonObject>cast();
			return obj;
		} catch (Exception e) {
			e.printStackTrace();
			System.exit(1);
		}
		
		//never executed.
		return null;
	}
	
	private static JsonObject object = getObject();
	
	public static String stringConstant(String key) {
		return object.get(key).<JsonString>cast().toString();
	}
	
	public static int intConstant(String key) {
		return object.get(key).<JsonNumber>cast().intValue();
	}
	
	public static String YUVImageType_DEFAULT() {
		return object.get("YUVImageType_DEFAULT").asString().toString();
	}
	public static String YUVImageType_Y16() {
		return object.get("YUVImageType_Y16").asString().toString();
	}
	public static String YUVImageType_Y8() {
		return object.get("YUVImageType_Y8").asString().toString();
	}
	
	public static String SexprType_DEFAULT() {
		return object.get("SexprType_DEFAULT").asString().toString();
	}
	public static String JsonType_DEFAULT() {
		return object.get("JsonType_DEFAULT").asString().toString();
	}
	public static String LogType_DEFAULT() {
		return object.get("LogType_DEFAULT").asString().toString();
	}
	
	public static String LogClass_Null() {
		return object.get("LogClass_Null").asString().toString();
	}
	public static String LogClass_Flags() {
		return object.get("LogClass_Flags").asString().toString();
	}
	public static String LogClass_Tripoint() {
		return object.get("LogClass_Tripoint").asString().toString();
	}
	
	public static String LogClass_YUVImage() {
		return object.get("LogClass_YUVImage").asString().toString();
	}
	
	public static String LogClass_String() {
		return object.get("LogClass_String").asString().toString();
	}
	
	public static String LogClass_EdgeImage() {
		return object.get("LogClass_EdgeImage").asString().toString();
	}
	
	public static String LogClass_RPC_Call() {
		return object.get("LogClass_RPC_Call").asString().toString();
	}
	public static String LogClass_RPC_Return() {
		return object.get("LogClass_RPC_Return").asString().toString();
	}
	public static String RPC_NAME() {
		return object.get("RPC_NAME").asString().toString();
	}
	public static String RPC_KEY() {
		return object.get("RPC_KEY").asString().toString();
	}
	
	public static int ROBOT_PORT() {
		return object.get("ROBOT_PORT").<JsonNumber>cast().intValue();
	}
	public static int CROSS_PORT() {
		return object.get("CROSS_PORT").<JsonNumber>cast().intValue();
	}
	
	public static int REMOTE_HOST_TIMEOUT() {
		return object.get("REMOTE_HOST_TIMEOUT").<JsonNumber>cast().intValue();
	}
	
	public static String ROBOT_LOG_PATH_PREFIX() {
		return object.get("ROBOT_LOG_PATH_PREFIX").asString().toString();
	}
	
	public static String ONLINE_CAMERA_OFFSET_PATH() {
		return object.get("ONLINE_CAMERA_OFFSET_PATH").asString().toString();
	}
	
	public static String ONLINE_COLOR_PARAMS_PATH() {
		return object.get("ONLINE_COLOR_PARAMS_PATH").asString().toString();
	}
	
	public static String OFFLINE_CAMERA_OFFSET_SUFFIX() {
		return object.get("OFFLINE_CAMERA_OFFSET_SUFFIX").asString().toString();
	}
	
	public static String OFFLINE_COLOR_PARAMS_SUFFIX() {
		return object.get("OFFLINE_COLOR_PARAMS_SUFFIX").asString().toString();
	}
	
	public static String LOG_TOPLEVEL_MAGIC_KEY() {
		return object.get("LOG_TOPLEVEL_MAGIC_KEY").asString().toString();
	}
	
	public static String LOG_TOPLEVEL_BLOCKS() {
		return object.get("LOG_TOPLEVEL_BLOCKS").asString().toString();
	}
	public static String LOG_TOPLEVEL_LOGCLASS() {
		return object.get("LOG_TOPLEVEL_LOGCLASS").asString().toString();
	}
	public static String LOG_TOPLEVEL_CREATED_WHEN() {
		return object.get("LOG_TOPLEVEL_CREATED_WHEN").asString().toString();
	}
	public static String LOG_TOPLEVEL_HOST_TYPE() {
		return object.get("LOG_TOPLEVEL_HOST_TYPE").asString().toString();
	}
	public static String LOG_TOPLEVEL_HOST_NAME() {
		return object.get("LOG_TOPLEVEL_HOST_NAME").asString().toString();
	}
	public static String LOG_TOPLEVEL_HOST_ADDR() {
		return object.get("LOG_TOPLEVEL_HOST_ADDR").asString().toString();
	}
	
	public static String LOG_BLOCK_TYPE() {
		return object.get("LOG_BLOCK_TYPE").asString().toString();
	}
	public static String LOG_BLOCK_WHERE_FROM() {
		return object.get("LOG_BLOCK_WHERE_FROM").asString().toString();
	}
	public static String LOG_BLOCK_WHEN_MADE() {
		return object.get("LOG_BLOCK_WHEN_MADE").asString().toString();
	}
	public static String LOG_BLOCK_IMAGE_INDEX() {
		return object.get("LOG_BLOCK_IMAGE_INDEX").asString().toString();
	}
	public static String LOG_BLOCK_NUM_BYTES() {
		return object.get("LOG_BLOCK_NUM_BYTES").asString().toString();
	}
	
	public static String LOG_BLOCK_IMAGE_WIDTH_PIXELS() {
		return object.get("LOG_BLOCK_IMAGE_WIDTH_PIXELS").asString().toString();
	}
	public static String LOG_BLOCK_IMAGE_HEIGHT_PIXELS() {
		return object.get("LOG_BLOCK_IMAGE_HEIGHT_PIXELS").asString().toString();
	}
	
	public static void _NBL_ADD_TESTS_() {
		Tests.add("SharedConstants", new TestBase("check all keys"){

			@Override
			public boolean testBody() throws Exception {
				
				final Set<String> exclude = new HashSet<>();
				exclude.add("stringConstant");
				exclude.add("intConstant");
				exclude.add("_NBL_ADD_TESTS_");
				
				for (Method m : SharedConstants.class.getDeclaredMethods()) {
					if (Modifier.isStatic(m.getModifiers()) && Modifier.isPublic(m.getModifiers())
							&& !exclude.contains(m.getName())) {
//						Debug.print("%s", m.getName());
						m.invoke(null);
					}
				}
				 
				return true;
			}
			
		});
	}
	
}
