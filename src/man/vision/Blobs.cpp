// This file is part of Man, a robotic perception, locomotion, and
// team strategy application created by the Northern Bites RoboCup
// team of Bowdoin College in Brunswick, Maine, for the Aldebaran
// Nao robot.
//
// Man is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// Man is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser Public License for more details.
//
// You should have received a copy of the GNU General Public License
// and the GNU Lesser Public License along with Man.  If not, see
// <http://www.gnu.org/licenses/>.


/*
*/

#include <iostream>
#include "Blob.h"
#include "Blobs.h"

//using namespace std;

Blobs::Blobs(int howMany) {
	total = howMany;
	blobs = (Blob*)malloc(sizeof(Blob) * howMany);
	for (int i = 0; i < howMany; i++) {
		blobs[i] = Blob();
	}
	init();
}


void Blobs::init() {
	for (int i = 0; i < total; i++) {
		blobs[i].init();
	}
	numBlobs = 0;
}

void Blobs::setLeft(int i, int x) {
	blobs[i].setLeftTopX(x);
	blobs[i].setLeftBottomX(x);
}

void Blobs::setRight(int i, int x) {
	blobs[i].setRightTopX(x);
	blobs[i].setRightBottomX(x);
}

void Blobs::setTop(int i, int y) {
	blobs[i].setLeftTopY(y);
	blobs[i].setRightTopY(y);
}

void Blobs::setBottom(int i, int y) {
	blobs[i].setLeftBottomY(y);
	blobs[i].setRightBottomY(y);
}

/*
 * Pseudo-blobbing used for goal recognition.  Since the goals are rectangles we
 * should be able to just paste the new runs in to a main blob directly.
 * This uses the huge hack that our objects (except the ball) are square - so we
 * can just keep a bounding box.
 *
 * Basically we are collecting runs.  Everytime we get one we either add it to
 * an existing run or create a new run. In theory we can fragment runs this way.
 * In fact, we should probably check on that.
 *
 * @param x        x value of run
 * @param y        y value of run
 * @param h        height of run
*/
void Blobs::blobIt(int x, int y, int h, bool merge)
{
    const int WHAT_IS_CONTIGUOUS = 4;  // fudge factor for juding contiguity
    const int MIN_BLOB_SIZE = 20;      // number of blobs before changing standard

    // start out deciding to make a new blob
    // the loop will decide not to run it
    bool newBlob = true;
    int contig = WHAT_IS_CONTIGUOUS;

    //cout << x << " " << y << " " << h << endl;
    // sanity check: too many blobs on screen
    if (numBlobs >= total) {
        //cout << "Ran out of blob space " << color << endl;
        // We're seeing too many blobs -it is unlikely we can do anything
        // useful with this color
        numBlobs = 0;
        numberOfRuns = 0;
        return;
    } else if (numBlobs > MIN_BLOB_SIZE) {
        contig = WHAT_IS_CONTIGUOUS;
    }

    // is this run contiguous with any previous blob?
    for (int i = 0; i < numBlobs; i++) {

        // first check: if currentBlob x is greater than blob left and less than
        // a little bit more than the blob right.
        // AND
        // second check: currentBlob y is within fits within current blob
        // OR
        // currentBlob's bottom is within blob and height makes it higher
        if ((x > blobs[i].getLeftTopX()  && x < blobs[i].getRightTopX()
			 + contig) && ((y >= blobs[i].getLeftTopY() - contig &&
							y < blobs[i].getLeftBottomY() + contig) ||
						   (y < blobs[i].getLeftTopY() &&
							y+h+contig > blobs[i].getLeftTopY()))) {

            /* BOUNDING BOX CHECKS
             * if current x or y increases the size of the box, do so and keep
             * track of the corresponding x or y value
             */
            //assign the right, if it is better
            if (x > blobs[i].getRightTopX()) {
                blobs[i].setRightTopX(x);
                blobs[i].setRightBottomX(x);
            }

            //assign the top, if it is better
            if (blobs[i].getLeftTopY() > y) {
                blobs[i].setLeftTopY(y);
                blobs[i].setRightTopY(y);
            }

            // assign the bottom, if it is better
            if (y+h > blobs[i].getLeftBottomY()) {
                blobs[i].setLeftBottomY(y+h);
                blobs[i].setRightBottomY(y + h);
            }

            //add the run length to the number of real pixels in the blob
            //calculate the area of this blob under consideration
            int s = (blobs[i].getRightTopX() - blobs[i].getLeftTopX() + 1) *
                (blobs[i].getLeftBottomY() - blobs[i].getLeftTopY() + 1);
            blobs[i].setArea(s); //store the area for later.
            blobs[i].setPixels(blobs[i].getPixels() + h);

            // don't create a blob
            newBlob = false;
            // sometimes blobs that weren't contiguous before may be now
            if (merge) {
                checkForMergers();
            }
            break;
        }
        // no else
    } // END blob for loop

    // create newBlob
    if (newBlob) {
        // bounding box
        blobs[numBlobs].setLeftTopX(x);
        blobs[numBlobs].setLeftTopY(y);
        blobs[numBlobs].setRightTopX(x);
        blobs[numBlobs].setRightTopY(y);
        blobs[numBlobs].setLeftBottomX(x);
        blobs[numBlobs].setLeftBottomY(y + h);
        blobs[numBlobs].setRightBottomX(x);
        blobs[numBlobs].setRightBottomY(y + h);
        blobs[numBlobs].setPixels(h);
        blobs[numBlobs].setArea(h);
        numBlobs++;
    }
}

