#include "PaintBots.h"
#include "viewer/FieldViewer.h"
#include "BMPImage.h"
#include <vector>
#include "image/Color.h"
#include "viewer/BotLocs.h"
#include <cmath>

namespace qtool {
namespace image {

using namespace viewer;

QColor ball = QColor::fromRgb(Color_RGB[Orange]);
QColor bluePlayer = QColor::fromRgb(Color_RGB[Navy]);
QColor redPlayer = QColor::fromRgb(Color_RGB[Red]);
QColor grey = QColor::fromRgb(Color_RGB[Grey]);

int robotDrawSize = 7;

int headingLineWidth = 3;
int headingLineLength = 20;
int headingUncertLineWidth = 1;

int ballDrawSize = 5;

PaintBots::PaintBots(float scale, QObject *parent)
    : PaintFieldOverlay(scale, parent)
{
    locs = new BotLocs(this);
}


void PaintBots::buildBitmap()
{
    bitmap.fill(Qt::transparent);
    QPainter painter(&bitmap);

    this->transformPainterToFieldCoordinates(painter);

    for(int i = 0; i < locs->getSize(); i++) {
        //set pen/brush for correct team
        QColor playerColor;
        if(locs->getTeam(i) == 0){
            playerColor = bluePlayer;
        } else {
            playerColor = redPlayer;
        }

        QPoint robotPt = QPoint(locs->getX(i), locs->getY(i));
        QPoint ballPt = QPoint(locs->getBallX(i), locs->getBallY(i));

        //robot
        this->paintDot(painter, playerColor, robotPt);

        //robot uncertainty
        this->paintEllipseArea(painter, playerColor, robotPt,
                               locs->getXUncert(i), locs->getYUncert(i));

        //heading
        this->paintPolarLine(painter, playerColor, headingLineWidth,
                             robotPt, headingLineLength, locs->getHeading(i));

        //heading uncertainty
        this->paintPolarLine(painter, playerColor, headingUncertLineWidth,
                             robotPt, headingLineLength, locs->getHeading(i) + locs->getheadUncert(i)/2);
        this->paintPolarLine(painter, playerColor, headingUncertLineWidth,
                             robotPt, headingLineLength, locs->getHeading(i) - locs->getheadUncert(i)/2);

        //don't draw ball if robot thinks its distance is 0)
        if(robotPt != ballPt){
            //ball
            this->paintDot(painter, ball, ballPt, ballDrawSize);

            //ball uncertainty
            this->paintEllipseArea(painter, ball, ballPt,
                                   locs->getBallXUncert(i), locs->getBallYUncert(i));

            //robot-ball line
            painter.setPen(Qt::DashLine);
            painter.drawLine(ballPt, robotPt);
        }

    }
}

}
}

