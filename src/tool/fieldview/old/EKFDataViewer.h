/**
 * @class Viewer
 *
 * This class will be for testing the BallEKF.
 * It will allow you to input values into the BallEKF
 * and get back the predicted values.
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

class EKFDataViewer : public QWidget{
        Q_OBJECT

public:
	EKFDataViewer(QWidget *parent = NULL);

private slots:
  void runEKF();

private:
	std::vector<QDockWidget*> dockWidget;
	data::DataManager::ptr dataManager;
        QLabel *xPosLabel;
        QLabel *yPosLabel;
        QLabel *xVelLabel;
        QLabel *yVelLabel;
        QLabel *xAccLabel;
        QLabel *yAccLabel;

        QLineEdit *xEdit;
        QLineEdit *yEdit;
        QLineEdit *xVelEdit;
        QLineEdit *yVelEdit;
        QLineEdit *xAccEdit;
        QLineEdit *yAccEdit;  

        QPushButton *testEKFButton;
  
        QStandardItemModel *model;
        QDataWidgetMapper *mapper; 

};

}
}
