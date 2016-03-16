
#include "BallDetector.h"

#include <algorithm>
#include <iostream>


using std::to_string;

namespace man {
namespace vision {

	BallDetector::BallDetector(FieldHomography* homography_,
							   Field* field_, bool topCamera_):
		blobber(),
		homography(homography_),
		field(field_),
		topCamera(topCamera_)
	{
		blobber.secondThreshold(115);
		blobber.minWeight(4);
	}

	BallDetector::~BallDetector() { }

	void BallDetector::setDebugImage(DebugImage * di) {
		debugDraw =  *di;
	}

    bool BallDetector::bottomCameraCheck(int left, int right, int top, int bottom)
    {
        int BOTTOM_MIN = 20;
        int BUFFER = 3;

        int ballWidth = right - left;
        int ballHeight = bottom - top;
        if (ballWidth < BOTTOM_MIN && left > BUFFER && right < width - BUFFER) {
            if (debugBall) {
                std::cout << "Width too small " << ballWidth << std::endl;
            }
            return false;
        }
        if (ballHeight < BOTTOM_MIN && top > BUFFER) {
            if (debugBall) {
                std::cout << "Height too small " << ballHeight << std::endl;
            }
            return false;
        }
    }

    bool BallDetector::topCameraCheck(int left, int right, int top, int bottom)
    {
        int TOP_MIN = 10;
        int BUFFER = 3;
        int farHorizon = 100;
        int farDim = 20;
        int midHorizon = 180;
        int midDim = 30;

        int ballWidth = right - left;
        int ballHeight = bottom - top;
        int minDim = min(ballWidth, ballHeight);
        if (ballWidth < TOP_MIN && left > BUFFER && right < width - BUFFER) {
            if (debugBall) {
                std::cout << "Width too small " << ballWidth << std::endl;
            }
            return false;
        }
        if (ballHeight < TOP_MIN && top > BUFFER) {
            if (debugBall) {
                std::cout << "Height too small " << ballHeight << std::endl;
            }
            return false;
        }
        if (bottom < farHorizon) {
            if (ballWidth > farDim || ballHeight > farDim) {
                if (debugBall) {
                    std::cout << "Dangerous far ball with large dimension " <<
                        ballWidth << " " << ballHeight << std::endl;
                }
                return false;
            }
        } else if (bottom < midHorizon) {
            if (minDim > midDim) {
                if (debugBall) {
                    std::cout << "Dangerous mid ball with large dimensions " <<
                        ballWidth << " " << ballHeight << std::endl;
                }
                return false;
            }
        }
    }

	bool BallDetector::findBallWithEdges(ImageLiteU8 white, EdgeList& edges)
	{
		Ball reset;
		_best = reset;
		width = white.width();
		height = white.height();

		int edgeList[width];
		for (int i = 0; i < width; i++) {
			edgeList[i] = 0;
		}
		// Get edges from vision
		AngleBinsIterator<Edge> abi(edges);
        int yMax = 0;
		for (Edge* e = *abi; e; e = *++abi){
			// If we are part of a hough line, we are not a ball edge
			if (e->memberOf()) { continue; }

			int x = e->x() + width / 2;
			int y = height / 2 - e->y();
			int ang = e->angle();
			edgeList[x] += 1;
			// if we're off the field we aren't a ball
			if (topCamera && y < field->blockHorizonAt(x)) { continue; }
			// we're only going to look at downward angles
			if ((ang < 128 && topCamera) || (ang > 128 && !topCamera)) { continue; }
			//int mag = e->mag();      // magnitude - could be useful later?
			getColor(x, y+1);
			int others = 0;
			int total = 0;
			int whites = 0;
			int i;
			yMax = 0;
			int yMin = 500;
			getColor(x, y);
			int previousY = *(yImage.pixelAddr(currentX, currentY)) / 4;
			int previous2 = previousY;
			int firstBreak = -1; // track the first place we saw a big Y change
			// scan down from the edge until we hit green
            int direction = 1;
            if (!topCamera) {
                direction = -1;
            }
			for (i = y+(2*direction); i < height && i > 0 && !isGreen(); i+=direction) {
				getColor(x, i);
				if (isGreen() && !isOrange()) {
					break;
				}
				// we're going to track the biggest and smallest Y values we've seen
				int yValue = *(yImage.pixelAddr(currentX, currentY)) / 4;
				yMin = min(yValue, yMin);
				yMax = max(yValue, yMax);
				if (previous2 - yValue > 100) {
					if (firstBreak == -1) {
						firstBreak = i;
					}
				}
                int yThresh = 35;
                if (yMax > 160) {
                    yThresh = 60;
                }
                if (yValue < yMax - yThresh) {
                    //debugDraw.drawDot(x, i, RED);
                }
				previous2 = previousY;
				previousY = yValue;
				total++;
				if (!isWhite() || isOrange() || yValue < yMax - yThresh) {
					others++;
				}
				if (isWhite()) {
					whites++;
				}
			}
			if (firstBreak != -1 && i - firstBreak < 3) {
				firstBreak = -1;
			} else if (firstBreak != -1) {
				//debugDraw.drawPoint(x, firstBreak, RED);
			}

			if (debugBall) {
				// if various things are true, then do more checking
				// saw some black, some some white, saw a range of Y
				if (firstBreak != -1 ||
					(others > (total / 5) && total > 10 && total < 50 && whites > 5 && yMin < 100)) {
					//std::cout << "Spread is " << yMin << " " << yMax << std::endl;
					//debugDraw.drawPoint(x, y, BLUE);
					if (testForBall(x, y, i, yMax)) {
                        debugDraw.drawPoint(x, y, RED);
						return true;
					} else {
                        debugDraw.drawPoint(x, y, BLACK);
                    }
				} else if (total > 10) {
					//std::cout << "Missed " << others << " " << whites << " " << yMin << std::endl;
					/*if (yMin < 100 && whites > 5) {
						if (testForBall(x, y, i, yMax)) {
                            debugDraw.drawPoint(x, y, GREEN);
							return true;
						}
                        else {
                            debugDraw.drawPoint(x, y, BLUE);
                        }
                        }*/
				} else {
					if (ang > 192) {
						//debugDraw.drawPoint(x, y, BLACK);
					} else {
						//debugDraw.drawPoint(x, y, BLUE);
					}
				}
			}
		}

		return false;
	}

