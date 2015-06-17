package nbtool.gui.logviews.images;

public class GeoLine {
	double r,t,end0,end1,houghIndex,fieldIndex;

	public GeoLine() {
		r = 0;
		t = 0;
		end0 = 0;
		end1 = 0;
		houghIndex = 0;
		fieldIndex = 0;
	}

	public GeoLine(double line_r, double line_t, double line_end0, 
			double line_end1, double line_houghInd, double line_fieldInd) {
		r = line_r;
		t = line_t;
		end0 = line_end0;
		end1 = line_end1;
		houghIndex = line_houghInd;
		fieldIndex = line_fieldInd;
	}
}