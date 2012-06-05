/**
 * @class Viewer
 *
 * This class is going to be the WorldController from
 * the previous Tool.  It will show the field with
 * robots and balls in their positions which are
 * passed in.
 *
 * @author Dani McAvoy
 * @date October 2011
 */

#pragma once

#include <QWidget>
#include <QtGui>
#include <vector>

#include "data/DataManager.h"
#include "image/PaintField.h"
#include "image/PaintLocalization.h"
#include "BMPImageViewer.h"

namespace qtool {
namespace viewer {

class FieldViewer : public QMainWindow{
    Q_OBJECT

public:
    FieldViewer(data::DataManager::ptr dataManager);

private slots:
    void updateLocalization();

private:
    std::vector<QDockWidget*> dockWidget;
    data::DataManager::ptr dataManager;
    image::PaintField* paintField;
    image::PaintLocalization* paintLocalization;
    BMPImageViewer* fieldView;
    image::OverlayedImage *particlesOnField;
    man::memory::MLocalization::const_ptr localizationStream;
    
};

}
}
