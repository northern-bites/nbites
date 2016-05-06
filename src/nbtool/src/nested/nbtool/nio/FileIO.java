package nbtool.nio;

import java.io.DataInputStream;
import java.io.IOException;
import java.nio.file.Files;
import java.nio.file.Path;
import java.nio.file.StandardOpenOption;

public class FileIO {
	
	public static String logDescriptionAt(Path path) throws IOException {
		DataInputStream dis = new DataInputStream(
				Files.newInputStream(path, StandardOpenOption.READ));
		
		int descSize = dis.readInt();
		byte[] desc = new byte[descSize];
		dis.readFully(desc);
		dis.close();
		
		String ret = new String(desc);
		ret = ret.replace("\0", "");
		return ret;
	}
	
	

}
