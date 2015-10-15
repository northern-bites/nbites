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

import nbtool.data.SExpr;
import nbtool.data.log._Log;
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
	public static BufferedImage biFromLog(_Log log) {
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
	
	//binary, not SI
	public static final long bytesPerKB = (1 << 10);
	public static final long bytesPerMB = (1 << 20);
	public static final long bytesPerGB = (1 << 30);
	
	public static String byteString( long bytes, boolean sepGig, boolean sepMeg, boolean sepKyb, boolean showByte ) {
		String total = "";
		long left = bytes;
		
		if (sepGig) {
			long gigs = left / bytesPerGB;
			if (gigs > 0) {
				total = total + String.format("%dGB ", gigs);
				left = left % bytesPerGB;
			}
		}
		
		if (sepMeg) {
			long megs = left / bytesPerMB;
			if (megs > 0) {
				total = total + String.format("%dMB ", megs);
				left = left % bytesPerMB;
			}
		}
		
		if (sepKyb) {
			long kbs = left / bytesPerKB;
			if (kbs > 0) {
				total = total + String.format("%dKB ", kbs);
				left = left % bytesPerKB;
			}
		}
		
		if (showByte && left > 0) {
			total = total + String.format("%d bytes", left);
		}
		
		return total;
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
	 * assuming the .proto files have the necessary java_options, 
	 * this function can get the java class of an protobuf from its name
	 * (the type as used in c++)
	 * */
	
	@SuppressWarnings("unchecked")
	public static Class<? extends com.google.protobuf.GeneratedMessage> protobufClassFromType(String _type) {
		assert(_type.startsWith(NBConstants.PROTOBUF_TYPE_PREFIX));
		String type = _type.substring(NBConstants.PROTOBUF_TYPE_PREFIX.length()); //Remove prefix.
		String classname = String.format("messages.%s", type);

		try {
			return (Class<? extends com.google.protobuf.GeneratedMessage>) Class.forName(classname);
		} catch (ClassNotFoundException e) {
			e.printStackTrace();
			return null;
		}
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
	public static boolean v6Convert(_Log old) {
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
	
	public static void main(String[] args) {
		Logger.println(Utility.byteString(12497681235l, true, true, true, true));
	}
}
