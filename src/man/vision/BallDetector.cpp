
#include "BallDetector.h"

#include <algorithm>
#include <iostream>


using std::to_string;

namespace man {
namespace vision {

	BallDetector::BallDetector(FieldHomography* homography_,
							   Field* field_, bool topCamera_):
		blobber(),
        blobber2(),
		homography(homography_),
		field(field_),
		topCamera(topCamera_)
	{
        // Note from Chown:  I don't know what these numbers mean,
        // they are holdovers from the Dan Zeller days
		blobber.secondThreshold(115);
		blobber.minWeight(4);
		blobber2.secondThreshold(115);
		blobber2.minWeight(4);
	}

	BallDetector::~BallDetector() { }

	void BallDetector::setDebugImage(DebugImage * di) {
		debugDraw =  *di;
	}

    void BallDetector::filterBlackBlobs(Blob currentBlob,
                                        std::vector<std::pair<int,int>> & blobs)
    {
        int MAXBLACKBLOB = 8;
        float MINBLACKAREA = 10.0f;

        int centerX = static_cast<int>(currentBlob.centerX());
        int centerY = static_cast<int>(currentBlob.centerY());
        int principalLength = static_cast<int>(currentBlob.firstPrincipalLength());
        int principalLength2 = static_cast<int>(currentBlob.secondPrincipalLength());
        int minSecond = 1;
        if (!topCamera) {
            minSecond = 1;
        }
        if (principalLength < MAXBLACKBLOB &&
            principalLength2 >= minSecond && currentBlob.area() > MINBLACKAREA &&
            (centerY > field->horizonAt(centerX) || !topCamera)) {
            blobs.push_back(std::make_pair(centerX, centerY));
            if (debugBall) {
                debugDraw.drawPoint(centerX, centerY, BLUE);
                std::cout << "Black blob " << centerX << " " << centerY <<
                    " " << principalLength << " " << principalLength2;
                }
        }
    }

    int BallDetector::filterWhiteBlobs(Blob currentBlob,
                                        std::vector<std::pair<int,int>> & blobs,
                                        std::vector<std::pair<int,int>> blackBlobs)
    {
        int MAXWHITEBLOB = 40;
        float MINWHITEAREA = 10.0f;
        int centerX = static_cast<int>(currentBlob.centerX());
        int centerY = static_cast<int>(currentBlob.centerY());
        int principalLength = static_cast<int>(currentBlob.firstPrincipalLength());
        int principalLength2 = static_cast<int>(currentBlob.secondPrincipalLength());
        int minSecond = 3;
        if (!topCamera) {
            minSecond = 3;
        }

        // see if the blob is of the right general shape for a ball
        if (principalLength < MAXWHITEBLOB &&
            principalLength2 >= minSecond &&
            principalLength < principalLength2 * 2 &&
            currentBlob.area() > MINWHITEAREA &&
            (centerY > field->horizonAt(centerX) || !topCamera)) {
            blobs.push_back(std::make_pair(centerX, centerY));
            if (debugBall) {
                debugDraw.drawPoint(centerX, centerY, BLUE);
                std::cout << "White blob " << centerX << " " << centerY <<
                    " " << principalLength << " " << principalLength2 << std::endl;
            }
            int count = 0;
            // now loop through the black blobs and see if they are inside
            for (std::pair<int,int> p : blackBlobs) {
                if (abs(p.first - centerX) < principalLength &&
                    abs(p.second - centerY) < principalLength) {
                    count++;
                }
            }
            return count;
        }
        return 0;
    }

