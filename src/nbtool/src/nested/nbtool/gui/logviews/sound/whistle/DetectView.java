package nbtool.gui.logviews.sound.whistle;

import java.io.IOException;
import java.nio.file.FileVisitResult;
import java.nio.file.Files;
import java.nio.file.Path;
import java.nio.file.Paths;
import java.nio.file.SimpleFileVisitor;
import java.nio.file.attribute.BasicFileAttributes;

import nbtool.data.log.Log;
import nbtool.data.log.LogReference;
import nbtool.gui.logviews.misc.ViewParent;
import nbtool.io.CommonIO.IOFirstResponder;
import nbtool.io.CommonIO.IOInstance;
import nbtool.nio.CrossServer;
import nbtool.nio.CrossServer.CrossInstance;
import nbtool.nio.FileIO;
import nbtool.util.Debug;
import nbtool.util.Utility;

public class DetectView extends ViewParent implements IOFirstResponder {

	@Override
	public void setupDisplay() {
		Debug.info("view!");

		CrossInstance ci = CrossServer.instanceByIndex(0);

		if (ci == null) return;

		ci.tryAddCall(this, "whistle_detect", this.displayedLog);
	}

	@Override
	public String[] displayableTypes() {
		return new String[]{"DetectAmplitude"};
	}

	private static final Path heard = Paths.get("/Users/pkoch/Robotics/LOG/whistle_HEARD/");
	private static final Path nheard = Paths.get("/Users/pkoch/Robotics/LOG/whistle_NHEARD/");

	public static void main(String[] args) throws IOException {

		assert(Files.exists(heard));
		assert(Files.exists(nheard));



		if (args.length != 2) {
			Debug.error("needs two args: whistle_true path_base");
			return;
		}
		final boolean hb = Boolean.valueOf(args[0]);
		final Path toPath = hb ? heard : nheard;
		Path path = Paths.get(args[1]);

		if (!Files.exists(path) || !Files.isDirectory(path)) {
			Debug.error("bad path %s", args[1]);
			return;
		}

		Files.walkFileTree(path, new SimpleFileVisitor<Path>(){
			@Override
			public FileVisitResult visitFile(Path file, BasicFileAttributes attrs) throws IOException {

				if (file.toString().endsWith(".nblog")) {
					Debug.info("found nblog");
					LogReference ref = FileIO.readRefFromPath(file);

					if (ref.logClass.equals("soundStuff")) {
						Log lg = ref.get();

						lg.logClass = "DetectAmplitude";
						lg.topLevelDictionary.put("WhistleHeard", hb);
						lg.blocks.get(0).type = "SoundAmplitude";

						Debug.info("copy...");
						FileIO.writeLogToPath(toPath.resolve(
								Utility.getRandomHexString(20) + ".nblog"), lg);
					}
				}

				return FileVisitResult.CONTINUE;
			}
		});
	}

	@Override
	public void ioFinished(IOInstance instance) {
		// TODO Auto-generated method stub

	}

	@Override
	public void ioReceived(IOInstance inst, int ret, Log... out) {
		Debug.info("received!");
	}

	@Override
	public boolean ioMayRespondOnCenterThread(IOInstance inst) {
		// TODO Auto-generated method stub
		return false;
	}

}
