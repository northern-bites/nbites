package nbtool.term;

import java.io.BufferedInputStream;
import java.io.BufferedOutputStream;
import java.io.ByteArrayInputStream;
import java.io.DataInputStream;
import java.io.DataOutputStream;
import java.io.File;
import java.io.FileInputStream;
import java.io.FileOutputStream;
import java.io.IOException;
import java.nio.ByteBuffer;
import java.nio.ByteOrder;
import java.nio.channels.FileChannel;
import java.util.ArrayList;
import java.util.LinkedList;

import messages.Header;
import nbtool.data.SExpr;
import nbtool.data._log._Log;
import nbtool.io.CommonIO;
import nbtool.util.Debug;
import nbtool.util.Utility;

public class NBLOG_pack {
	public static void main(String[] args) throws IOException {
		Debug.logf(Debug.INFO, "Attempting pack %d logs into OLD format...\n\n", args.length);
		
		LinkedList<_Log> accepted = new LinkedList<_Log>();
		
		for (String f : args) {
			Debug.logf(Debug.INFO, "file %s\n", f);
			
			File lf = new File(f);
			
			if (!lf.exists() ) {
				Debug.logf(Debug.INFO, "\t... file does not exist.\n");
				continue;
			}
			
			if (lf.isDirectory()) {
				Debug.logf(Debug.INFO, "\t... is directory.\n");
				continue;
			}
			
			if (!f.endsWith(".nblog") ) {
				Debug.logf(Debug.INFO, "\t... file isn't nblog.\n");
				continue;
			}
			
			long tlen = lf.length();
			if (tlen < 8 ) { //min size
				Debug.logf(Debug.INFO, "\t... could not get reasonable value for file size.\n");
				continue;
			}
						
			DataInputStream dis = null;
			try {
				FileInputStream fis = new FileInputStream(lf);
				dis = new DataInputStream(new BufferedInputStream(fis));
				
				_Log found = CommonIO.simpleReadLog(dis);
				
				if (Utility.isv6Description(found._olddesc_)) {
					found.setTree(SExpr.deserializeFrom(found._olddesc_));
					
					if (found.primaryType().equals("YUVImage")) {
						found.name = f;
						accepted.add(found);
					}
				}
				
			} catch (Exception e) {
				e.printStackTrace();
				Debug.logf(Debug.INFO, "\tError loading file %f!\n", f);
			} finally {
				if (dis != null)
					dis.close();
			}
		}
		
		
		packTo(null, accepted);
	}
	
	public static void packTo(String path, LinkedList<_Log> accepted) throws IOException {
		Debug.logf(Debug.INFO, "Found %d acceptable logs for concatenation.\n", accepted.size());
		//... concatenate...
		
		if (accepted.size() == 0)
			return;
		
		ArrayList<_Log> top = new ArrayList<_Log>();
		ArrayList<_Log> bot = new ArrayList<_Log>();
		
		for (_Log l : accepted) {
			SExpr fromField = l.tree().find("contents").get(1).find("from");
			assert(fromField.exists() && fromField.count() == 2);
			String from = fromField.get(1).value();
			
			if (from.contains("TOP") || from.contains("top"))
				top.add(l);
			else if (from.contains("BOT") || from.contains("bot"))
				bot.add(l);
			else {
				Debug.logf(Debug.INFO, "Image Log %s [%s] UNKNOWN FROM FIELD!: %s\n", l.name, l.description(), from);
			}
		}
		
		{
			String topName = path == null ? "top.log" : String.format("%s/top.log", path); 
			File logf = new File(topName);
			if (!logf.exists())
				logf.createNewFile();
			
			Header head = Header.newBuilder().setName("messages.YUVImage")
					.setVersion(3).setTimestamp(42).setTopCamera(true).build();
			byte[] headBytes = head.toByteArray();
			
			int length = 4 + headBytes.length;
			for (_Log l : top)
				length += l.bytes.length + (4 * 3);
			
			ByteBuffer bb = ByteBuffer.allocate(length);
			bb.order(ByteOrder.LITTLE_ENDIAN);
			
			bb.putInt(headBytes.length);
			bb.put(headBytes);
			
			for (_Log l : top) {
				SExpr c1 = l.tree().find("contents").get(1);
				int size = c1.find("nbytes").get(1).valueAsInt();
				int width = c1.find("width").get(1).valueAsInt() * 2;
				int height = c1.find("height").get(1).valueAsInt();
				
				bb.putInt(size);
				bb.putInt(width);
				bb.putInt(height);
				bb.put(l.bytes, 0, size);
			}
			
			bb.flip();
			FileOutputStream fos = new FileOutputStream(logf);
			FileChannel fc = fos.getChannel();
			fc.write(bb);
			fc.close();
			fos.close();
			
			Debug.logf(Debug.INFO, "Wrote %d logs to %s.\n", top.size(), topName);
		}
		
		{
			String botName = path == null ? "bottom.log": String.format("%s/bottom.log", path); 
			File logf = new File(botName);
			if (!logf.exists())
				logf.createNewFile();
			
			Header head = Header.newBuilder().setName("messages.YUVImage")
					.setVersion(3).setTimestamp(42).setTopCamera(false).build();
			byte[] headBytes = head.toByteArray();
			
			int length = 4 + headBytes.length;
			for (_Log l : top)
				length += l.bytes.length + (4 * 3);
			
			ByteBuffer bb = ByteBuffer.allocate(length);
			bb.order(ByteOrder.LITTLE_ENDIAN);
			
			bb.putInt(headBytes.length);
			bb.put(headBytes);
			
			for (_Log l : bot) {
				SExpr c1 = l.tree().find("contents").get(1);
				int size = c1.find("nbytes").get(1).valueAsInt();
				int width = c1.find("width").get(1).valueAsInt() * 2;
				int height = c1.find("height").get(1).valueAsInt();
				
				bb.putInt(size);
				bb.putInt(width);
				bb.putInt(height);
				bb.put(l.bytes, 0, size);
			}
			
			FileOutputStream fos = new FileOutputStream(logf);
			FileChannel fc = fos.getChannel();
			fc.write(bb);
			fc.close();
			fos.close();
			
			Debug.logf(Debug.INFO, "Wrote %d logs to %s.\n", bot.size(), botName);
		}
	}
}