    bool BallDetector::findCorrelatedBlackBlobs(std::vector<std::pair<int,int>> & blackBlobs,
                                                double cameraHeight, bool foundBall)
    {
        // loop through the filtered blobs and see if any are close together
        int TOPCAMERABLOBNEARNESS = 30;
        int BOTTOMCAMERABLOBNEARNESS = 20;
        int correlations[blackBlobs.size()];
        int count = 0;
        int closeness = TOPCAMERABLOBNEARNESS;
        if (!topCamera) {
            closeness = BOTTOMCAMERABLOBNEARNESS;
        }
        // loop through filtered black blobs
        for (std::pair<int,int> p : blackBlobs) {
            correlations[count] = 0;
            std::pair<int,int> pair1;
            // we're going to check against all other black blobs
            for (std::pair<int,int> q : blackBlobs) {
                int xdiff = abs(p.first - q.first);
                int ydiff = abs(p.second - q.second);
                if (xdiff < closeness && ydiff < closeness &&
                    (xdiff > 0 || ydiff > 0)) {
                    // could be correlated, watch out for shadows, robot parts, etc.
                    correlations[count] += 1;
                    if (correlations[count] > 1) {
                        if (debugBall) {
                            int midx = (pair1.first + q.first + p.first) / 3;
                            int midy = (pair1.second + q.second + p.second) / 3;
                            debugDraw.drawPoint(p.first, p.second, ORANGE);
                            debugDraw.drawPoint(midx, midy, RED);
                        }
                    } else {
                        pair1 = q;
                    }
                    // Three close black blobs is good evidence for a ball
                    if (correlations[count] > 2) {
                        if (debugBall) {
                            debugDraw.drawPoint(p.first, p.second, GREEN);
                        }
                        double bIX = p.first - width/2;
                        double bIY = (height/2 - p.second) - 10;
                        double x_rel, y_rel;
                        bool belowHoriz = homography->fieldCoords(bIX, bIY,
                                                                  x_rel, y_rel);
                        for (auto i =blobber.blobs.begin();
                             i!=blobber.blobs.end(); i++) {
                            int cX = static_cast<int>((*i).centerX());
                            int cY = static_cast<int>((*i).centerY());
                            if (cX == p.first && cY == p.second) {
                                makeBall((*i), cameraHeight, 0.9, foundBall);
#ifdef OFFLINE
                                foundBall = true;
#else
                                return true;
#endif
                            }
                        }
                    }
                }
            }
            count++;
        }
        // TO DO: This currently finds the first good blob, it ought to find the best
        for (int c = 0; c < count; c++) {
            if (correlations[c] > 1) {
                // good candidate ball
                std::pair<int,int> p = blackBlobs[c];
                for (auto i =blobber.blobs.begin(); i!=blobber.blobs.end(); i++) {
                    int cX = static_cast<int>((*i).centerX());
                    int cY = static_cast<int>((*i).centerY());
                    if (cX == p.first && cY == p.second) {
                        makeBall((*i), cameraHeight, 0.9, foundBall);
#ifdef OFFLINE
                        foundBall = true;
#else
                        return true;
#endif
                    }
                }
            }
        }
        return foundBall;
    }

    void BallDetector::makeBall(Blob blob, double cameraHeight, double conf,
                                bool foundBall)
    {
        int centerX = static_cast<int>(blob.centerX());
        int centerY = static_cast<int>(blob.centerY());
        int principalLength = static_cast<int>(blob.firstPrincipalLength());
        int principalLength2 = static_cast<int>(blob.secondPrincipalLength());
        double bIX = (blob.centerX() - width/2);
        double bIY = (height / 2 - blob.centerY()) -
            blob.firstPrincipalLength();
        double x_rel, y_rel;
        bool belowHoriz = homography->fieldCoords(bIX, bIY, x_rel, y_rel);
        Ball b(blob, x_rel, -1 * y_rel, cameraHeight, height,
               width, topCamera, false, false, false);
        b._confidence = 0.9;
        if (!foundBall) {
            _best = b;
        }
#ifdef OFFLINE
                        candidates.push_back(b);
#endif
    }

