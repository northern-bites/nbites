package nbtool.gui.logviews.sound;

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
		Logger.printf("out length:%d", out.length);
		for (int i = 0; i < out.length; ++i) {
			Logger.printf("%d: %s: [%f,%f]", i, out[i].toString(), out[i].getReal(), out[i].getImaginary());
		}
	}
	
}