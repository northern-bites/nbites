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