    bool BallDetector::findBallWithEdges(ImageLiteU8 white, double cameraHeight)
	{
		Ball reset;
		_best = reset;
		width = white.width();
		height = white.height();

#ifdef OFFLINE
        candidates.clear();
#endif

        bool foundBall = false;
        int FARAWAYWHITESIZE = 15;
        int BOTTOMEDGEWHITEMAX = 20;
        int BUFFER = 10;

        // First we're going to run the blobber on the black image
        blobber.run(orangeImage.pixelAddr(), orangeImage.width(),
                    orangeImage.height(), orangeImage.pitch());

        // Then we are going to filter out all of the blobs that obviously
        // aren't part of the ball
        std::vector<std::pair<int,int>> blackBlobs;
        for (auto i =blobber.blobs.begin(); i!=blobber.blobs.end(); i++) {
            filterBlackBlobs((*i), blackBlobs);
        }

        // Now run the blobber on the white image
        int MAXWHITEBLOB = 40;
        float MINWHITEAREA = 10.0f;
        blobber2.run(white.pixelAddr(), white.width(), white.height(), white.pitch());
        std::vector<std::pair<int,int>> whiteBlobs;
        // loop through the white blobs hoping to find a ball sized blob
        for (auto i =blobber2.blobs.begin(); i!=blobber2.blobs.end(); i++) {
            int count = filterWhiteBlobs((*i), whiteBlobs, blackBlobs);
            int centerY = static_cast<int>((*i).centerY());
            int principalLength = static_cast<int>((*i).firstPrincipalLength());
            if (count > 2) {
                makeBall((*i), cameraHeight, 0.9, foundBall);
#ifdef OFFLINE
                foundBall = true;
#else
                return true;
#endif
            } else if (count > 0 && topCamera &&
                       centerY + principalLength > height - BUFFER &&
                       principalLength < BOTTOMEDGEWHITEMAX) {
                makeBall((*i), cameraHeight, 0.5, foundBall);
#ifdef OFFLINE
                foundBall = true;
#else
                return true;
#endif
            }
        }

        if (findCorrelatedBlackBlobs(blackBlobs, cameraHeight, foundBall)) {
#ifdef OFFLINE
            foundBall = true;
#else
            return true;
#endif
        }

        // TO DO: At this point go back and check for small balls and
        // balls on the edge
        for (auto i =blobber2.blobs.begin(); i!=blobber2.blobs.end(); i++) {
            int centerX = static_cast<int>((*i).centerX());
			int centerY = static_cast<int>((*i).centerY());
			int principalLength = static_cast<int>((*i).firstPrincipalLength());
			int principalLength2 = static_cast<int>((*i).secondPrincipalLength());
            if (topCamera && centerY < height / 2 &&
                principalLength < FARAWAYWHITESIZE &&
                principalLength2 > principalLength / 2 &&
                (*i).area() > MINWHITEAREA &&
                principalLength2 >= 1 &&
                (centerY > field->horizonAt(centerX) || !topCamera)) {
                // could be a ball - or a line, or a field cross or a robot chest
                // sanity checks needed here
                makeBall((*i), cameraHeight, 0.5, foundBall);
#ifdef OFFLINE
                foundBall = true;
#else
                return true;
#endif
            }
        }

		return foundBall;
	}

	void BallDetector::getColor(int x, int y) {
		currentX = x;
		currentY = y;
	}

	bool BallDetector::isGreen() {
		if (*(greenImage.pixelAddr(currentX, currentY)) > 158) {
			return true;
		}
		return false;
	}

	bool BallDetector::isWhite() {
		if (*(whiteImage.pixelAddr(currentX, currentY)) > 88)// &&
			//*(yImage.pixelAddr(currentX, currentY)) < 350) {
			{
			return true;
		}
		return false;
	}

	bool BallDetector::isOrange() {
		if (*(orangeImage.pixelAddr(currentX, currentY)) > 68) {
			return true;
		}
		return false;
	}

