package nbtool.io;

import java.io.DataInputStream;
import java.io.DataOutputStream;
import java.io.EOFException;
import java.io.IOException;
import java.net.InetSocketAddress;
import java.net.Socket;
import java.net.SocketTimeoutException;
import java.nio.channels.UnresolvedAddressException;
import java.nio.charset.StandardCharsets;

import nbtool.data.Log;
import nbtool.util.NBConstants;
import nbtool.util.U;

public class CommonIO {
	
	public static void writeLog(DataOutputStream dos, Log l) throws IOException {
		byte[] cbytes = l.description.getBytes(StandardCharsets.UTF_8);
		byte[] dbytes = l.bytes;
		
		//Ensure string is null-terminated.
		dos.writeInt(cbytes.length + 1);
		dos.write(cbytes);
		dos.writeByte(0);
		
		if (l.bytes != null) {
			dos.writeInt(dbytes.length);
			dos.write(dbytes);
		} else {
			dos.writeInt(0);
		}
		
		dos.flush();
	}
	
	public static Log readLog(DataInputStream dis) throws IOException {
		String desc = readLogDescription(dis);
		
		int len = dis.readInt();
		//U.w("COMMON IO: data len " + len);
		byte[] dbytes = new byte[len];
		dis.readFully(dbytes);
				
		return new Log(desc, dbytes);
	}

	public static String readLogDescription(DataInputStream dis) throws IOException {
		int len = -1;
		len = dis.readInt();
		//U.w("COMMON IO: desc len " + len);
		byte[] cbytes = new byte[len];
		dis.readFully(cbytes);
		
		String desc = new String(cbytes);
		desc = desc.replace("\0", "");	//Remove any null characters.
		
		return desc;
	}
	
	/**
	 * 'loading' a log should be done by reading the log in fully, then checking that the description matches the log's original description.
	 * @throws IOException 
	 * */
	
	
	/*
	 * Can't use (host, port) constructor because it doesn't allow setting of timeout.
	 * */
	protected static Socket setupNetSocket(String addr, int port) throws IOException {
		Socket newsock = new Socket();
		
		InetSocketAddress address = new InetSocketAddress(addr, port);
		if (address.isUnresolved()) {
			U.w("CommonIO.setupNetSocket(): ERROR: Could not resolve address: " + addr);
			throw new UnresolvedAddressException();
		}
		try {
			newsock.connect(new InetSocketAddress(addr, port), NBConstants.SOCKET_TIMEOUT);
		} catch (SocketTimeoutException ste) {
			U.w("CommonIO.setupNetSocket() could not connect to socket within timeout!");
			throw ste;
		}
		
		return newsock;
	}
}
