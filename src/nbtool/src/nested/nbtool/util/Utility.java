package nbtool.util;

import java.awt.BorderLayout;
import java.awt.Component;
import java.io.File;
import java.lang.reflect.InvocationTargetException;
import java.lang.reflect.Method;
import java.util.Arrays;
import java.util.HashMap;
import java.util.Map;
import java.util.Random;

import javax.swing.JButton;
import javax.swing.JFrame;
import javax.swing.JLabel;
import javax.swing.JPanel;
import javax.swing.JTextField;

import nbtool.data.log.Block;
import nbtool.data.log.Log;
import nbtool.util.test.TestBase;
import nbtool.util.test.Tests;


public class Utility {

	/* true if top, false if bot */
	public static boolean camera(Log imageLog) {
		assert(imageLog.blocks.size() > 0);
		Block imageBlock = imageLog.blocks.get(0);

		if (imageBlock.whereFrom.equals("camera_TOP")) {
			return true;
		}

		if (imageBlock.whereFrom.equals("camera_BOT")) {
			return false;
		}

		throw new RuntimeException("unknown camera: " + imageBlock.whereFrom);
	}

	/* for testing components standalone */
	public static void display(Component comp) {
		JFrame display = new JFrame("test of " + comp);
		display.add(comp, BorderLayout.CENTER);
		display.setSize(comp.getPreferredSize());
		display.setVisible(true);
		display.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
	}

	public static String progressString(int characters, double fraction) {
		if (fraction < 0 || fraction > 1.0) {
			return "invalid fraction";
		}

		final String format = "[%s%s]";
		int internal = characters - 2;
		int left = (int) (fraction * internal);
		int right = internal - left;
		assert(left >= 0 && right >= 0);
		char[] lc = new char[left];
		Arrays.fill(lc, '|');
		char[] rc = new char[right];
		Arrays.fill(rc, ' ');
		return String.format(format, new String(lc), new String(rc));
	}

	private static final Map<Class<?>, Long> indexMap = new HashMap<>();
	public static final long getNextIndex(Object type) {
		return getNextIndex(type.getClass());
	}

	public static final long getNextIndex(Class<?> type) {
		synchronized(type) {
			if (indexMap.containsKey(type)) {
				long index = indexMap.get(type);
				++index;
				indexMap.put(type, index);
				return index;
			} else {
				synchronized(indexMap) {
					indexMap.put(type, 0L);
					return 0L;
				}
			}
		}
	}

	private static final Random hexRandom = new Random();
	public static String getRandomHexString(int numchars){
	        StringBuffer sb = new StringBuffer();
	        while(sb.length() < numchars){
	            sb.append(Integer.toHexString(hexRandom.nextInt()).toUpperCase());
	        }

	        return sb.toString().substring(0, numchars);
	}

	public static class Pair<A,B> {
		public A a;
		public B b;
		public Pair(A a, B b) {this.a = a; this.b = b;}
	}

	public static StackTraceElement codeLocation(int i) {
		StackTraceElement[] stack = (new Throwable()).getStackTrace();
		return stack[i];
	}

	public static byte[] subArray(byte[] ar, int start, int len) {
		byte[] ret = new byte[len];

		System.arraycopy(ar, start, ret, 0, len);

		return ret;
	}

//	//Almost all image logs will have null or [Y8(U8/V8)] encoding, but this method should be extended if that changes.
//	public static BufferedImage biFromLog(_Log log) {
//		assert(log.primaryType().equalsIgnoreCase(ToolSettings.IMAGE_S));
//		int width = log.primaryWidth();
//		int height = log.primaryHeight();
//		String encoding = log.primaryEncoding();
//
//		ImageParent ip = null;
//		if (encoding == null ) {
//			//old image
//			ip = new YUYV8888Image(width / 2, height, log.bytes);
//		} else if (encoding.equalsIgnoreCase("[Y8(U8/V8)]")) {
//			ip = new YUYV8888Image(2*width, height, log.bytes);
//		} else if (encoding.equalsIgnoreCase("[Y16]")) {
//			ip = new Y16Image(width , height, log.bytes);
//		} else if (encoding.equalsIgnoreCase("[Y8]")) {
//			ip = new Y8Image(width , height, log.bytes);
//		} else {
//			Debug.warn( "Cannot use image with encoding:" + encoding);
//			return null;
//		}
//
//		return ip.toBufferedImage();
//	}

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

	public static JPanel labelWithPanel(JLabel lbl, JPanel pnl) {
		JPanel p = new JPanel(new BorderLayout());
		p.add(lbl,BorderLayout.WEST);
		p.add(pnl,BorderLayout.CENTER);

		return p;
	}

	/*
	 * assuming the .proto files have the necessary java_options,
	 * this function can get the java class of an protobuf from its name
	 * (the type as used in c++)
	 * */

	@SuppressWarnings("unchecked")
	public static Class<? extends com.google.protobuf.Message> protobufClassFromType(String _type) {
		assert(_type.startsWith(ToolSettings.PROTOBUF_TYPE_PREFIX));
		String type = _type.substring(ToolSettings.PROTOBUF_TYPE_PREFIX.length()); //Remove prefix.
		String classname = String.format("messages.%s", type);

		try {
			return (Class<? extends com.google.protobuf.Message>) Class.forName(classname);
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

	public static String localizePath(String p) {
		if (p.startsWith("~" + File.separator)) {
			return System.getProperty("user.home")
					+ p.substring(1);
		} else return p;
	}

	public static void _NBL_ADD_TESTS_() {
		Tests.add("Utility", new TestBase("getNextIndex"){

			@Override
			public boolean testBody() throws Exception {
				class Temp{public long id = getNextIndex(this);}
				Temp a = new Temp();
				assert(a.id == 0);
				Temp b = new Temp();
				assert(a.id == 0 && b.id == 1);
				return true;
			}

		}
		//...
				);

	}
}
