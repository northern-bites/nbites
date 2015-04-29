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
import java.util.ArrayList;
import java.util.LinkedList;

import messages.HeaderOuterClass.Header;
import nbtool.data.Log;
import nbtool.data.SExpr;
import nbtool.io.CommonIO;
import nbtool.util.U;

public class NBLOG_pack {
	public static void main(String[] args) throws IOException {
		U.wf("Attempting pack %d logs into OLD format...\n\n", args.length);
		
		LinkedList<Log> accepted = new LinkedList<Log>();
		
		for (String f : args) {
			U.wf("file %s\n", f);
			
			File lf = new File(f);
			
			if (!lf.exists() ) {
				U.wf("\t... file does not exist.\n");
				continue;
			}
			
			if (lf.isDirectory()) {
				U.wf("\t... is directory.\n");
				continue;
			}
			
			if (!f.endsWith(".nblog") ) {
				U.wf("\t... file isn't nblog.\n");
				continue;
			}
			
			long tlen = lf.length();
			if (tlen < 8 ) { //min size
				U.wf("\t... could not get reasonable value for file size.\n");
				continue;
			}
			
			DataInputStream dis = null;
			try {
				FileInputStream fis = new FileInputStream(lf);
				dis = new DataInputStream(new BufferedInputStream(fis));

				Log found = CommonIO.readLog(dis);
				if (U.is_v6Log(found) && found.pType().equals("YUVImage")) {
					found.name = f;
					accepted.add(found);
				}
				
			} catch (Exception e) {
				e.printStackTrace();
				U.wf("\tError loading file %f!\n", f);
			} finally {
				if (dis != null)
					dis.close();
			}
		}
		
		U.wf("Found %d acceptable logs for concatenation.\n", accepted.size());
		//... concatenate...
		
		if (accepted.size() == 0)
			return;
		
		ArrayList<Log> top = new ArrayList<Log>();
		ArrayList<Log> bot = new ArrayList<Log>();
		
		for (Log l : accepted) {
			SExpr fromField = l.tree().find("contents").get(1).find("from");
			assert(fromField.exists() && fromField.count() == 2);
			String from = fromField.get(1).value();
			
			if (from.equalsIgnoreCase("cameraTOP"))
				top.add(l);
			else if (from.equalsIgnoreCase("cameraBOT"))
				bot.add(l);
			else {
				U.wf("Image Log %s [%s] UNKNOWN FROM FIELD!\n", l.name, l.description);
			}
		}
		
		{
			String topName = String.format("concat_%d_TOP_images.old", top.size()); 
			File logf = new File(topName);
			if (!logf.exists())
				logf.createNewFile();

			FileOutputStream fos = new FileOutputStream(logf);
			BufferedOutputStream bos = new BufferedOutputStream(fos);
			DataOutputStream dos = new DataOutputStream(bos);
			
			Header head = Header.newBuilder().setName("messages.YUVImage")
					.setVersion(3).setTimestamp(0).setTopCamera(true).build();
			byte[] headBytes = head.toByteArray();
			dos.writeInt(headBytes.length);
			dos.write(headBytes);
			
			for (Log l : top) {
				SExpr c1 = l.tree().find("contents").get(1);
				int size = c1.find("nbytes").get(1).valueAsInt();
				int width = c1.find("width").get(1).valueAsInt();
				int height = c1.find("height").get(1).valueAsInt();
				
				dos.writeInt(size);
				dos.writeInt(width);
				dos.writeInt(height);
				dos.write(l.bytes, 0, size);
			}
			
			dos.close();
			U.wf("Wrote %d logs to %s.\n", top.size(), topName);
		}
		
		{
			String botName = String.format("concat_%d_BOT_images.old", bot.size()); 
			File logf = new File(botName);
			if (!logf.exists())
				logf.createNewFile();

			FileOutputStream fos = new FileOutputStream(logf);
			BufferedOutputStream bos = new BufferedOutputStream(fos);
			DataOutputStream dos = new DataOutputStream(bos);
			
			Header head = Header.newBuilder().setName("messages.YUVImage")
					.setVersion(3).setTimestamp(0).setTopCamera(false).build();
			byte[] headBytes = head.toByteArray();
			dos.writeInt(headBytes.length);
			dos.write(headBytes);
			
			for (Log l : bot) {
				SExpr c1 = l.tree().find("contents").get(1);
				int size = c1.find("nbytes").get(1).valueAsInt();
				int width = c1.find("width").get(1).valueAsInt();
				int height = c1.find("height").get(1).valueAsInt();
				
				dos.writeInt(size);
				dos.writeInt(width);
				dos.writeInt(height);
				dos.write(l.bytes, 0, size);
			}
			
			dos.close();
			U.wf("Wrote %d logs to %s.\n", top.size(), botName);
		}
	}
}
