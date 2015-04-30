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

import nbtool.data.Log;
import nbtool.data.SExpr;
import nbtool.io.CommonIO;
import nbtool.util.U;

public class NBLOG_v5_v6 {
	public static void main(String[] args) throws IOException {
		U.wf("Attempting to convert %d logs to version6 format...\n\n", args.length);
		
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

				Log fnd = CommonIO.readLog(dis);
				
				if (U.is_v6Log(fnd)) {
					U.wf("\tFile %f already v6 log!\n", f);
					continue;
				}
				
				if (!U.v6Convert(fnd)) {
					U.wf("\tCOULD NOT CONVERT FILE %f!\n", f);
					continue;
				}
				
				DataOutputStream dos = new DataOutputStream(
						new FileOutputStream(lf)
						);
				CommonIO.writeLog(dos, fnd);
				dos.close();
				
				U.wf("\tConverted.\n");

			} catch (Exception e) {
				e.printStackTrace();
				U.wf("\tError loading file %f!\n", f);
			} finally {
				if (dis != null)
					dis.close();
			}
		}
	}
}
