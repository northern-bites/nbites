package nbtool.term;

import java.io.IOException;
import java.nio.file.Files;
import java.nio.file.Path;
import java.nio.file.Paths;

import nbtool.data.log.Block;
import nbtool.data.log.Log;
import nbtool.data.log.LogReference;
import nbtool.nio.FileIO;
import nbtool.util.Debug;
import nbtool.util.Utility;

public class whistle_merge {

	public static void main(String[] args) throws IOException {

		if (args.length < 2) {
			Debug.error("needs at least two args");
			return;
		}

		Path toPath = Paths.get(args[0]);

		if (!Files.exists(toPath) || !Files.isDirectory(toPath)) {
			Debug.error("bad path %s", toPath);
			return;
		}

		Path[] paths = new Path[args.length - 1];

		for (int i = 1; i < args.length; ++i) {
			Path lg = Paths.get(args[i]);

			if (!lg.isAbsolute()) {
				lg = toPath.resolve(lg);
			}

			if (! (Files.exists(lg) && Files.isRegularFile(lg) && lg.toString().endsWith(".nblog"))) {
				Debug.error("bad log path: %s", lg);
				return;
			}

			paths[i - 1] = lg;
		}

		int total = 0;
		byte[][] all = new byte[paths.length][];

		for (int i = 0; i < paths.length; ++i) {
			LogReference ref = FileIO.readRefFromPath(paths[i]);

			all[i] = ref.get().blocks.get(0).data;
			total += all[i].length;

			Debug.info("\t%d bytes from", all[i].length, paths[i]);
		}

		byte[] all_bytes = new byte[total];

		int pos = 0;
		for (int i = 0; i < all.length; ++i) {
			System.arraycopy(all[i], 0, all_bytes, pos, all[i].length);
			pos += all[i].length;
		}

		Log out = Log.emptyLog();
		out.logClass = "DetectAmplitude";
		out.blocks.add(Block.explicit(all_bytes, "SoundAmplitude"));

		FileIO.writeLogToPath(toPath.resolve("merged_" + Utility.getRandomHexString(10) + ".nblog"), out);
	}
}
