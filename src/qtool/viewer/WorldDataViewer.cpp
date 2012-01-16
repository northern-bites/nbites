#include "WorldDataViewer.h"
#include <vector>
#include "man/memory/Memory.h"

namespace qtool {
namespace viewer {

using namespace data;
using namespace man::memory;

WorldDataViewer::WorldDataViewer(QWidget *parent) :
           QWidget(parent){

     model= new QStandardItemModel(2,23, this);


     // *** KNOWN POSITION ***
     knownPos = new QLabel(tr("KNOWN POSITION"));

     LxKnownPos = new QLabel(tr("X:"));
     xKnownPos = new QLineEdit();
     xKnownPos->setReadOnly(true);
     LxKnownPos->setBuddy(xKnownPos);

     LyKnownPos = new QLabel(tr("Y:"));
     yKnownPos = new QLineEdit();
     yKnownPos->setReadOnly(true);
     LyKnownPos->setBuddy(yKnownPos);

     LhKnownPos = new QLabel(tr("H:"));
     hKnownPos = new QLineEdit();
     hKnownPos->setReadOnly(true);
     LhKnownPos->setBuddy(hKnownPos);

     LballXKnown = new QLabel(tr("Ball X:"));
     ballXKnown = new QLineEdit();
     ballXKnown->setReadOnly(true);
     LballXKnown->setBuddy(ballXKnown); 
     
     LballYKnown = new QLabel(tr("Ball Y:"));
     ballYKnown = new QLineEdit();
     ballYKnown->setReadOnly(true);
     LballYKnown->setBuddy(ballYKnown);


     // *** MY LOC ESTIMATES ***
     myLocEst = new QLabel(tr("MY LOC ESTIMATES"));

     LxLocEst = new QLabel(tr("X:"));
     xLocEst = new QLineEdit();
     xLocEst->setReadOnly(true);
     LxLocEst->setBuddy(xLocEst);

     LyLocEst = new QLabel(tr("Y:"));
     yLocEst = new QLineEdit();
     yLocEst->setReadOnly(true);
     LyLocEst->setBuddy(yLocEst);

     LhLocEst = new QLabel(tr("H:"));
     hLocEst = new QLineEdit();
     hLocEst->setReadOnly(true);
     LhLocEst->setBuddy(hLocEst);

     LuncertX = new QLabel(tr("Uncert X:"));
     uncertX = new QLineEdit();
     uncertX->setReadOnly(true);
     LuncertX->setBuddy(uncertX);

     LuncertY = new QLabel(tr("Uncert Y:"));
     uncertY = new QLineEdit();
     uncertY->setReadOnly(true);
     LuncertY->setBuddy(uncertY);

     LuncertH = new QLabel(tr("Uncert H:"));
     uncertH = new QLineEdit();
     uncertH->setReadOnly(true);
     LuncertH->setBuddy(uncertH);

     LodoF = new QLabel(tr("ODO F:"));
     odoF = new QLineEdit();
     odoF->setReadOnly(true);
     LodoF->setBuddy(odoF);

     LodoL = new QLabel(tr("ODO L:"));
     odoL = new QLineEdit();
     odoL->setReadOnly(true);
     LodoL->setBuddy(odoL);

     LodoR = new QLabel(tr("ODO R:"));
     odoR = new QLineEdit();
     odoR->setReadOnly(true);
     LodoR->setBuddy(odoR);

     // *** BALL ESTIMATES ***
     ballEst = new QLabel(tr("BALL ESTIMATES"));

     LballX= new QLabel(tr("Ball X:"));
     ballX = new QLineEdit();
     ballX->setReadOnly(true);
     LballX->setBuddy(ballX);

     LballY= new QLabel(tr("Ball Y:"));
     ballY = new QLineEdit();
     ballY->setReadOnly(true);
     LballY->setBuddy(ballY);

     LballUncertX= new QLabel(tr("Uncert X:"));
     ballUncertX = new QLineEdit();
     ballUncertX->setReadOnly(true);
     LballUncertX->setBuddy(ballUncertX);

     LballUncertY= new QLabel(tr("Uncert Y:"));
     ballUncertY = new QLineEdit();
     ballUncertY->setReadOnly(true);
     LballUncertY->setBuddy(ballUncertY);

     LballVelX= new QLabel(tr("Vel X:"));
     ballVelX = new QLineEdit();
     ballVelX->setReadOnly(true);
     LballVelX->setBuddy(ballVelX);

     LballVelY= new QLabel(tr("Vel Y:"));
     ballVelY = new QLineEdit();
     ballVelY->setReadOnly(true);
     LballVelY->setBuddy(ballVelY);

     LabsVel = new QLabel(tr("Abs Vel"));
     absVel = new QLineEdit();
     absVel->setReadOnly(true);
     LabsVel->setBuddy(absVel);

     LballVelUncertX= new QLabel(tr("Uncert Vel X:"));
     ballVelUncertX = new QLineEdit();
     ballVelUncertX->setReadOnly(true);
     LballVelUncertX->setBuddy(ballVelUncertX);

     LballVelUncertY= new QLabel(tr("Uncert Vel Y:"));
     ballVelUncertY = new QLineEdit();
     ballVelUncertY->setReadOnly(true);
     LballVelUncertY->setBuddy(ballVelUncertY);

     mapper = new QDataWidgetMapper(this);
     mapper->setModel(model);
     mapper->addMapping(xKnownPos   , 0);
     mapper->addMapping(yKnownPos   , 1);
     mapper->addMapping(hKnownPos, 2);
     mapper->addMapping(ballXKnown, 3);
     mapper->addMapping(ballYKnown, 4);
     mapper->addMapping(xLocEst, 5);
     mapper->addMapping(yLocEst, 6);
     mapper->addMapping(hLocEst   , 7);
     mapper->addMapping(uncertX   , 8);
     mapper->addMapping(uncertY, 9);
     mapper->addMapping(uncertH, 10);
     mapper->addMapping(odoF, 11);
     mapper->addMapping(odoL, 12);
     mapper->addMapping(odoR, 13);
     mapper->addMapping(ballX, 14);
     mapper->addMapping(ballY, 15);
     mapper->addMapping(ballUncertX, 16);
     mapper->addMapping(ballUncertY   , 17);
     mapper->addMapping(ballVelX   , 18);
     mapper->addMapping(ballVelY, 19);
     mapper->addMapping(absVel, 20);
     mapper->addMapping(ballVelUncertX, 21);
     mapper->addMapping(ballVelUncertY, 22);


     // This puts each piece in a certain row
     // and position with in that row
     QGridLayout *layout = new QGridLayout();

			// Known Position
     layout->addWidget(knownPos, 0, 0, 1, 1);
     layout->addWidget(LxKnownPos, 0, 0, 2, 1);
     layout->addWidget(xKnownPos  , 0, 1, 2, 1);
     layout->addWidget(LyKnownPos, 0, 0, 3, 1);
     layout->addWidget(yKnownPos , 0, 1, 3, 1);
     layout->addWidget(LhKnownPos, 0, 0, 4, 1);
     layout->addWidget(hKnownPos , 0, 1, 4, 1);
     layout->addWidget(LballXKnown, 0, 0, 5, 1);
     layout->addWidget(ballXKnown , 0, 1, 5, 1);
     layout->addWidget(LballYKnown, 0, 0, 6, 1);
     layout->addWidget(ballYKnown, 0, 1, 6, 1);

			// Loc Estimates
     layout->addWidget(myLocEst, 0, 0, 7, 1);
     layout->addWidget(LxLocEst, 0, 0, 8, 1);
     layout->addWidget(xLocEst  , 0, 1, 8, 1);
     layout->addWidget(LyLocEst, 0, 0, 9, 1);
     layout->addWidget(yLocEst , 0, 1, 9, 1);
     layout->addWidget(LhLocEst, 0, 0, 10, 1);
     layout->addWidget(hLocEst , 0, 1, 10, 1);
     layout->addWidget(LuncertX, 0, 0, 11, 1);
     layout->addWidget(uncertX , 0, 1, 11, 1);
     layout->addWidget(LuncertY, 0, 0, 12, 1);
     layout->addWidget(uncertY , 0, 1, 12, 1);
     layout->addWidget(LuncertH, 0, 0, 13, 1);
     layout->addWidget(uncertH    , 0, 1, 13, 1);
     layout->addWidget(LodoF, 0, 0, 14, 1);
     layout->addWidget(odoF   , 0, 1, 14, 1);
     layout->addWidget(LodoL, 0, 0, 15, 1);
     layout->addWidget(odoL , 0, 1, 15, 1);
     layout->addWidget(LodoR, 0, 0, 16, 1);
     layout->addWidget(odoR , 0, 1, 16, 1);

			// Ball Estimates
     layout->addWidget(ballEst, 0, 0, 17, 1);
     layout->addWidget(LballX, 0, 0, 18, 1);
     layout->addWidget(ballX , 0, 1, 18, 1);    
     layout->addWidget(LballY, 0, 0, 19, 1);
     layout->addWidget(ballY , 0, 1, 19, 1);
     layout->addWidget(LballUncertX, 0, 0, 20, 1);
     layout->addWidget(ballUncertX , 0, 1, 20, 1);
     layout->addWidget(LballUncertY, 0, 0, 21, 1);
     layout->addWidget(ballUncertY , 0, 1, 21, 1);
     layout->addWidget(LballVelX, 0, 0, 22, 1);
     layout->addWidget(ballVelX , 0, 1, 22, 1);
     layout->addWidget(LballVelY, 0, 0, 23, 1);
     layout->addWidget(ballVelY    , 0, 1, 23, 1);
     layout->addWidget(LabsVel, 0, 0, 24, 1);
     layout->addWidget(absVel   , 0, 1, 24, 1);
     layout->addWidget(LballVelUncertX, 0, 0, 25, 1);
     layout->addWidget(ballVelUncertX , 0, 1, 25, 1);
     layout->addWidget(LballVelUncertY, 0, 0, 26, 1);
     layout->addWidget(ballVelUncertY , 0, 1, 26, 1);
     setLayout(layout);

     mapper->toFirst();

}
}
}
