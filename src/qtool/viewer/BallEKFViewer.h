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
#include "EKFDataViewer.h"

namespace qtool {
namespace viewer {

class BallEKFViewer : public QMainWindow{
        Q_OBJECT

public:
	BallEKFViewer(data::DataManager::ptr dataManager);

private:
    std::vector<QDockWidget*> dockWidget;
    data::DataManager::ptr dataManager;
    EKFDataViewer* ekfDataViewer;
 
};

}
}
