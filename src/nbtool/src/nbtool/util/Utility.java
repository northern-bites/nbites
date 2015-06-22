package nbtool.util;

import java.awt.BorderLayout;
import java.awt.image.BufferedImage;
import java.io.File;
import java.lang.reflect.InvocationTargetException;
import java.lang.reflect.Method;
import java.util.HashMap;
import java.util.Map.Entry;
import java.util.MissingResourceException;

import javax.swing.JButton;
import javax.swing.JLabel;
import javax.swing.JPanel;
import javax.swing.JTextField;

import nbtool.data.Log;
import nbtool.data.SExpr;
import nbtool.images.ImageParent;
import nbtool.images.UV88image;
import nbtool.images.Y16image;
import nbtool.images.Y8image;
import nbtool.images.YUYV8888image;


public class Utility {
	
	public static byte[] subArray(byte[] ar, int start, int len) {
		byte[] ret = new byte[len];
		
		System.arraycopy(ar, start, ret, 0, len);
		
		return ret;
	}
	
	//Almost all image logs will have null or [Y8(U8/V8)] encoding, but this method should be extended if that changes.
	public static BufferedImage biFromLog(Log log) {
		assert(log.primaryType().equalsIgnoreCase(NBConstants.IMAGE_S));
		int width = log.primaryWidth();
		int height = log.primaryHeight();
		String encoding = log.primaryEncoding();
		
		ImageParent ip = null;
		if (encoding == null ) {
			//old image
			ip = new YUYV8888image(width / 2, height, log.bytes);
		} else if (encoding.equalsIgnoreCase("[Y8(U8/V8)]")) {
			ip = new YUYV8888image(2*width, height, log.bytes);
		} else if (encoding.equalsIgnoreCase("[Y16]")) {
			ip = new Y16image(width , height, log.bytes);
		} else if (encoding.equalsIgnoreCase("[U8V8]")) {
			ip = new UV88image(width , height, log.bytes);
		} else if (encoding.equalsIgnoreCase("[Y8]")) {
			ip = new Y8image(width , height, log.bytes);
		} else {
			Logger.log(Logger.WARN, "Cannot use image with encoding:" + encoding);
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
	
	public static JPanel fieldWithButton(JTextField f, JButton b) {
		JPanel p = new JPanel(new BorderLayout());
		p.add(b,BorderLayout.EAST);
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
	
	@SuppressWarnings("unchecked")
	public static Class<? extends com.google.protobuf.GeneratedMessage> protobufClassFromType(String _type) {
		assert(_type.startsWith(NBConstants.PROTOBUF_TYPE_PREFIX));
		String type = _type.substring(NBConstants.PROTOBUF_TYPE_PREFIX.length()); //Remove prefix.
		String classname;
		String except;
		
		try {
			except = Prefs.CLASS_EXCEPTIONS_MAP.get(type);
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
	
	
	public static boolean isv6Description(String desc) {
		return (desc != null && desc.trim().startsWith("(nblog"));
	}
	
	/* creates tree for param old out of old._olddesc_ */
	public static boolean v6Convert(Log old) {
		if (old._olddesc_ != null && isv6Description(old._olddesc_)) {
			old.setTree(SExpr.deserializeFrom(old._olddesc_));
			return true;
		}
		if (old._olddesc_ == null) {
			Logger.log(Logger.WARN, "cannot convert old log without string description.");
			return false;
		}	//nothing to work with.
				
		HashMap<String, String> map = new HashMap<String, String>();
		String[] attrs = old._olddesc_.trim().split(" ");
		for (String a : attrs) {
			if (a.trim().isEmpty()) continue;
			
			String[] parts = a.split("=");
			if (parts.length != 2){
				Logger.warnf("conversion found invalid kvp");
				return false;	//Don't attempt to reconstruct malformed descriptions.
			}
			
			String key = parts[0].trim();
			if (key.isEmpty()) {
				Logger.warnf("conversion found empty key");
				return false;
			}
			
			String value = parts[1].trim();
			if (value.isEmpty()) {
				Logger.warnf("conversion found empty value");
				return false;
			}
			
			if (map.containsKey(key)) {
				//we never allowed duplicate keys
				Logger.warnf("conversion found duplicate key");
				return false;
			}
			
			map.put(key, value);
		}
		
		/*
		if (map.containsKey("checksum")) {
			int found_sum = checksum(old.bytes);
			int read_sum = Integer.parseInt(map.get("checksum"));
			if (found_sum != read_sum) {
				Logger.warnf("conversion found wrong checksum");
				return false;
			}
		} */
		
		//Ok, we can convert this.
		map.remove("checksum");
		map.remove("version");
		
		SExpr top_level = SExpr.newList();
		top_level.append(SExpr.newAtom("nblog"));
		top_level.append(SExpr.newList(
				SExpr.newAtom("created"),
				SExpr.newAtom("CONVERTED"),
				SExpr.newAtom("null")
				));
		
		top_level.append(SExpr.newKeyValue("version", 6 + ""));
		
		SExpr c1 = SExpr.newList();
		c1.append(SExpr.newKeyValue("nbytes", old.bytes.length + ""));
		if (map.containsKey("index")) {
			c1.append(SExpr.newKeyValue("iindex", map.get("index")));
			map.remove("index");
		}
		
		for (Entry<String, String> kp : map.entrySet()) {
			c1.append(SExpr.newKeyValue(kp.getKey(), kp.getValue()));
		}
		
		SExpr clist = SExpr.newList();
		clist.append(SExpr.newAtom("contents"));
		clist.append(c1);
		
		top_level.append(clist);
		
		old.setTree(top_level);
		return true;
	}
	
	public static int checksum(byte[] data) {
		int checksum = 0;
		for (byte b : data)
			checksum += (b & 0xFF);
		return checksum;
	}
}
