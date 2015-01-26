package nbtool.util;

import java.awt.BorderLayout;
import java.awt.datatransfer.DataFlavor;
import java.awt.image.BufferedImage;
import java.io.File;
import java.io.PrintStream;
import java.lang.reflect.InvocationTargetException;
import java.lang.reflect.Method;
import java.util.HashMap;
import java.util.Map;
import java.util.MissingResourceException;
import java.util.ResourceBundle;

import javax.swing.BoxLayout;
import javax.swing.JLabel;
import javax.swing.JPanel;
import javax.swing.JTextField;
import javax.swing.tree.TreePath;

import nbtool.data.Log;
import nbtool.images.ImageParent;
import nbtool.images.UV88image;
import nbtool.images.Y16image;
import nbtool.images.YUYV8888image;


public class U {
	
	/*
	 * Currently synchronized to avoid output interleaving.
	 * 
	 * If the code becomes sluggish, behaves erratically, this is a good place to start looking...
	 * */
	
	/*
	 * Also: synch methods sync on the class.  So while it makes sense for these three to be synchronized, if other static U methods
	 * need to be synchronized it should be with a different locking object.
	 * */
	public static synchronized void w(String s) {
		System.out.println(s);
	}
	
	public static synchronized void wnl(String s) {
		System.out.print(s);
	}
	
	public static synchronized void wf(String f, Object ... a) {
		System.out.printf(f, a);
	}
	
	public static Map<String, String> attributes(String desc) {
		if (desc.trim().isEmpty()) return null;
		HashMap<String, String> map = new HashMap<String, String>();
		
		String[] attrs = desc.trim().split(" ");
		for (String a : attrs) {
			if (a.trim().isEmpty()) continue;
			
			String[] parts = a.split("=");
			if (parts.length != 2)
				return null;	//Don't attempt to reconstruct malformed descriptions.
			
			String type = parts[0].trim();
			if (type.isEmpty())
				return null;	//empty key is an error.  Empty value is NOT error, though parsing it may throw one later.
			
			if (map.containsKey(type)) {
				U.wf("ERROR: description\n\t%s\ncontains multiple key: %s\n", desc, type);
				return null;
			}
			
			map.put(type, parts[1]);
		}
		
		if (map.size() > 0) return map; //If description contains no k/v pairs, explicitly indicate that with null return.
		else return null;
	}
	
	public static byte[] subArray(byte[] ar, int start, int len) {
		byte[] ret = new byte[len];
		
		System.arraycopy(ar, start, ret, 0, len);
		
		return ret;
	}
	
	//Almost all image logs will have null or [Y8(U8/V8)] encoding, but this method should be extended if that changes.
	public static BufferedImage biFromLog(Log log) {
		assert(log.type().equalsIgnoreCase(NBConstants.IMAGE_S));
		int width = log.width();
		int height = log.height();
		String encoding = log.encoding();
		
		ImageParent ip = null;
		if (encoding == null ) {
			//old image
			ip = new YUYV8888image(width / 2, height, log.bytes);
		} else if (encoding.equalsIgnoreCase("[Y8(U8/V8)]")) {
			ip = new YUYV8888image(width , height, log.bytes);
		} else if (encoding.equalsIgnoreCase("[Y16]")) {
			ip = new Y16image(width , height, log.bytes);
		} else if (encoding.equalsIgnoreCase("[U8V8]")) {
			ip = new UV88image(width , height, log.bytes);
		} else {
			U.w("WARNING:  Cannot use image with encoding:" + encoding);
			return null;
		}
		
		return ip.toBufferedImage(); 
	}

	public static final char[] hexArray = "0123456789ABCDEF".toCharArray();
	public static String bytesToHexString(byte[] bytes) {
		char[] hexChars = new char[bytes.length * 2];
		for (int i = 0; i < bytes.length; ++i) {
			int v = bytes[i] & 0xFF;
			hexChars[i * 2] = hexArray[v >>> 4];
			hexChars[i * 2 + 1] = hexArray[v & 0x0F];
		}
		
		return new String(hexChars);
	}
	
	public static JPanel fieldWithlabel(JLabel l, JTextField f) {
		JPanel p = new JPanel(new BorderLayout());
		p.add(l,BorderLayout.WEST);
		p.add(f,BorderLayout.CENTER);
		
		return p;
	}
	
	/*
	 * Can't (to my knowledge) search for classes in the JVM except by trying to find specific ones.
	 * 
	 * So, this function tries a number of class name formats that (at the time of writing) span all used nb messages.
	 * 
	 * This is somewhat hackish, and renaming messages will break this: a better solution would involve rewriting all .proto
	 * files so that protoc does not feel it needs to generate semi-arbitrary class names.
	 * */
	
	
	public static Class<? extends com.google.protobuf.GeneratedMessage> protobufClassFromType(String _type) {
		assert(_type.startsWith(NBConstants.PROTOBUF_TYPE_PREFIX));
		String type = _type.substring(NBConstants.PROTOBUF_TYPE_PREFIX.length()); //Remove prefix.
		String classname;
		String except;
		
		try {
			except = P.CLASS_EXCEPTIONS_MAP.get(type);
		} catch(MissingResourceException mre) {
			except = null;
		}
		
		ClassNotFoundException ocE = null, nocE = null;
		
		/*
		 * Generate the %sOuterClass%s format, using the outer class name exception if found.
		 * */
		if ( except != null )
			classname = String.format("messages.%sOuterClass$%s", except, type);
		else
			classname = String.format("messages.%sOuterClass$%s", type, type);

		Class<? extends com.google.protobuf.GeneratedMessage> retClass = null;
		
		//Try that format.
		try {
			retClass = (Class<? extends com.google.protobuf.GeneratedMessage>) Class.forName(classname);
		} catch (ClassNotFoundException e) {
			ocE  = e;
			retClass = null;
		}
		
		if (retClass != null)
			return retClass;	//OuterClass format found the class.
		
		//Didn't find the class, try class name format WITHOUT 'OuterClass'
		if (except == null) return null; //Can't try this if we didn't find a class name exception.
		classname = String.format("messages.%s$%s", except, type);
		try {
			retClass = (Class<? extends com.google.protobuf.GeneratedMessage>) Class.forName(classname);
		} catch (ClassNotFoundException e) {
			nocE  = e;
			retClass = null;
		}
		
		if (retClass != null)
			return retClass;
		
		//Couldn't find the class, print the errors we found for debugging.
		ocE.printStackTrace();
		nocE.printStackTrace();
		return null;
	}
	
	public static <T extends com.google.protobuf.Message>
		T protobufInstanceForClassWithData(
			Class<T> cls,
			byte[] data) 
			
	{
		T ret = null;
		
		try {
			Method m = cls.getDeclaredMethod("parseFrom", byte[].class);
			ret = (T) m.invoke(null, data);
		} catch (NoSuchMethodException e) {
			e.printStackTrace();
		} catch (SecurityException e) {
			e.printStackTrace();
		} catch (IllegalAccessException e) {
			e.printStackTrace();
		} catch (IllegalArgumentException e) {
			e.printStackTrace();
		} catch (InvocationTargetException e) {
			e.printStackTrace();
		}
		
		return ret;
	}
	
	/*
	 * Path localization assumes unix paths.  It should theoretically work on other filesystems, but this is untested.
	 * */
	public static String localizePath(String p) {
		if (p.startsWith("~" + File.separator)) {
			return System.getProperty("user.home") 
					+ p.substring(1);
		} else return p;
	}
}
