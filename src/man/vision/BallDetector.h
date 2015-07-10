#pragma once

#include <vector>
#include <string>
#include <math.h>

#include "Images.h"
#include "Camera.h"
#include "FastBlob.h"
#include "Homography.h"
#include "Field.h"


namespace man {
	namespace vision {

		const double BALL_RADIUS = 3.25;
		const double VERT_FOV_DEG = 47.64;
		const double VERT_FOV_RAD = VERT_FOV_DEG * M_PI / 180;
		const double HORIZ_FOV_DEG = 60.97;
		const double HORIZ_FOV_RAD = HORIZ_FOV_DEG * M_PI / 180;

		class Ball {
		public:
			Ball(Blob& b, double x_, double y_, double cameraH_, int imgHeight_,
				 int imgWidth_, bool top, bool os, bool ot, bool ob);
			Ball();

			std::string properties();

			double confidence() const { return _confidence; }

			// For tool
			Blob& getBlob() { return blob; }
//private: should be private. leaving public for now
			void compute();

			double pixDiameterFromDist(double d) const;

			Blob blob;
			FuzzyThr thresh;
			FuzzyThr radThresh;

			double x_rel;
			double y_rel;
			double cameraH;
			double dist;

			bool occludedSide;
			bool occludedTop;
			bool occludedBottom;
			bool topCamera;

			int imgHeight, imgWidth;
			double expectedDiam;
			double diameterRatio;

			double _confidence;

			std::string details;
		};

		class BallDetector {
		public:
			BallDetector(FieldHomography* homography_, Field* field_, bool topCamera);
			~BallDetector();

			bool findBall(ImageLiteU8 orange, double cameraHeight);

			bool preScreen(int centerX, int centerY, int principalLength,
						   int principalLength2,
						   bool & occludedSide, bool & occludedTop,
						   bool & occludedBottom);


			Ball& best() { return _best; }

			// For tool
#ifdef OFFLINE
			const std::vector<Ball>& getBalls() const { return candidates; }
			Connectivity* getBlobber() { return &blobber; }
			void setDebugBall(bool debug) {debugBall = debug;}
#endif
		private:
			Connectivity blobber;
			FieldHomography* homography;
			Field* field;
			bool topCamera;
			int width;
			int height;

			Ball _best;

			// For tool
#ifdef OFFLINE
			std::vector<Ball> candidates;
#endif
#ifdef OFFLINE
			bool debugBall;
#else
			static const bool debugBall = false;
#endif
		};

	}
}
