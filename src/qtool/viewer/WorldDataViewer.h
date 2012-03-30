/**
 * @class Viewer
 *
 * 
 *
 * @author Dani McAvoy
 */

#pragma once

#include <QWidget>
#include <QtGui>
#include <vector>

#include "data/DataManager.h"
#include "MObjectViewer.h"

namespace qtool {
namespace viewer {

class WorldDataViewer : public QWidget{
        Q_OBJECT

public:
  WorldDataViewer(QWidget *parent = NULL);

private:
  std::vector<QDockWidget*> dockWidget;
  data::DataManager::ptr dataManager;

  // Known position
  QLabel *knownPos;

  QLabel *LxKnownPos;
  QLabel *LyKnownPos;
  QLabel *LhKnownPos;
  QLabel *LballXKnown;
  QLabel *LballYKnown;

  QLineEdit *xKnownPos;  
  QLineEdit *yKnownPos;
  QLineEdit *hKnownPos;
  QLineEdit *ballXKnown;
  QLineEdit *ballYKnown;

  
  //My Loc Estimates
  QLabel *myLocEst;

  QLabel *LxLocEst;
  QLabel *LyLocEst;
  QLabel *LhLocEst;
  QLabel *LuncertX;
  QLabel *LuncertY;
  QLabel *LuncertH;
  QLabel *LodoF;
  QLabel *LodoL;
  QLabel *LodoR;

  QLineEdit *xLocEst;
  QLineEdit *yLocEst;
  QLineEdit *hLocEst;
  QLineEdit *uncertX;
  QLineEdit *uncertY;
  QLineEdit *uncertH;
  QLineEdit *odoF;
  QLineEdit *odoL;
  QLineEdit *odoR;
  
  // Ball Estimates
  QLabel *ballEst;

  QLabel *LballX;
  QLabel *LballY;
  QLabel *LballUncertX;
  QLabel *LballUncertY;
  QLabel *LballVelX;
  QLabel *LballVelY;
  QLabel *LabsVel;
  QLabel *LballVelUncertX;
  QLabel *LballVelUncertY;

  QLineEdit *ballX;
  QLineEdit *ballY;
  QLineEdit *ballUncertX;
  QLineEdit *ballUncertY;
  QLineEdit *ballVelX;
  QLineEdit *ballVelY;
  QLineEdit *absVel;
  QLineEdit *ballVelUncertX;
  QLineEdit *ballVelUncertY;
  
  QStandardItemModel *model;
  QDataWidgetMapper *mapper; 

};

}
}
