package nbtool.gui.logviews.sound.whistle;

import java.awt.BorderLayout;
import java.io.IOException;
import java.nio.file.FileVisitResult;
import java.nio.file.Files;
import java.nio.file.Path;
import java.nio.file.Paths;
import java.nio.file.SimpleFileVisitor;
import java.nio.file.attribute.BasicFileAttributes;

import javax.swing.JScrollPane;

import nbtool.data.log.Block;
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

	FloatBuffer buff = null;
	SoundPane pane = null;
	JScrollPane scroll = new JScrollPane();

	@Override
	public void setupDisplay() {
		Debug.info("view!");

		CrossInstance ci = CrossServer.instanceByIndex(0);

		if (ci == null) return;
		ci.tryAddCall(this, "whistle_detect", this.displayedLog);
		this.add(scroll, BorderLayout.CENTER);
	}

	@Override
	public String[] displayableTypes() {
		return new String[]{"DetectAmplitude"};

	}

	@Override
	public void ioFinished(IOInstance instance) { }

	@Override
	public void ioReceived(IOInstance inst, int ret, Log... out) {
		assert(out.length == 1);
		int total = 0;
		for (Block b : out[0].blocks) {
			total += b.data.length;
		}

		byte[] all = new byte[total];
		int offset = 0;

		for (Block b : out[0].blocks) {
			System.arraycopy(b.data, 0, all, offset, b.data.length);
			offset += b.data.length;
		}

		buff = new FloatBuffer(new Block(all, ""), out[0].blocks.size(), 2048);

		if (pane != null) {
			scroll.remove(pane);
		}

		pane = new SoundPane(out[0].blocks.size(), 1024) {

			@Override
			public int pixels(int c, int f, int radius) {
				return (int) ((buff.get(f, c) / buff.max()) * radius * -1);
			}

			@Override
			public String peakString() {
				return "max = " + buff.max;
			}

			@Override
			public String selectionString(int c, int f) {
				String fmat = String.format("c%df%d: %f", c, f, buff.get(f, c));
				Debug.print("%s", fmat);
				return fmat;
			}
		};

		scroll.setViewportView(pane);
		repaint();
	}

	@Override
	public boolean ioMayRespondOnCenterThread(IOInstance inst) {
		// TODO Auto-generated method stub
		return false;
	}


	/***************/
	//MASS ANNOTATION STUFF

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
}
