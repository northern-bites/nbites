/**
 * @class ColorTableCreator
 *
 * Tool to define/calibrate a color table
 *
 * @author EJ Googins
 */

#include "ColorTableCreator.h"

#include <QtDebug>

#include <QFileDialog>

namespace qtool {
    namespace colorcreator {
        using namespace qtool::data;
        using namespace qtool::image;

        ColorTableCreator::ColorTableCreator(DataManager::ptr dataManager,
                                             QWidget *parent) :
            QWidget(parent), dataManager(dataManager)
        {


        }

        void ColorTableCreator::loadColorTableBtnPushed(){
            QString base_directory = QString(NBITES_DIR) + "/data/tables";
            QString filename = QFileDialog::getOpenFileName
                                       (this,
                                        tr("Load Color Table from File"),
                                        base_directory,
                                        tr("Color Table files (*.tbl)"));
            // Use the filename
        }

        void ColorTableCreator::saveColorTableBtnPushed(){
            QString base_directory = QString(NBITES_DIR) + "/data/tables";
            QString filename = QFileDialog::getSaveFileName
                                       (this,
                                        tr("Save Color Table to File"),
                                        base_directory + "/new_sliders.tbl",
                                        tr("Color Table files (*.tbl)"));
            // Use the filename
        }
    }
}

// void ColorTableCreator::saveColorTableBtnPushed() {
//     QString base_directory = QString(NBITES_DIR) + "/data/tables";
//     QString filename =
//             QFileDialog::getSaveFileName(this,
//                     tr("Save Color Table to File"),
//                     base_directory + "/new_table.mtb",
//                     tr("Color Tables(*.mtb)"));
//     ColorTableCreator::writeFromSliders(filename, colorSpace);
// }
