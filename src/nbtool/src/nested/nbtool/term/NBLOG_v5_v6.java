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

import nbtool.data.SExpr;
import nbtool.data._log._Log;
import nbtool.io.CommonIO;
import nbtool.util.Debug;
import nbtool.util.Utility;

public class NBLOG_v5_v6 {
	public static void main(String[] args) throws IOException {
		Debug.logf(Debug.INFO, "Attempting to convert %d logs to version6 format...\n\n", args.length);
		long cindex = 0;
		
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

				
				_Log fnd = CommonIO.simpleReadLog(dis);
				
				if (fnd._olddesc_ != null && !Utility.isv6Description(fnd._olddesc_)) {
					Debug.logf(Debug.INFO, "\tFile %s already v6 log!\n", f);
					continue;
				}
				
				if (!Utility.v6Convert(fnd)) {
					Debug.logf(Debug.INFO, "\tCOULD NOT CONVERT FILE %f!\n", f);
					continue;
				}
				
				fnd.setNameFromDesc();
				String n2 = "n"+ (++cindex) + fnd.name;
				DataOutputStream dos = new DataOutputStream(
						new FileOutputStream(n2)
						);
				CommonIO.writeLog(dos, fnd);
				dos.close();
				
				Debug.logf(Debug.INFO, "\tConverted to %s\n", n2);

			} catch (Exception e) {
				e.printStackTrace();
				Debug.logf(Debug.INFO, "\tError loading file %f!\n", f);
			} finally {
				if (dis != null)
					dis.close();
			}
		}
	}
}
