//package nbtool.gui.logviews.sound2;
//
//import java.awt.BorderLayout;
//
//import javax.swing.JLabel;
//
//import org.apache.commons.math3.complex.Complex;
//import org.apache.commons.math3.transform.DftNormalization;
//import org.apache.commons.math3.transform.FastFourierTransformer;
//import org.apache.commons.math3.transform.TransformType;
//
//import nbtool.data._log._Log;
//import nbtool.gui.logviews.misc.ViewParent;
//import nbtool.util.Debug;
//
//public class SndFreqView extends ViewParent {
//
//	ShortBuffer sb = null;
//	
//	JLabel topLabel = new JLabel();
//	
//	@Override
//	public void setLog(_Log newlog) {
//		this.sb = new ShortBuffer();
//		sb.parse(newlog);
//		
//		double[] ls = new double[sb.frames];
//		for (int i = 0; i < ls.length; ++i)
//			ls[i] = sb.left(i);
//		
//		FastFourierTransformer trans = new FastFourierTransformer(DftNormalization.STANDARD);
//		Complex[] out = trans.transform(ls, TransformType.FORWARD);		
//		
//		final Double[] magn = new Double[ls.length];
//		
//		double max = Double.NEGATIVE_INFINITY;
//		for (int i = 0; i < out.length; ++i) {
//			magn[i] = out[i].abs();
//			if (magn[i] > max)
//				max = magn[i];			
//		}
//		final double MAX = max;
//		
//		SoundPane sp = new SoundPane(1, sb.frames) {
//
//			@Override
//			public int pixels(int c, int f, int radius) {
//				return (int) ((magn[f] / MAX) * radius);
//			}
//
//			@Override
//			public String peakString() {
//				return "max = " + MAX;
//			}
//
//			@Override
//			public String selectionString(int c, int f) {
//				String fmat = String.format("c%df%d: %f", c, f, magn[f]);
//				Debug.print("%s", fmat);
//				return fmat;
//			}
//			
//		};
//		
//		this.add(topLabel, BorderLayout.NORTH);
//		this.add(sp, BorderLayout.CENTER);
//	}
//	
//	public SndFreqView() {
//		super();
//		this.setLayout(new BorderLayout());
//	}
//}