	void BallDetector::setImages(ImageLiteU8 white, ImageLiteU8 green, ImageLiteU8 orange,
		ImageLiteU16 yImg) {
		whiteImage = white;
		greenImage = green;
		orangeImage = orange;
		yImage = yImg;
	}


    Ball::Ball(Blob& b, double x_, double y_, double cameraH_, int imgHeight_,
			   int imgWidth_, bool top, bool os, bool ot, bool ob) :
        blob(b),
		radThresh(.3, .7),
		thresh(.5, .8),
		x_rel(x_),
		y_rel(y_),
		cameraH(cameraH_),
		imgHeight(imgHeight_),
		imgWidth(imgWidth_),
		topCamera(top),
		occludedSide(os),
		occludedTop(ot),
		occludedBottom(ob),
		_confidence(0)
	{
		if (!top) {
			//radThresh = thresh;
			FuzzyThr relaxed(.3, .6);
			thresh = relaxed;
		}
		compute();
	}

	Ball::Ball() :
        blob(0),
		thresh(0, 0),
		radThresh(0, 0),
		_confidence(0),
		dist(200000)
	{ }

	void Ball::compute()
	{
		dist = hypot(x_rel, y_rel);
		double density = blob.area() / blob.count();
		double aspectRatio = (blob.secondPrincipalLength() /
                              blob.firstPrincipalLength());

		double hypotDist = hypot(dist, cameraH);

		expectedDiam = pixDiameterFromDist(hypotDist);

		diameterRatio;
		if (expectedDiam > 2 * firstPrincipalLength) {
			diameterRatio = 2 * firstPrincipalLength / expectedDiam;
		} else {
			diameterRatio = expectedDiam / (2 * firstPrincipalLength);
		}

		//_confidence = (density > thresh).f() * (aspectRatio > thresh).f() * (diameterRatio > radThresh).f();

		if ((occludedSide || occludedTop || occludedBottom) && density > 0.9) {
			_confidence = ((density > thresh) & (aspectRatio > thresh) &
						   (diameterRatio > radThresh)).f();
			_confidence = ((density > thresh) &
						   (diameterRatio > radThresh)).f() * 0.85;
		} else {
			_confidence = ((density > thresh) & (aspectRatio > thresh) &
						   (diameterRatio > radThresh)).f();
		}


		// Hack/Sanity check to ensure we don't see crazy balls
		if (dist > 600) {
			_confidence = 0;
		}
	}

	std::string Ball::properties()
	{
		std::string d("====Ball properties:====\n");
		d += "\tRelativePosition: " + to_string(x_rel) + " "+ to_string( y_rel) + "\n";
		d += "\tHomographyDistance is: " + to_string(dist) + "\n";
		//d += "\tdensity is: " + to_string(blob.area() / blob.count()) + "\n";
		//d += "\tcount is: " + to_string(blob.count()) + "\n";
		d += "\tlocated: (" + to_string(centerX) +  ", " +
			to_string(centerY) + ")\n";
		d += "\tprinceLens: " + to_string(firstPrincipalLength) + " " +
			to_string(firstPrincipalLength) + "\n";
		//d += "\taspectR is: " + to_string(blob.secondPrincipalLength() /
		//								  blob.firstPrincipalLength()) + "\n";
		d += "\texpect ball to be this diam: " + to_string(expectedDiam) + "\n";
		d += "\tdiamRatio: " + to_string(diameterRatio) + "\n";
		d += "\tdiam Confidence: " + to_string((diameterRatio> radThresh).f()) + "\n";
		d += "\n\tconfidence is: " + to_string(_confidence) + "\n====================\n";
		return d;
	}

    // The expected diameter of ball in image at distance d in CM
	double Ball::pixDiameterFromDist(double d) const
	{
		double trig = atan(BALL_RADIUS / d);
		return 2 * imgHeight * trig / VERT_FOV_RAD;
	}



}
}
