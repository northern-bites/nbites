package nbtool.gui.logviews.sound.whistle;

import java.util.LinkedList;

import javax.sound.sampled.AudioFormat;
import javax.sound.sampled.AudioSystem;
import javax.sound.sampled.LineUnavailableException;
import javax.sound.sampled.SourceDataLine;

public class PlaySound {

	private static final LinkedList<Player> queue = new LinkedList<>();
	private static final Thread playingThread = new Thread(new Runnable(){
		@Override
		public void run() {
			for(;;){
				Player player = null;

				for(;;) {
					synchronized(queue) {
						player = queue.poll();

						if (player == null) {
							try {
								queue.wait();
							} catch (InterruptedException e) {
								e.printStackTrace();
							}
						} else break;
					}
				}

				player.run();
			}
		}
	});


	private static class Player implements Runnable {

		Player(byte[] d) {this.data = d;}
		byte[] data;

		@Override
		public void run() {
			try {
				_play(data);
			} catch (LineUnavailableException e) {
				e.printStackTrace();
			}
		}
	}

	private static void start() {
		if (playingThread.isAlive()) return;
		else {
			playingThread.setDaemon(true);
			playingThread.start();
		}
	}

	private static void _play(byte[] parts) throws LineUnavailableException {
		byte[] buf = new byte[ 1 ];;
	    AudioFormat af = new AudioFormat( 48000, 16, 2, true, false );
	    SourceDataLine sdl = AudioSystem.getSourceDataLine( af );
	    sdl.open();
	    sdl.start();
	    sdl.write(parts, 0, parts.length);

	    sdl.drain();
	    sdl.stop();
	}

	public static void play(byte[] parts) {
		start();

		synchronized(queue) {
			queue.add(new Player(parts));
			queue.notify();
		}
	}
}
