package nbtool.gui.logviews.sound;

import java.awt.BorderLayout;
import java.awt.Color;
import java.awt.Dimension;
import java.awt.Graphics;
import java.nio.ByteBuffer;
import java.nio.ByteOrder;

import nbtool.data.Log;
import nbtool.gui.logviews.misc.ViewParent;
import nbtool.util.Logger;

public class SoundView extends ViewParent {
	
	@Override
	public void setLog(Log newlog) {
		SoundBuffer sb = new SoundBuffer(newlog);
		Logger.printf("SoundBuffer.max = %d", sb.max);
		SoundPane<Short> sp = new SoundPane<>(
				SoundBuffer.objectify(sb.left),
				SoundBuffer.objectify(sb.right),
				new SoundPane.Scaler<Short>() {

					@Override
					public int pixelsFor(Short val, int pixels) {
						return (int) (( (long)val *  (long) pixels) / Short.MAX_VALUE) ;
					}
					
				}
				);
		this.add(sp, BorderLayout.CENTER);
	}
	
	public SoundView() {
		super();
		this.setLayout(new BorderLayout());
	}
	
	public static enum Format {
		NBS_S32_LE,
		NBS_S16_LE
	}
}