//adds a blob to the blobs
void Blobs::add(Blob blob) {

    // sanity check: too many blobs on screen
    if (numBlobs >= total) {
        //cout << "Ran out of blob space " << color << endl;
        // We're seeing too many blobs -it is unlikely we can do anything
        // useful with this color
        numBlobs = 0;
        numberOfRuns = 0;
        return;
    }

    blobs[numBlobs] = blob;
    numBlobs++;
}

/* Checks if any of our blobs are overlapping.  If so, then merge then and
   eliminate one of them.
 */
void Blobs::checkForMergers()
{
    for (int i = 1; i < numBlobs; i++) {
        for (int j = 0; j < i; j++) {
            if (blobsOverlap(j, i)) {
                mergeBlobs(i, j);
            }
        }
    }
}

/* Do two blobs overlap?  Return true if they do.
 */
bool Blobs::blobsOverlap(int first, int second)
{
    const int SKIP = 4;
    if (blobs[first].getLeft() < blobs[second].getLeft()) {
        if (blobs[second].getLeft() > blobs[first].getRight() + SKIP) {
            return false;
        }
    } else if (blobs[first].getLeft() > blobs[second].getRight() + SKIP) {
        return false;
    }
    // at this point we know they overlap in the x direction
    if (blobs[first].getTop() < blobs[second].getTop()) {
        if (blobs[second].getTop() > blobs[first].getBottom() + SKIP) {
            return false;
        } else {
            return true;
        }
    }
    if (blobs[first].getTop() > blobs[second].getBottom() + SKIP) {
        return false;
    }
    return true;
}

/*
 * Find the biggest blob.  Ideally this will end up also merging blobs when they
 * are occluded (e.g. by a dog).
 * It may not be necessary though.
 * @param maxY     max value (ignored)
 * @return         a pointer to the biggest blob
*/
Blob* Blobs::getTopAndMerge(int maxY)
{
    Blob* topBlob = NULL;
    int size = 0;
    //check each blob in the array
    for (int i = 0; i < numBlobs; i++) {
        if (blobs[i].getArea() > size) {
            size = blobs[i].getArea();
            topBlob = &blobs[i];
        }
    }
	return topBlob;
}

/*
  Checks all of the blobs of this color.  Can be used to draw the widest blob.
*/
Blob* Blobs::getWidest()
{
    Blob* topBlob = NULL;
    int size = 0;
    int width = 0;
    //check each blob in the array
    for (int i = 0; i < numBlobs; i++) {
        width = blobs[i].width();
        if (width > size) {
            size = width;
            topBlob = &blobs[i];
        }
    }
	return topBlob;
}

/*
  Checks all of the blobs of this color and finds the biggest one.
  returns its index.
*/
int Blobs::getBiggest()
{
    int index = 0;
    int size = 0;
    //check each blob in the array
    for (int i = 0; i < numBlobs; i++) {
        if (blobs[i].getArea() > size) {
            size = blobs[i].getArea();
            index = i;
        }
    }
    return index;
}

/*
  Checks all of the blobs of this color and finds the biggest one.
  returns its index. Only checks blobs after index i
*/
int Blobs::getBiggest(int i)
{
    int index = i;
    int size = 0;
    //check each blob in the array
    for (i; i < numBlobs; i++) {
        if (blobs[i].getArea() > size) {
            size = blobs[i].getArea();
            index = i;
        }
    }
    return index;
}


/* Turn a blob back to zeros because of merging.
   @param which     The index of the blob to be zeroed
*/

void Blobs::zeroTheBlob(int which)
{
	blobs[which].init();
    blobs[which].setLeftTopX(BADONE);
}

/* Merge blobs.  If two blobs are discovered to be connected, then they
   should be merged into one.  This is done here.
   @param first         one of the blobs
   @param second        the other
*/
void Blobs::mergeBlobs(int first, int second)
{
	blobs[first].merge(blobs[second]);
	zeroTheBlob(second);
}

/* Sort blobs in descending order using selection sort.
   NOTE: if efficiency is an issue this sort can be improved
*/
void Blobs::sort() {
    for (int i = 0; i < number(); i++){
        Blob temp = blobs[i];
        int biggest = getBiggest(i);
        blobs[i] = blobs[biggest];
        blobs[biggest] = temp;
    }
}
