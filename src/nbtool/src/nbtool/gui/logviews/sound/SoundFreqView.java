package nbtool.gui.logviews.sound;

import java.awt.BorderLayout;

import org.apache.commons.math3.complex.Complex;
import org.apache.commons.math3.transform.DftNormalization;
import org.apache.commons.math3.transform.FastFourierTransformer;
import org.apache.commons.math3.transform.TransformType;

import nbtool.data.Log;
import nbtool.gui.logviews.misc.ViewParent;
import nbtool.util.Logger;

public class SoundFreqView extends ViewParent {
	SoundBuffer buffer = null;
	
	@Override
	public void setLog(Log newlog) {
		this.buffer = new SoundBuffer(newlog);
		
		double[] ls = buffer.left();
		FastFourierTransformer trans = new FastFourierTransformer(DftNormalization.STANDARD);
		Complex[] out = trans.transform(ls, TransformType.FORWARD);		
		
		Double[] magn = new Double[buffer.left.length];
		Double[] zero = new Double[buffer.left.length];
		
		
		
		for (int i = 0; i < out.length; ++i) {
			magn[i] = out[i].abs();
			zero[i] = 0d;
		}
		
		SoundPane<Double> sp = new SoundPane<>(magn, zero, new SoundPane.Scaler<Double>() {

			@Override
			public int pixelsFor(Double val, int pixels) {
				return (int) (val * pixels);
			}
		});
		
		this.add(sp, BorderLayout.CENTER);
	}
	
}