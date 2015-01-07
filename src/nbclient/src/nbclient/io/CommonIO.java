package nbclient.io;

import java.io.DataInputStream;
import java.io.DataOutputStream;
import java.io.EOFException;
import java.io.IOException;

import nbclient.data.Log;

public class CommonIO {
	
	public static void writeLog(DataOutputStream dos, Log l) throws IOException {
		byte[] cbytes = l.description.getBytes();
		byte[] dbytes = l.bytes;
		
		dos.writeInt(cbytes.length + 1);
		dos.write(cbytes);
		dos.writeByte(0);
		dos.writeInt(dbytes.length);
		dos.write(dbytes);
		dos.flush();
	}
	
	public static Log readLog(DataInputStream dis) throws IOException, EOFException {
		int len = -1;
		
		len = dis.readInt();
		byte[] cbytes = new byte[len];
		dis.readFully(cbytes);
		
		len = dis.readInt();
		byte[] dbytes = new byte[len];
		dis.readFully(dbytes);
		
		String desc = new String(cbytes);
		
		return new Log(desc, dbytes);
	}

}
