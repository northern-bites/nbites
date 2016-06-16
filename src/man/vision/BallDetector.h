#pragma once

#include <vector>
#include <string>
#include <math.h>

#include "Images.h"
#include "Camera.h"
#include "FastBlob.h"
#include "Homography.h"
#include "Field.h"
#include "Hough.h"
#include "Spots.h"


namespace man {
	namespace vision {

        const double BALL_RADIUS = 5.25; //3.25;
		const double VERT_FOV_DEG = 47.64;
		const double VERT_FOV_RAD = VERT_FOV_DEG * M_PI / 180;
		const double HORIZ_FOV_DEG = 60.97;
		const double HORIZ_FOV_RAD = HORIZ_FOV_DEG * M_PI / 180;

#define BLACK 1
#define BLUE 7
#define MAROON 8
#define WHITE 2
#define GREEN 6
#define YELLOW 5
#define RED 3
#define ORANGE 4

		class Ball {
		public:
			Ball(Spot & s, double x_, double y_, double cameraH_, int imgHeight_,
				 int imgWidth_, bool tc,
				 double cx, double cy, double conf);
			Ball();

			std::string properties();

            Spot & getSpot() { return spot; }
			double confidence() const { return _confidence; }

			// For tool
//private: should be private. leaving public for now
			void compute();

			double pixDiameterFromDist(double d) const;

            Spot spot;

			FuzzyThr thresh;
			FuzzyThr radThresh;

			int centerX;
			int centerY;
			int radius;

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

		typedef std::vector<std::pair<int, int>> intPairVector;
		typedef std::vector<Spot> spotVector;

		class BallDetector {
		public:
			BallDetector(FieldHomography* homography_, Field* field_, bool topCamera);
			~BallDetector();

			void setDebugImage(DebugImage * di);
            void edgeSanityCheck(int x, int y, int radius);
            void sanityChecks(int bx, int by, int radius);
            void makeEdgeList(EdgeList & edges);

            int scanX(int startX, int startY, int direction, int stop);
            int scanY(int startX, int startY, int direction, int stop);
            int getAzimuthColumnRestrictions();
            int getAzimuthRowRestrictions();

			bool findBall(ImageLiteU8 white, double cameraHeight, EdgeList& edges);

            void initializeSpotterSettings(SpotDetector &s, bool darkSpot, float innerDiam, 
								float altInnerDiam, bool topCamera, int filterThreshold, 
								int greenThreshold, float filterGain);

            void processDarkSpots(SpotList & darkSpots, intPairVector & blackSpots, 
            						intPairVector & badBlackSpots, spotVector & actualBlackSpots);
            bool processWhiteSpots(SpotList & whiteSpots, intPairVector & blackSpots,
            						intPairVector & badBlackSpots, spotVector & actualWhiteSpots,
            						double cameraHeight, bool & foundBall);
            bool processBlobs(Connectivity & blobber, intPairVector & blackSpots,
                               		bool & foundBall, intPairVector & badBlackSpots,
                                	spotVector & actualWhiteSpots, double cameraHeight,
                                	int bottomQuarer);

			bool whiteNoBlack(Spot spot);
			bool checkDiagonalCircle(Spot spot);

            bool filterBlackSpots(Spot currentBlob);
            bool filterWhiteBlob(Spot spot, intPairVector & blackSpots,
            						intPairVector & badBlackSpots);
            bool filterWhiteSpot(Spot spot, intPairVector & blackSpots,
            						intPairVector & badBlackSpots);
            int filterWhiteBlobs(Blob currentBlob, intPairVector &blobs,
                                  	intPairVector blackBlobs);
            bool findCorrelatedBlackSpots(intPairVector & blackBlobs,spotVector & actualBlobs,
                                          	double cameraHeight, bool & foundBall);
            bool blobsAreClose(std::pair<int,int> first,
                               std::pair<int,int> second);

            void makeBall(Spot blob, double cameraHeight, double conf,
                          bool foundBall, bool isBlack);

            bool lookForFarAwayBalls(Blob blob);

            bool farSanityChecks(Blob blob);
            bool nearSanityChecks(Blob blob);
			bool hardSanityCheck(int leftx, int rightx, int topy, int bottomy);

			void setImages(ImageLiteU8 white, ImageLiteU8 green, ImageLiteU8 black,
						   ImageLiteU16 yImg);
			void getColor(int x, int y);
			bool isGreen();
			bool isWhite();
			bool isBlack();

			Ball& best() { return _best; }

			// For tool
#ifdef OFFLINE
			const std::vector<Ball>& getBalls() const { return candidates; }
			const std::vector<Spot>& getWhiteSpots() { return debugWhiteSpots; }
			const std::vector<Spot>& getBlackSpots() { return debugBlackSpots; }
			void setDebugBall(bool debug) {debugBall = debug;}
			void setDebugFilterDark(int fd) { filterThresholdDark = fd; }
			void setDebugGreenDark(int gd) { greenThresholdDark = gd; }
			void setDebugFilterBrite(int fb) { filterThresholdBrite = fb; }
			void setDebugGreenBrite(int gb) { greenThresholdBrite = gb; }
#endif
		private:
			FieldHomography* homography;
			Field* field;
			bool topCamera;
			int width;
			int height;
			int currentX, currentY;
            std::vector<Edge> goodEdges;
			spotVector debugBlackSpots;
			spotVector debugWhiteSpots;
			Connectivity blobber;

			DebugImage debugDraw;
			ImageLiteU8 whiteImage, greenImage, blackImage;
			ImageLiteU16 yImage;

			Ball _best;

			enum SpotType {
				WHITE_CANDIDATE = 1,
				WHITE_REJECT,
				DARK_CANDIDATE,
				DARK_REJECT,
				WHITE_BLOB,
				WHITE_BLOB_BAD
			};

			// For tool
			std::vector<Ball> candidates;
#ifdef OFFLINE
			bool debugBall;
			int filterThresholdDark;
			int greenThresholdDark;
			int filterThresholdBrite;
			int greenThresholdBrite;
#else
			static const bool debugBall = false;
			static const int filterThresholdDark = 144;
			static const int greenThresholdDark = 60;
			static const int filterThresholdBrite = 144;
			static const int greenThresholdBrite = 120;
#endif
		};

	}
}
