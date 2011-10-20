/**
 * @class Viewer
 *
 * This class draws the field so that we can display the
 * movement of robots and the ball on the field.  The 
 * painting uses all the constants from FieldConstants
 * so that if the field changes dimensions it should
 * still work.
 *
 * @author Dani McAvoy
 * @date October 2011
 */

#pragma once

#include <QWidget>
#include <QtGui>
#include <vector>

#include "data/DataManager.h"
#include "MObjectViewer.h"
#include "man/include/FieldConstants.h"

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
