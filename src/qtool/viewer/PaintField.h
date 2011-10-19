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

class PaintField : public QWidget{
        Q_OBJECT

public:
  PaintField(QWidget *parent = 0);

  QSize minimumSizeHint() const;
  QSize sizeHint() const;

  void ballDataChanged(const QBrush &brush);

private slots:


protected:
  void paintEvent(QPaintEvent *event);


private:
  std::vector<QDockWidget*> dockWidget;
  data::DataManager::ptr dataManager;
  
  QStandardItemModel *model;
  QDataWidgetMapper *mapper; 

};

}
}
