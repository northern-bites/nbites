//package nbtool.gui.logviews.sound2;
//
//public class WhistleDetector {
//	
//	private static final int EXPECTED_FREQ_LEN = 513;
//	private static final int WINDOW_START = 410;
//	private static final int WINDOW_END = 512;
//	private static final float WHISTLE_THRESH = 150;
//	private static final int REQ_FRAMES = 1;
//	
//	public static boolean detect(FFTWBuffer buffer, int c) {
//		
//		assert(buffer.frames == EXPECTED_FREQ_LEN);
//		
//		int frames = 0;
//		for (int i = WINDOW_START; i < WINDOW_END; ++i) {
//			if (buffer.get(i, c) > WHISTLE_THRESH) {
//				++frames;
//				
//				if (frames >= REQ_FRAMES)
//					break;
//			}			
//		}
//		
//		if (frames < REQ_FRAMES)
//			return false;
//		else {
//			int bframes = 0;
//			for (int i = 0; i < WINDOW_START; ++i) {
//				if (buffer.get(i, c) > WHISTLE_THRESH) {
//					++bframes;
//					return false;
//				}
//			}
//			
//			return true;
//		}
//	}
//
//}
