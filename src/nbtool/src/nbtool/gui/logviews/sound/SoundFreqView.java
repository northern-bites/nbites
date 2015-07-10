package nbtool.gui.logviews.sound;

import java.awt.BorderLayout;

import javax.swing.JLabel;

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
		
		double max = Double.NEGATIVE_INFINITY;
		for (int i = 0; i < out.length; ++i) {
			magn[i] = out[i].abs();
			if (magn[i] > max)
				max = magn[i];
			zero[i] = 0d;
			
			//Logger.println("" + magn[i]);
		}
		final double MAX = max;
		SoundPane<Double> sp = new SoundPane<>(magn, zero, new SoundPane.Scaler<Double>() {
			@Override
			public int pixelsFor(Double val, int pixels) {
				return (int) ( (val / MAX) * pixels);
			}
		});
		
		this.add(new JLabel("max=" + MAX), BorderLayout.NORTH);
		this.add(sp, BorderLayout.CENTER);
	}
	public SoundFreqView() {
		super();
		this.setLayout(new BorderLayout());
	}
}