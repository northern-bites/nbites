package nbtool.term;

import java.io.BufferedInputStream;
import java.io.BufferedOutputStream;
import java.io.ByteArrayInputStream;
import java.io.DataInputStream;
import java.io.DataOutputStream;
import java.io.File;
import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;

import nbtool.data._log._Log;
import nbtool.io.CommonIO;
import nbtool.util.Debug;
import nbtool.util.Utility;

public class NBLOG_v2_v4 {
	public static void main(String[] args) {
		Debug.logf(Debug.INFO, "Attempting to convert %d files:\n\n", args.length);

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

			try {
				FileInputStream fis = new FileInputStream(lf);
				DataInputStream dis = new DataInputStream(new BufferedInputStream(fis));

				int desclen = dis.readInt();
				byte[] descb = new byte[desclen];
				dis.readFully(descb);

				byte[] rest_of_file = new byte[(int) (tlen - (long)desclen - 4L)];
				dis.readFully(rest_of_file);
				dis.close();

				DataInputStream rofdis = new DataInputStream(new ByteArrayInputStream(rest_of_file));
				int datalen = rofdis.readInt();
				rofdis.close();

				if (datalen != rest_of_file.length - 4) {
					Debug.logf(Debug.INFO, "\t... reformatting...");

					_Log log = new _Log((new String(descb)).replace("\0", ""), rest_of_file);

					DataOutputStream dos = new DataOutputStream(
							new BufferedOutputStream(
									new FileOutputStream(lf)));

					CommonIO.writeLog(dos, log);

					dos.close();

					Debug.logf(Debug.INFO, "done\n");
				} else {
					Debug.logf(Debug.INFO, "\tLOG [%s] ALREADY IN v4 FORMAT\n", f);
				}

			} catch (Exception e) {
				e.printStackTrace();
				continue;
			}
		}
	}
}
