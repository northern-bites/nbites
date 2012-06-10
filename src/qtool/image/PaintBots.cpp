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
QColor playerColor; QPen pen;

PaintBots::PaintBots(QObject *parent, float sF):
	BMPImage(parent)
{
	scaleFactor = 1.25;
	bitmap = QPixmap(FIELD_WIDTH*scaleFactor, FIELD_HEIGHT*scaleFactor);
	locs = new BotLocs();
}


void PaintBots::buildBitmap()
{
	bitmap.fill(Qt::transparent);
	QPainter painter(&bitmap);
	painter.translate(0, FIELD_HEIGHT);
	painter.scale(1*scaleFactor, -1*scaleFactor);

	for(int i=1; i < locs->getSize(); i++) { //the first bot is a placeholder
		//set pen/brush for correct team
		if(locs->getTeam(i)==0){
			playerColor = bluePlayer;
		} else {
			playerColor = redPlayer;
		}

		painter.setBrush(playerColor);
		painter.setPen(playerColor);
		pen = QPen(playerColor);

		float heading = locs->getHeading(i)*(float)(PI/180);
		float hTopUncert = heading + locs->getheadUncert(i)*(float)(PI/180)/2;
		float hBotUncert = heading - locs->getheadUncert(i)*(float)(PI/180)/2;
		QPoint robotPt = QPoint(locs->getX(i), locs->getY(i));
		QPoint ballPt = QPoint(locs->getBallX(i), locs->getBallY(i));
		QPoint headingLine = QPoint(locs->getX(i)+40*cos(heading),
									locs->getY(i)+40*sin(heading));
		QPoint hUncertL1 = QPoint(locs->getX(i)+30*cos(hTopUncert),
								  locs->getY(i)+30*sin(hTopUncert));
		QPoint hUncertL2 = QPoint(locs->getX(i)+30*cos(hBotUncert),
								  locs->getY(i)+30*sin(hBotUncert));

		//robot
		painter.drawEllipse(robotPt, 10, 10);

		//robot uncertainty
		painter.setBrush(Qt::NoBrush);
		painter.drawEllipse(robotPt, locs->getXUncert(i), locs->getYUncert(i));

		//heading
		pen.setWidth(5);
		painter.setPen(pen);
		painter.drawLine(robotPt, headingLine);

		//heading uncertainty
		pen.setWidth(2);
		painter.setPen(pen);
		painter.drawLine(robotPt, hUncertL1);
		painter.drawLine(robotPt, hUncertL2);

		//don't draw ball if robot thinks its distance is 0)
		if(robotPt!=ballPt){
			//ball
			painter.setPen(ball);
			painter.setBrush(ball);
			painter.drawEllipse(ballPt, 5, 5);

			//ball uncertainty
			painter.setBrush(Qt::NoBrush);
			painter.drawEllipse(ballPt, locs->getBallXUncert(i), locs->getBallYUncert(i));

			//robot-ball line
			painter.setPen(Qt::DashLine);
			painter.drawLine(ballPt, robotPt);
		}

	}
}

}
}

