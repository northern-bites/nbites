package nbtool.term;

import java.io.File;

import nbtool.util.U;

public class NBLOG_v2_v4 {
	public static void main(String[] args) {
		U.wf("Attempting to convert %d files:\n\n", args.length);
		
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
				
			
		}
	}
}
