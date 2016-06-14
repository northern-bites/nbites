package nbtool.gui.logviews.sound.whistle;

import javax.sound.sampled.AudioFormat;
import javax.sound.sampled.AudioSystem;
import javax.sound.sampled.LineUnavailableException;
import javax.sound.sampled.SourceDataLine;

public class PlaySound {

	public static void play(byte[] parts) throws LineUnavailableException {
		byte[] buf = new byte[ 1 ];;
	    AudioFormat af = new AudioFormat( 48000, 16, 2, true, false );
	    SourceDataLine sdl = AudioSystem.getSourceDataLine( af );
	    sdl.open();
	    sdl.start();
	    sdl.write(parts, 0, parts.length);

	    sdl.drain();
	    sdl.stop();
	}

}
