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

import messages.HeaderOuterClass.Header;
import nbtool.data.Log;
import nbtool.data.SExpr;
import nbtool.io.CommonIO;
import nbtool.util.Logger;
import nbtool.util.Utility;

public class NBLOG_pack {
	public static void main(String[] args) throws IOException {
		Logger.logf(Logger.INFO, "Attempting pack %d logs into OLD format...\n\n", args.length);
		
		LinkedList<Log> accepted = new LinkedList<Log>();
		
		for (String f : args) {
			Logger.logf(Logger.INFO, "file %s\n", f);
			
			File lf = new File(f);
			
			if (!lf.exists() ) {
				Logger.logf(Logger.INFO, "\t... file does not exist.\n");
				continue;
			}
			
			if (lf.isDirectory()) {
				Logger.logf(Logger.INFO, "\t... is directory.\n");
				continue;
			}
			
			if (!f.endsWith(".nblog") ) {
				Logger.logf(Logger.INFO, "\t... file isn't nblog.\n");
				continue;
			}
			
			long tlen = lf.length();
			if (tlen < 8 ) { //min size
				Logger.logf(Logger.INFO, "\t... could not get reasonable value for file size.\n");
				continue;
			}
						
			DataInputStream dis = null;
			try {
				FileInputStream fis = new FileInputStream(lf);
				dis = new DataInputStream(new BufferedInputStream(fis));
				
				Log found = CommonIO.simpleReadLog(dis);
				
				if (!Utility.isv6Description(found._olddesc_)) {
					found.setTree(SExpr.deserializeFrom(found._olddesc_));
					
					if (found.primaryType().equals("YUVImage")) {
						found.name = f;
						accepted.add(found);
					}
				}
				
			} catch (Exception e) {
				e.printStackTrace();
				Logger.logf(Logger.INFO, "\tError loading file %f!\n", f);
			} finally {
				if (dis != null)
					dis.close();
			}
		}
		
		
		packTo(null, accepted);
	}
	
	public static void packTo(String path, LinkedList<Log> accepted) throws IOException {
		Logger.logf(Logger.INFO, "Found %d acceptable logs for concatenation.\n", accepted.size());
		//... concatenate...
		
		if (accepted.size() == 0)
			return;
		
		ArrayList<Log> top = new ArrayList<Log>();
		ArrayList<Log> bot = new ArrayList<Log>();
		
		for (Log l : accepted) {
			SExpr fromField = l.tree().find("contents").get(1).find("from");
			assert(fromField.exists() && fromField.count() == 2);
			String from = fromField.get(1).value();
			
			if (from.contains("TOP") || from.contains("top"))
				top.add(l);
			else if (from.contains("BOT") || from.contains("bot"))
				bot.add(l);
			else {
				Logger.logf(Logger.INFO, "Image Log %s [%s] UNKNOWN FROM FIELD!: %s\n", l.name, l.description(), from);
			}
		}
		
		{
			String topName = path == null ? "top.log" : String.format("%s/top.log", path); 
			File logf = new File(topName);
			if (!logf.exists())
				logf.createNewFile();
			
			Header head = Header.newBuilder().setName("messages.YUVImage")
					.setVersion(3).setTimestamp(0).setTopCamera(true).build();
			byte[] headBytes = head.toByteArray();
			
			int length = 4 + headBytes.length;
			for (Log l : top)
				length += l.bytes.length + (4 * 3);
			
			ByteBuffer bb = ByteBuffer.allocate(length);
			bb.order(ByteOrder.LITTLE_ENDIAN);
			
			bb.putInt(headBytes.length);
			bb.put(headBytes);
			
			for (Log l : top) {
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
			FileChannel fc = new FileOutputStream(logf).getChannel();
			fc.write(bb);
			fc.close();
			
			Logger.logf(Logger.INFO, "Wrote %d logs to %s.\n", top.size(), topName);
		}
		
		{
			String botName = path == null ? "bottom.log": String.format("%s/bottom.log", path); 
			File logf = new File(botName);
			if (!logf.exists())
				logf.createNewFile();
			
			Header head = Header.newBuilder().setName("messages.YUVImage")
					.setVersion(3).setTimestamp(0).setTopCamera(false).build();
			byte[] headBytes = head.toByteArray();
			
			int length = 4 + headBytes.length;
			for (Log l : top)
				length += l.bytes.length + (4 * 3);
			
			ByteBuffer bb = ByteBuffer.allocate(length);
			bb.order(ByteOrder.LITTLE_ENDIAN);
			
			bb.putInt(headBytes.length);
			bb.put(headBytes);
			
			for (Log l : bot) {
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
			FileChannel fc = new FileOutputStream(logf).getChannel();
			fc.write(bb);
			fc.close();
			
			Logger.logf(Logger.INFO, "Wrote %d logs to %s.\n", bot.size(), botName);
		}
	}
}