    bool BallDetector::testForBall(int x, int top, int bottom, int yMax) {
        if (top > bottom) {
            int swap = top;
            top = bottom;
            bottom = swap;
        }
		int mid = (bottom + top) / 2;
		getColor(x, mid);
		int right = x;
		int whites = 0;
		for ( ; right < width && !isGreen(); right++) {
			getColor(right, mid);
			if (isWhite()) {
				whites++;
			}
		}
		int left = x;
		getColor(left, mid);
		for ( ; left >= 0 && !isGreen(); left--) {
			getColor(left, mid);
			if (isWhite()) {
				whites++;
			}
		}
		if (whites < (left - right) / 3) {
			if (debugBall) {
				std::cout << "Not enought horizontal white " << whites << " " <<
					(right - left) << std::endl;
			}
			return false;
		}
		int midX = (left + right) / 2;
		whites = bottom - top - 1;
		getColor(midX, top);
		for ( ; top > 0 && !isGreen(); top--) {
			getColor(midX, top);
			if (isWhite()) {
				whites++;
			}
		}
		getColor(midX, bottom);
		for ( ; bottom < height && !isGreen(); bottom++) {
			getColor(midX, bottom);
			if (isWhite()) {
				whites++;
			}
		}
		if (whites < (bottom - top) / 3) {
			if (debugBall) {
				std::cout << "Not enought vertical white " << whites << " " <<
					(bottom - top) << std::endl;
			}
			return false;
		}
        if (!topCamera) {
            if (!bottomCameraCheck(left, right, top, bottom)) {
                if (debugBall) {
                    std::cout << "Too small for bottom camera ball " << std::endl;
                }
                return false;
            }
        } else if (!topCameraCheck(left, right, top, bottom)) {
            return false;
        }
		mid = (bottom + top) / 2;
        if (topCamera && mid < field->horizonAt(midX)) {
            debugDraw.drawPoint(midX, mid, BLUE);
            std::cout << "Mid point off field" << std::endl;
            return false;
        }

		// now scan the diagonals
		whites = 0;
		int diaglength1 = 0;
		getColor(midX, mid);
		for (int i = 0; !isGreen(); i++) {
			if (midX + i > width || mid + i > height) {
				break;
			}
			getColor(midX + i, mid + i);
			if (isWhite()) {
				whites++;
			}
			diaglength1++;
		}
		getColor(midX, mid);
		for (int i = 0; !isGreen(); i++) {
			if (midX - i < 0 || mid - i < 0) {
				break;
			}
			getColor(midX - i, mid - i);
			if (isWhite()) {
				whites++;
			}
			diaglength1++;
		}
		if (whites < min(right - left, bottom - top) / 3) {
			if (debugBall) {
				std::cout << "Not enought diagonal white " << whites << " " <<
					(bottom - top) << std::endl;
			}
			return false;
		}
		// now scan the diagonals
		whites = 0;
		int diaglength2 = 0;
		getColor(midX, mid);
		for (int i = 0; !isGreen(); i++) {
			if (midX + i > width || mid - i < 0) {
				break;
			}
			getColor(midX + i, mid - i);
			if (isWhite()) {
				whites++;
			}
			diaglength2++;
		}
		getColor(midX, mid);
		for (int i = 0; !isGreen(); i++) {
			if (midX - i < 0 || mid + i > height) {
				break;
			}
			getColor(midX - i, mid + i);
			if (isWhite()) {
				whites++;
			}
			diaglength2++;
		}
		if (whites < min(right - left, bottom - top) / 3) {
			if (debugBall) {
				std::cout << "Not enought left diagonal white " << whites << " " <<
					(bottom - top) << std::endl;
			}
			return false;
		}
		int firstPrincipalLength = min(bottom - top, right - left);
		int maxLength = max(bottom - top, right - left);
		if (firstPrincipalLength > 55 || (firstPrincipalLength > 35 && !topCamera)) {
			return false;
		}
        if (firstPrincipalLength < 10 ||
            (firstPrincipalLength < 25 && bottom > 100 && bottom < height- 5)) {
            return false;
        }
		/*if (diaglength1 > 2 * firstPrincipalLength || diaglength2 > 2 * firstPrincipalLength) {
			// probably a line, just make sure everything else is fine
			if (maxLength > 1.5 * firstPrincipalLength) {
				std::cout << "two things too long " << maxLength << " " << firstPrincipalLength <<
					" " << midX << " " << mid << std::endl;
                debugDraw.drawDot(midX, mid, BLUE);
				return false;
			}
            }*/
		// check a square inside the ball, there ought to be black in there
		int offset = (right - left) / 4;
		int offsetY = (bottom - top) / 4;
		if (offset > 2 * offsetY) {
			offset = offsetY;
		} else if (offsetY > 2 * offset) {
			offsetY = offset;
		}
		int blacks = 0;
		int greens = 0;
		for (int i = -offset; i <= offset; i++) {
			for (int j = -offsetY; j <= offsetY; j++) {
				getColor(i + midX, j + mid);
				// this needs to be replaced by a true black test
				int yValue = *(yImage.pixelAddr(i + midX, j + mid)) / 4;
                int yThresh = 35;
                if (yMax > 160) {
                    yThresh = 55;
                }
				if (!isWhite() && !isGreen() || (yValue < yMax - yThresh && yValue < 110)) {
					debugDraw.drawDot(i + midX, j + mid, RED);
					blacks++;
				}
				if (isGreen()) {
					greens++;
				}
			}
		}
		if ((blacks < offset + offsetY && offset > 3) || greens > 3 || blacks == 0) {
			if (debugBall) {
                debugDraw.drawBox(midX - offset, midX + offset, mid + offsetY, mid - offsetY, BLUE);
				std::cout << "Too white or too green " << blacks << " " << greens << std::endl;
			}
			return false;
		}
        if (blacks > (offset * offsetY)) {
			if (debugBall) {
                debugDraw.drawBox(midX - offset, midX + offset, mid + offsetY, mid - offsetY, BLUE);
				std::cout << "Too much black " << blacks << " " << (offset * offsetY) << std::endl;
			}
            return false;
        }
		if (debugBall) {
            debugDraw.drawBox(midX - offset, midX + offset, mid + offsetY, mid - offsetY, BLACK);
			debugDraw.drawLine(left, mid, right, mid, BLACK);
			debugDraw.drawLine(midX, top, midX, bottom, BLACK);
			std::cout << "Ball at " << midX << " " << mid << " width " <<
				(right - left) << " height " << (bottom - top) << std::endl;
			std::cout << diaglength1 << " " << diaglength2 << " " << blacks << std::endl;
		}
		int centerX = midX;
		int centerY = mid;
		double x_rel, y_rel;

		double bIX = (centerX - width/2);
		double bIY = (height / 2 - centerY) - firstPrincipalLength;

		Ball b(x_rel, -1 * y_rel, 0, height,
			   width, topCamera, false, false,
			   false);
		b.centerX = centerX;
		b.centerY = centerY;
		b.firstPrincipalLength = firstPrincipalLength;
		b._confidence = 0.9;
		_best = b;
		return true;
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


	bool BallDetector::findBall(ImageLiteU8 orange, double cameraHeight)
	{
		const double CONFIDENCE = 0.5;

		if (debugBall) {
			candidates.clear();
		}

		blobber.run(orange.pixelAddr(), orange.width(), orange.height(),
					orange.pitch());

		Ball reset;
		_best = reset;
		width = orange.width();
		height = orange.height();

		// TODO: Sort blobber list by size
		for (auto i=blobber.blobs.begin(); i!=blobber.blobs.end(); i++) {
			int centerX = static_cast<int>((*i).centerX());
			int centerY = static_cast<int>((*i).centerY());
			int principalLength = static_cast<int>((*i).firstPrincipalLength());
			int principalLength2 = static_cast<int>((*i).secondPrincipalLength());
			bool occludedSide = false;
			bool occludedTop = false;
			bool occludedBottom = false;

			double x_rel, y_rel;

			double bIX = ((*i).centerX() - width/2);
			double bIY = (height / 2 - (*i).centerY()) -
				(*i).firstPrincipalLength();

			bool belowHoriz = homography->fieldCoords(bIX, bIY, x_rel, y_rel);

			if (preScreen(centerX, centerY, principalLength, principalLength2,
						  occludedSide, occludedTop, occludedBottom)) {
				continue;
			}

			Ball b(x_rel, -1 * y_rel, cameraHeight, height,
				   width, topCamera, occludedSide, occludedTop,
				   occludedBottom);

			if (b.confidence() > CONFIDENCE) {
#ifdef OFFLINE
				// we always want to draw the ball, even when not debugging it
				candidates.push_back(b);
#endif
				if (debugBall) {
					std::cout << "accepted ball at: " << centerX << " " <<
						centerY << " because:\n" << b.properties()
							  << std::endl;
				}
				if (b.dist < _best.dist) {
					_best = b;
				}
			}
			else {
				if (debugBall) {
					std::cout << "declined ball at " << centerX << " " <<
						centerY << " because:\n" << b.properties()
							  << std::endl;
				}
			}
		}
		if (_best.confidence() > CONFIDENCE) {
			return true;
		}
		else {
			return false;
		}
	}

	/* Before we evaluate a ball check to see if there are obvious problems
	 * with it - e.g. it isn't on the field.
	 */
	bool BallDetector::preScreen(int centerX, int centerY, int principalLength,
							   int principalLength2,
							   bool & occludedSide, bool & occludedTop,
							   bool & occludedBottom) {
		// When looking in the top camera worry about the field
		if (topCamera) {
			bool offField = centerY + principalLength < field->horizonAt(centerX);
			if (offField) {
				if (debugBall) {
					std::cout << "Blob is off the field: " << std::endl;
				}
				return true;
			}
			offField = centerY + 2 * principalLength + 3 <
									 field->blockHorizonAt(centerX);
			if (offField) {
				if (debugBall) {
					std::cout << "Blob is on blocked part of field " <<
						"and is probably a robot uniform" << std::endl;
				}
				return true;
			}
		}

		// is the ball occluded?
		if (centerX - principalLength < 2 ||
			centerX + principalLength > width -2) {
			if (debugBall) {
				std::cout << "Blob is occluded on side" << std::endl;
			}
			occludedSide = true;
		}

		if (centerY - principalLength < 0) {
			if (debugBall) {
				std::cout << "Blob is occluded on top" << std::endl;
			}
			occludedTop = true;
		}
		if (centerY + principalLength > height - 2) {
			if (debugBall) {
				std::cout << "Blob is occluded on bottom" << std::endl;
			}
			occludedBottom = true;
		}
		// Note: this test is for bottom camera, occluded top could mean
		// a facing red robot. If ball is actually occluded on top it should
		// show up in the top camera
		/*bool occluded = occludedSide || occludedBottom;

		if (!topCamera && !occluded && principalLength2 < height / 20) {
			if (debugBall) {
				std::cout << "Blob on bottom is too thin " <<
					principalLength2 << " " << principalLength << std::endl;
			}
			return true;
			}*/
		return false;
	}

	Ball::Ball(double x_, double y_, double cameraH_, int imgHeight_,
			   int imgWidth_, bool top, bool os, bool ot, bool ob) :
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
		thresh(0, 0),
		radThresh(0, 0),
		_confidence(0),
		dist(200000)
	{ }

	void Ball::compute()
	{
		dist = hypot(x_rel, y_rel);
		double density = 1.0; //blob.area() / blob.count();
		double aspectRatio = 1.0; //(blob.secondPrincipalLength() /
		//blob.firstPrincipalLength());

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
