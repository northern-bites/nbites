package TOOL.Misc;

import java.util.LinkedList;

// *****************
// *               *
// *  Hough Space  *
// *               *
// *****************
//
// A cylindrical Hough space covering the range -128 <= R < 128 pixels,
// with direction T covering a full 8-bit binary angle.
public class HoughSpace {
    public static final int R_SPAN = 256, T_SPAN = 256;
    int[][] hs;
    boolean[][] peak;

    // Operating parameters
    public int          acceptThreshold = 24;
    public boolean      extendedPeak    = false;
    public int          angleSpread     = 9;

    public LinkedList<HoughLine> lines;

    public HoughSpace(){
        lines = new LinkedList<HoughLine>();
        hs    = new int[R_SPAN][T_SPAN+1];
        peak  = new boolean[R_SPAN][T_SPAN];
    }

    public void clear(){
        for (int t = 0; t < T_SPAN; ++t){
            for (int r = 0; r < R_SPAN; r++){
                hs  [r][t] = 0;
                peak[r][t] = false;
            }
        }
        lines.clear();
    }

    public static int getR(int x, int y, int t){

        double a = (t & 0xFF) * Math.PI / 128.0;
        return (int)Math.floor(x * Math.cos(a) + y * Math.sin(a));
    }

    // effect   Do Hough processing for a boundary point at (x,y) with
    //          direction t0 <= t <= t1
    // requires t0 <= t1, so for example to go from binary angle 0xFC to
    //          0x04, one would specify t0 = 0xFC, t1 = 0x104
    // note     This is the inner loop of Hough boundary point processing
    public void edge(int x, int y, int t0, int t1){
        int r0 = getR(x, y, t0);
        for (int t = t0; t <= t1; ++t){
            int t8 = t & 0xFF;
            int r1 = getR(x, y, t8 + 1);

            for (int r = Math.min(r0, r1); r <= Math.max(r0, r1); ++r){
                int ri = r + R_SPAN / 2;
                if (0 <= ri && ri < R_SPAN){
                    ++hs[ri][t8];
                }
            }
            r0 = r1;
        }
    }
    // effect Smooth Hough space with a 2x2 boxcar kernel
    //
    // notes Smoothing corrects (mostly) for Hough space quantization
    //       effects. If an image line happens to correspond to the center
    //       of a Hough point, the boundary points will mostly fall into one
    //       Hough accumulator; if the image line happens to fall away from
    //       a center, the counts will be spread out among neighboring
    //       points in Hough space. Also, if an image line is degraded
    //       (e.g. noisy), the counts will spread out. Smoothing helps.
    void smooth() {
        // Make a copy of the t = 0 row at t = TSpan, because t = 0 is a
        // neighbor of t = TSpan-1 (cylindrical space), but t = 0 will
        // be overwritten by the time we get to t = TSpan-1
        for (int r = 0; r < R_SPAN; ++r)
            hs[r][T_SPAN] = hs[r][0];

        // In-place 2x2 boxcar smoothing.
        for (int t = 0; t < T_SPAN; ++t)
            for (int r = 0; r < R_SPAN - 1; ++r)
                hs[r][t] = hs[r][t] + hs[r + 1][t] +
                    hs[r][t + 1] + hs[r + 1][t + 1];
    }

    // effect Find peaks in Hough space, mark them, and add new lines
    //        to the result list for each one.
    //
    // notes  A peak is a point whose count is at least
    //        AcceptThreshold and asymmetrically greater than either 8
    //        (ExtendedPeak = false) or 20 (ExtendedPeak = true)
    //        neighbors. ExtendedPeak = true has been found to be not
    //        useful; it was intended to solve a problem of false
    //        peaks that has now been solved by suppress.
    void peaks(){
        // Neighbor tables
        int[] drTab = {  1,  1,  0, -1, -2, -2, -2, -1,  0,  1};
        int[] dtTab = {  0,  1,  1,  1,  1,  0, -1, -2, -2, -2};

        int peakPoints = 4;
        int thr = 4 * acceptThreshold;  // smoothing has gain 4

        for (int t = 0; t < T_SPAN; ++t)
            for (int r = 2; r < R_SPAN - 2; ++r) {
                int z = hs[r][t];
                if (z >= thr) {
                    boolean shouldCreate = true;
                    for (int i = 0; i < peakPoints && shouldCreate; ++i){

                        if (!(z > hs[r + drTab[i]][(t + dtTab[i]) & 0xFF] &&
                              z >= hs[r - drTab[i]][(t - dtTab[i]) & 0xFF])){
                            shouldCreate = false;
                        }
                    }
                    if (shouldCreate){
                        peak[r][t] = true;
                        lines.add(new HoughLine(r, t,
                                                r - R_SPAN / 2 + 0.5,
                                                (t + 0.5) *
                                                Math.PI / 128.0, z >> 2));
                    }
                }
            }
    }

    // effect Run complete Hough transform on a source edge detected
    //          image, place resulting lines in Lines field. Set allT
    //          true to ignore gradient direction, for purposes of
    //          explaining why this is a bad idea.
    public void run(int[][] xGrad, int[][] yGrad, int[][] mag){
        clear();
        int height = mag.length;
        int width = mag[0].length;

        int x0 = width/2;
        int y0 = height/2;

        for (int y = 0; y < height; ++y){
            for (int x = 0; x < width; ++x){
                if (mag[y][x] > 0){
                    int t = dir(xGrad, yGrad, x, y);
                    edge(x - x0, y - y0,
                         t - angleSpread,
                         t + angleSpread);
                }
            }
        }

        smooth();
        peaks();
        suppress(x0, y0);
    }

    private void suppress(int rx, int ry){
    	// First mark lines for deletion
        boolean[] markForDelete = new boolean[lines.size()];
        for (int i = 0; i < lines.size() - 1; ++i)
            for (int j = i + 1; j < lines.size(); ++j)
                {
                    double px, py;
                    int ijAngle = Math.abs((lines.get(i).tIndex -
                                            lines.get(j).tIndex & 0xFF)
                                           << 24 >> 24);
                    if (0 < ijAngle && ijAngle <= angleSpread &&
                        lines.get(i).intersect(lines.get(j), rx, ry))
                        {
                            if (lines.get(i).score < lines.get(j).score)
                                markForDelete[i] = true;
                            if (lines.get(j).score < lines.get(i).score)
                                markForDelete[j] = true;
                        }
                }

        // Then delete marked lines
        int n = 0;
        for (int i = 0; i < markForDelete.length; ++i){
            if (markForDelete[i])
                lines.remove(n);
            else
                ++n;
        }

    }

    public static int dir(int[][] xArray, int[][] yArray, int x, int y){
        // Calculate the direction of the gradient
        return (int)(Math.atan2(yArray[y][x], xArray[y][x]) /
                     Math.PI * 128.0) & 0xFF;

    }
}