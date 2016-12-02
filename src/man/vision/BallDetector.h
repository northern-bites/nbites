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

		const int BOT_RESTRICTION_BUF = 15;
		const int TOP_RESTRICTION_BUF = 10;

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

             // custom constructor
	Ball(Spot & s, double x_, double y_, double cameraH_, int imgHeight_,
				 int imgWidth_, bool tc,
				 double cx, double cy, double conf);

			// default constructor`
			Ball();

                        // properties of the ball?
			std::string properties();

            // why this indentation?
            Spot & getSpot() { return spot; }

                        // confidence that this is in fact the ball?
			double confidence() const { return _confidence; }

			// For tool (what does this do?)
			void compute();

			double pixDiameterFromDist(double d) const;

            // again, why the indentation here?
            Spot spot;

                        // Not sure what these thresholds are for; maybe not
                        // worth looking into
			FuzzyThr thresh;
			FuzzyThr radThresh;

                        // x- and y-coordinates of the ball and radius
			int centerX;
			int centerY;
			int radius;

                        // the x- and y-position of the ball relative
                        // to the robot
			double x_rel;
			double y_rel;

                        // this is used in only one other place and
                        // appears to be the height of the camera, but
                        // there is a "cameraHeight" variable and a 
                        // "cameraH_" variable already
			double cameraH;

                        // distance of ball from robot
			double dist;

                        // is part of the ball occluded?
			bool occludedSide;
			bool occludedTop;
			bool occludedBottom;

                        // is the image from the top camera?
			bool topCamera;

                        // height and width of the image
			int imgHeight, imgWidth;

                        // the diameter one would expect the ball to be
			double expectedDiam;

                        // ?
			double diameterRatio;

                        // confidence that this is in fact the ball
			double _confidence;

                        // what kind of details?
			std::string details;

		}; // end ball class definition

		typedef std::vector<std::pair<int, int>> intPairVector;

                // a vector of spots
		typedef std::vector<Spot> spotVector;

                // a point in the image 
		typedef std::pair<double, double> imagePoint;

                // BallDetector class definition
class BallDetector {
	public:
                // constructor and destructor
		BallDetector(FieldHomography* homography_, Field* field_, bool topCamera);
		~BallDetector();

                // lets you draw stuff in the tool
		void setDebugImage(DebugImage * di);
       
            // these attempt to filter real balls from things that just
            // *look* like balls
            bool edgeSanityCheck(int x, int y, int radius);

            // this doesn't seem to be used anywhere...
            void sanityChecks(int bx, int by, int radius);

            // presumably, all edges in image
            void makeEdgeList(EdgeList & edges);

            // functions for scanning the image
            int scanX(int startX, int startY, int direction, int stop);
            int scanY(int startX, int startY, int direction, int stop);

            int projectedBallRadius(imagePoint p);

            // a centroid is a center of mass, right?
            imagePoint findPointsCentroid(intPairVector & v);
            bool pointsEquidistantFromCentroid(intPairVector & v, int projectedBallRadius);
            
            int getAzimuthColumnRestrictions(double az);
            int getAzimuthRowRestrictions(double az);
            void adjustWindow(int &startCol, int & endCol, int & endRow);

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

			bool whiteBelowSpot(Spot spot);
                        bool topOfBallBrighterThanBottom(Spot spot); // WILL + MARCUS
                        float getAvgBrightness(Spot spot);           // WILL + MARCUS
			bool greenAroundBallFromCentroid(imagePoint p);
			bool whiteNoBlack(Spot spot);
			bool checkGradientInSpot(Spot spot);
			bool checkGradientAroundSpot(int r);
			bool checkDiagonalCircle(Spot spot);
			bool checkBallHasNoGreenAndSomeWhite(int r);

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
						   ImageLiteU16 yImg, EdgeDetector * edgeD);
			void getColor(int x, int y);
			int  getGreen();
			int  getWhite();
			bool isGreen();
			bool isWhite();
			bool isBlack();


            // coordinate conversion functions
            void billToImageCoordinates(double bx, double by, double & ix, double & iy);
            void imageToBillCoordinates(double ix, double iy, double & bx, double & by);

                        // Best candidate ball?
			Ball& best() { return _best; }

			// For tool
#ifdef OFFLINE
                        // gets candidate balls
			const std::vector<Ball>& getBalls() const { return candidates; }

                        // gets vector of white spots
			const std::vector<Spot>& getWhiteSpots() { return debugWhiteSpots; }
    
                        // gets vector of black spots
			const std::vector<Spot>& getBlackSpots() { return debugBlackSpots; }

                        // functions to set various debug parameters
			void setDebugBall(bool debug) {debugBall = debug;}
			void setDebugSpots(bool debug) {debugSpots = debug; }
			void setDebugFilterDark(int fd) { filterThresholdDark = fd; }
			void setDebugGreenDark(int gd) { greenThresholdDark = gd; }
			void setDebugFilterBrite(int fb) { filterThresholdBrite = fb; }
			void setDebugGreenBrite(int gb) { greenThresholdBrite = gb; }
#endif
		private:
			FieldHomography* homography;
			Field* field;
			bool topCamera;

                        // width and height of the white image
			int width;
			int height;
     
                        // current x- and y-coordinates of the pixel
                        // being checked (there are a lot of checks)
			int currentX, currentY;
            std::vector<Edge> goodEdges;

                        // black and white spots to be debugged?
			spotVector debugBlackSpots;
			spotVector debugWhiteSpots;

                        // what is a Connectivity object?
			Connectivity blobber;

			DebugImage debugDraw;
			ImageLiteU8 whiteImage, greenImage, blackImage;
			ImageLiteU16 yImage;
			EdgeDetector * edgeDetector;
			EdgeList * edgeList;

                        // best candidate ball?
			Ball _best;

                        // Does the ball detector still look for white blobs?
			enum SpotType {
				WHITE_CANDIDATE = 1,
				WHITE_REJECT,
				DARK_CANDIDATE,
				DARK_REJECT,
				WHITE_BLOB,
				WHITE_BLOB_BAD
			};

			// everything in the '#ifdef OFFLINE' section is used
                        // in the tool only
			std::vector<Ball> candidates;
#ifdef OFFLINE
			bool debugBall;
			bool debugSpots;
			int filterThresholdDark;
			int greenThresholdDark;
			int filterThresholdBrite;
			int greenThresholdBrite;
#else
                        // default tool parameters?
			static const bool debugBall = false;
			static const bool debugSpots = false;
			static const int filterThresholdDark = 104;
			static const int greenThresholdDark = 12;
			static const int filterThresholdBrite = 130;
			static const int greenThresholdBrite = 80;
#endif
		}; // end BallDetector class definition

	}
}
