#include "PaintField.h"
#include <vector>

namespace qtool {
namespace viewer {

using namespace data;
using namespace man::memory;

PaintField::PaintField(QWidget *parent): 
		   QWidget(parent)
{

  setBackgroundRole(QPalette::Base);
  setAutoFillBackground(true);

}

QSize PaintField::sizeHint() const
{
  return QSize(800, 600);
}

QSize PaintField::minimumSizeHint() const
{
  return QSize(100, 100);
}

void PaintField::ballDataChanged(const QBrush &brush)
{
  update(); // updates the painting
}

void PaintField::paintEvent(QPaintEvent * /* event */)
{
		      
     QRect field(FIELD_GREEN_LEFT_SIDELINE_X, 
		 FIELD_GREEN_BOTTOM_SIDELINE_Y, 
		 FIELD_GREEN_WIDTH, FIELD_GREEN_HEIGHT);
     QRect playArea(FIELD_WHITE_LEFT_SIDELINE_X, 
		    FIELD_WHITE_BOTTOM_SIDELINE_Y,
		    FIELD_WHITE_WIDTH, FIELD_WHITE_HEIGHT);
     QRect blueGoalBox(FIELD_WHITE_LEFT_SIDELINE_X, 
		       MIDFIELD_Y -(GOALBOX_WIDTH/2), 
		       GOALBOX_DEPTH, GOALBOX_WIDTH);
     QRect yellowGoalBox(FIELD_WHITE_RIGHT_SIDELINE_X - 
			 (GOALBOX_DEPTH), 
			 MIDFIELD_Y - (GOALBOX_WIDTH/2), 
			 GOALBOX_DEPTH, GOALBOX_WIDTH);
     QRect blueGoal(LANDMARK_BLUE_GOAL_BOTTOM_POST_X - 
		    GOAL_DEPTH, 
		    LANDMARK_BLUE_GOAL_BOTTOM_POST_Y, 
		    GOAL_DEPTH, 
		    CROSSBAR_CM_WIDTH);
     QRect yellowGoal(LANDMARK_YELLOW_GOAL_BOTTOM_POST_X, 
		      LANDMARK_YELLOW_GOAL_BOTTOM_POST_Y, 
		      GOAL_DEPTH, CROSSBAR_CM_WIDTH);

     QPainter painter(this);
     painter.fillRect(field, Qt::darkGreen);
     
     QPen pen(Qt::white, LINE_WIDTH, Qt::SolidLine, 
	      Qt::SquareCap, Qt::MiterJoin);
     painter.setBrush(Qt::NoBrush);
     painter.setPen(pen);
     painter.drawRect(blueGoalBox);
     painter.drawRect(yellowGoalBox);
     painter.drawRect(playArea);
     painter.drawLine(((playArea.topLeft()+
			playArea.topRight())/2),
		      ((playArea.bottomLeft()+
			playArea.bottomRight())/2));

     QPoint centerField = QPoint(CENTER_FIELD_X, CENTER_FIELD_Y);
     painter.drawEllipse (centerField, 
			  CENTER_CIRCLE_RADIUS, 
			  CENTER_CIRCLE_RADIUS);
     
     painter.drawLine(LANDMARK_BLUE_GOAL_CROSS_X - 
		      (LINE_CROSS_LENGTH/2), 
		      LANDMARK_BLUE_GOAL_CROSS_Y,
		      LANDMARK_BLUE_GOAL_CROSS_X + 
		      (LINE_CROSS_LENGTH/2), 
		      LANDMARK_BLUE_GOAL_CROSS_Y);
     painter.drawLine(LANDMARK_YELLOW_GOAL_CROSS_X - 
		      (LINE_CROSS_LENGTH/2), 
		      LANDMARK_YELLOW_GOAL_CROSS_Y,
		      LANDMARK_YELLOW_GOAL_CROSS_X + 
		      (LINE_CROSS_LENGTH/2), 
		      LANDMARK_YELLOW_GOAL_CROSS_Y);
     painter.drawLine(LANDMARK_BLUE_GOAL_CROSS_X, 
		      LANDMARK_BLUE_GOAL_CROSS_Y - 
		      (LINE_CROSS_LENGTH/2),
		      LANDMARK_BLUE_GOAL_CROSS_X , 
		      LANDMARK_BLUE_GOAL_CROSS_Y + 
		      (LINE_CROSS_LENGTH/2));
     painter.drawLine(LANDMARK_YELLOW_GOAL_CROSS_X, 
		      LANDMARK_YELLOW_GOAL_CROSS_Y- 
		      (LINE_CROSS_LENGTH/2),
		      LANDMARK_YELLOW_GOAL_CROSS_X, 
		      LANDMARK_YELLOW_GOAL_CROSS_Y + 
		      (LINE_CROSS_LENGTH/2));

     QPoint topBluePost = QPoint(LANDMARK_BLUE_GOAL_TOP_POST_X, 
				 LANDMARK_BLUE_GOAL_TOP_POST_Y);
     QPoint bottomBluePost = QPoint(LANDMARK_BLUE_GOAL_BOTTOM_POST_X, 
				    LANDMARK_BLUE_GOAL_BOTTOM_POST_Y);
     QPoint topYellowPost = QPoint(LANDMARK_YELLOW_GOAL_TOP_POST_X, 
				 LANDMARK_YELLOW_GOAL_TOP_POST_Y);
     QPoint bottomYellowPost = QPoint(LANDMARK_YELLOW_GOAL_BOTTOM_POST_X, 
				    LANDMARK_YELLOW_GOAL_BOTTOM_POST_Y);

     painter.setBrush(Qt::blue);
     painter.setPen(Qt::blue);
     painter.drawEllipse(topBluePost, 
			 GOAL_POST_RADIUS, GOAL_POST_RADIUS);
     painter.drawEllipse(bottomBluePost, 
			 GOAL_POST_RADIUS, GOAL_POST_RADIUS);
     painter.setBrush(Qt::yellow);
     painter.setPen(Qt::yellow);
     painter.drawEllipse(topYellowPost, GOAL_POST_RADIUS, 
			 GOAL_POST_RADIUS);
     painter.drawEllipse(bottomYellowPost,GOAL_POST_RADIUS,
			 GOAL_POST_RADIUS);

     QBrush goals = QBrush(Qt::white, Qt::CrossPattern);
     painter.setBrush(goals);
     painter.setPen(Qt::blue);
     painter.drawRect(blueGoal);
     painter.setPen(Qt::yellow);
     painter.drawRect(yellowGoal);
}

}
}
 
