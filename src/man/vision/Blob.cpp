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
 * The blob is the primary data structure of run-length encoding.
 * Basically we just hold the four corner points and return them
 * as necessary.  Sometimes we calculate height and width, but it
 * is important to note that in these circumstances we do not
 * attempt to account for the the orientation of the blob.
 */

#include <iostream>
#include "Blob.h"

using namespace std;

Blob::Blob() {
    init();
}

void Blob::init() {
    setLeftTopX(0);
    setLeftTopY(0);
    setRightTopX(0);
    setRightTopY(0);
    setLeftBottomX(0);
    setLeftBottomY(0);
    setRightBottomX(0);
    setRightBottomY(0);
    setArea(0);
    setPixels(0);
}

int Blob::getArea() const{
    return width() * height();
}

int Blob::width() const{
    return rightTop.x - leftTop.x + 1;
}

int Blob::height() const{
    return leftBottom.y - leftTop.y + 1;
}

void Blob::merge(Blob other) {
    int value = min(leftTop.x, other.leftTop.x);
    leftTop.x = value;
    leftBottom.x = value;
    value = max(rightTop.x, other.rightTop.x);
    rightTop.x = value;
    rightBottom.x = value;
    value = min(leftTop.y, other.leftTop.y);
    leftTop.y = value;
    rightTop.y = value;
    value = max(leftBottom.y, other.leftBottom.y);
    leftBottom.y = value;
    rightBottom.y = value;
}

/* Print debugging information for a blob.
 */
void Blob::printBlob() const {
#if defined OFFLINE
    cout << "Outputting blob" << endl;
    cout << leftTop.x << " " << leftTop.y << " " << rightTop.x << " "
         << rightTop.y << endl;
    cout << leftBottom.x << " " << leftBottom.y << " " << rightBottom.x
         << " " << rightBottom.y << endl;
#endif
}




