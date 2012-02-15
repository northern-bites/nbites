/**
 * \file  classification_window.cpp
 * \brief  Class definitions for the classification window.
 *
 * \author  Piyush Khandelwal (piyushk), piyushk@cs.utexas.edu
 *          Mohan Sridharan (smohan) and others.
 * Copyright (C) 2011, The University of Texas at Austin, Piyush Khandelwal
 *
 * License: Modified BSD License
 *
 * $ Id: 08/11/2011 03:40:21 PM piyushk $
 */

#include <iostream>
#include <algorithm>

#include <QtGui>
#include <QMessageBox>
#include <QColor>

#include <color_table/classification_window.h>

namespace color_table {

  using namespace Qt;

  ClassificationWindow::ClassificationWindow(QWidget *parent) : QMainWindow(parent) {
    ui.setupUi(this); // Calling this incidentally connects all ui's triggers to on_...() callbacks in this class.

    // Set up segmented colors
    segColors[UNDEFINED] = qRgb(0,0,0);
    segColors[ORANGE] = qRgb(255,155,0);
    segColors[PINK] = qRgb(255,105,180);
    segColors[BLUE] = qRgb(0,0,255);
    segColors[GREEN] = qRgb(0,128,0);
    segColors[WHITE] = qRgb(255,255,255);
    segColors[YELLOW] = qRgb(255,255,0);

    segColorNames[UNDEFINED] = "Undefined";
    segColorNames[ORANGE] = "Orange";
    segColorNames[PINK] = "Pink";
    segColorNames[BLUE] = "Blue";
    segColorNames[GREEN] = "Green";
    segColorNames[WHITE] = "White";
    segColorNames[YELLOW] = "Yellow";

    // Set up the current bigImage selected as the RGB Image
    imageSelected = RGB;

    // Currently adding pixels
    clickMode = ADD;
    ui.addRadio->setDown(true);

    // Currently operating on the color orange
    currentColor = ORANGE;
    ui.orangeButton->setDown(true);
    
    updateStatus();
  }

  /**
   * \brief Sets currentColor if the user indicates a different color
   */
  void ClassificationWindow::setColor(int color) {
    ui.orangeButton->setChecked(false);
    ui.pinkButton->setChecked(false);
    ui.blueButton->setChecked(false);
    ui.greenButton->setChecked(false);
    ui.whiteButton->setChecked(false);
    ui.yellowButton->setChecked(false);

    switch(color) {
      case ORANGE:
        ui.orangeButton->setChecked(true);
        break;
      case PINK:
        ui.pinkButton->setChecked(true);
        break;
      case BLUE:
        ui.blueButton->setChecked(true);
        break;
      case GREEN:
        ui.greenButton->setChecked(true);
        break;
      case WHITE:
        ui.whiteButton->setChecked(true);
        break;
      case YELLOW:
        ui.yellowButton->setChecked(true);
        break;
      default:;
    };

    ui.colorCombo->setCurrentIndex(color - 1);
    currentColor = (Color) color;

    updateStatus();    
  }

  /**
   * \brief Update the status bar based on current action
   */
  void ClassificationWindow::updateStatus() {

    std::string status;
    switch(clickMode) {
      case ADD:
        status += "Adding";
        break;
      case DELETE:
        status += "Removing";
        break;
    }

    status += " ";
    status += segColorNames[currentColor];
    status += " ";
    status += "pixels.";

    ui.statusBar->showMessage(QString(status.c_str()));
  }

  /** 
   * \brief Function to draw an RgbImage onto an ImageWidget screen
   */
  void ClassificationWindow::drawRgbImage(ImageWidget *widget) {

    QRgb value;
    for (unsigned int i = 0; i < IMAGE_HEIGHT; i++) {
      for (unsigned int j = 0; j < IMAGE_WIDTH; j++) {
        value = qRgb(rgbImage[i][j].r, rgbImage[i][j].g, rgbImage[i][j].b);
        widget->img->setPixel(j,i,value);
      }
    }

    widget->repaint();

  }

  /** 
   * \brief Function to draw an RgbImage onto an ImageWidget screen
   */
  void ClassificationWindow::drawSegImage(ImageWidget *widget) {

    QRgb value;
    for (unsigned int i = 0; i < IMAGE_HEIGHT; i++) {
      for (unsigned int j = 0; j < IMAGE_WIDTH; j++) {
        value = segColors[segImage[i][j]];
        widget->img->setPixel(j,i,value);
      }
    }

    widget->repaint();

  }

  /**
   * \brief  Get an updated image from the main window
   */
  void ClassificationWindow::changeImage(sensor_msgs::ImageConstPtr image) {

    for (unsigned int i = 0; i < image->height; i++) {
      for (unsigned int j = 0; j < image->width; j++) {
        rgbImage[i][j].r =  image->data[i * image->step + 3 * j + 0]; 
        rgbImage[i][j].g =  image->data[i * image->step + 3 * j + 1]; 
        rgbImage[i][j].b =  image->data[i * image->step + 3 * j + 2];
      }
    }

    redrawImages();
  }

  /** 
   * \brief Obtains seg image by segmenting the raw image using specified
   *        color table
   */
  void ClassificationWindow::segmentImage(bool useTempColorTable) {

    ColorTable *table = &colorTable;
    if (useTempColorTable) {
      table = &tempColorTable;
    }

    for (unsigned int i = 0; i < IMAGE_HEIGHT; i++) {
      for (unsigned int j = 0; j < IMAGE_WIDTH; j++) {
        segImage[i][j] = (*table)[rgbImage[i][j].r / 2][rgbImage[i][j].g / 2][rgbImage[i][j].b / 2];
      }
    }

  }

  /**
   * \brief Redraw all the images in the 3 image widgets
   */
  void ClassificationWindow::redrawImages(bool useTempColorTable) {

    segmentImage(useTempColorTable);
    
    drawRgbImage(ui.rawImage);
    drawSegImage(ui.segImage);

    if (imageSelected == RGB) {
      drawRgbImage(ui.bigImage);
    } else {
      drawSegImage(ui.bigImage);
    }

  }

  /**
   * \brief   Close function has been overloaded to terminate the entire 
   *          program if this window is terminated
   */
  void ClassificationWindow::closeEvent(QCloseEvent *event) {
    event->accept();
  }

  /**
   * \brief Opens the color table specified by colorTableFilename
   */
  bool ClassificationWindow::openColorTable() {
    FILE* f = fopen(colorTableFilename.c_str(), "rb");
    if (!f || ferror(f)) {
      return false;
    }
    size_t bytesRead = fread(tempColorTable, 128*128*128, 1, f);
    fclose(f);
    memcpy(colorTable, tempColorTable, 128*128*128);
    return bytesRead;
  }

  /**
   * \brief   Opens up the default color table - helpful during startup
   */
  void ClassificationWindow::openDefaultColorTable() {
    colorTableFilename = dataDirectory + "default.col";
    openColorTable();
  }

  /**
   * \brief loads the base path of the data directory, allowing classWindow
   *        to get the default color table directory
   */
  void ClassificationWindow::loadDataDirectory(std::string basePath) {
    dataDirectory = basePath;
  }

  void ClassificationWindow::on_bigImage_clicked(int x, int y, int button) {
    
    switch (button) {

      case Qt::LeftButton: {
        memcpy(tempColorTable, colorTable, 128 * 128 * 128);
        int sen = ui.sensitivityDial->value();
        Rgb rgb = rgbImage[y][x];
        if (clickMode == ADD) {
          for (int r = std::max((int)rgb.r - sen * 5, 0); r <=std::min((int)rgb.r + sen * 5, 255); r+=2) {
            for (int g = std::max((int)rgb.g - sen * 5, 0); g <=std::min((int)rgb.g + sen * 5, 255); g+=2) {
              for (int b = std::max((int)rgb.b - sen * 5, 0); b <=std::min((int)rgb.b + sen * 5, 255); b+=2) {
                tempColorTable[r/2][g/2][b/2] = currentColor;
              }
            }
          }
        } else {
          for (int r = std::max((int)rgb.r - sen * 5, 0); r <=std::min((int)rgb.r + sen * 5, 255); r+=2) {
            for (int g = std::max((int)rgb.g - sen * 5, 0); g <=std::min((int)rgb.g + sen * 5, 255); g+=2) {
              for (int b = std::max((int)rgb.b - sen * 5, 0); b <=std::min((int)rgb.b + sen * 5, 255); b+=2) {
                tempColorTable[r/2][g/2][b/2] = (tempColorTable[r/2][g/2][b/2] == currentColor) ? (uint8_t)UNDEFINED : tempColorTable[r/2][g/2][b/2];
              }
            }
          }
        }
        redrawImages(true);
        break;
      }

      case Qt::RightButton: {
        memcpy(colorTable, tempColorTable, 128 * 128 * 128);
        redrawImages();
        break;
      }

    }

  }
  void ClassificationWindow::on_bigImage_mouseXY(int x, int y) {
    //std::cout << "bigImgMouseMv" << std::endl;
  }

  void ClassificationWindow::on_rawImage_clicked(int x, int y, int button) {
    imageSelected = RGB;
    redrawImages();
  }

  void ClassificationWindow::on_segImage_clicked(int x, int y, int button) {
    imageSelected = SEG;
    redrawImages();
  }

  void ClassificationWindow::on_addRadio_clicked() {
    clickMode = ADD;
    updateStatus();
  }

  void ClassificationWindow::on_deleteRadio_clicked() {
    clickMode = DELETE;
    updateStatus();
  }

  void ClassificationWindow::on_orangeButton_clicked() {
    setColor(ORANGE);
  }

  void ClassificationWindow::on_pinkButton_clicked() {
    setColor(PINK);
  }

  void ClassificationWindow::on_blueButton_clicked() {
    setColor(BLUE);
  }

  void ClassificationWindow::on_greenButton_clicked() {
    setColor(GREEN);
  }

  void ClassificationWindow::on_whiteButton_clicked() {
    setColor(WHITE);
  }

  void ClassificationWindow::on_yellowButton_clicked() {
    setColor(YELLOW);
  }

  void ClassificationWindow::on_colorCombo_currentIndexChanged(int index) {
    setColor(index + 1);
  }

  void ClassificationWindow::on_actionNew_triggered() {
    colorTableFilename.empty();
    memset(colorTable, UNDEFINED, 128 * 128 * 128);
    memset(tempColorTable, UNDEFINED, 128 * 128 * 128);
  }

  void ClassificationWindow::on_actionOpen_triggered() {
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open Color Table"),
                                                    QString(dataDirectory.c_str()),
                                                    tr("Color Table (*.col)"));
    if (fileName.isNull()) {
      ui.statusBar->showMessage("User cancelled operation");
      return;
    }

    colorTableFilename = fileName.toStdString();

    bool colorTableRead = openColorTable();
    if (!colorTableRead) {
      ui.statusBar->showMessage("Error reading color table!!");
      return;
    }

    ui.statusBar->showMessage("Color table read successfully!");
    redrawImages();
    
  }

  void ClassificationWindow::on_actionSave_triggered() {

    if (colorTableFilename.empty()) {
      ui.actionSave_As->trigger();
    }

    FILE* f = fopen(colorTableFilename.c_str(), "wb");
    size_t bytesWritten = fwrite(colorTable, 128*128*128, 1, f);
    fclose(f);
    if (!bytesWritten) {
      ui.statusBar->showMessage("Error writing color table!!");
      return;
    }

    ui.statusBar->showMessage("Color table written successfully!!");

  }

  void ClassificationWindow::on_actionSave_As_triggered() {
    QString fileName = QFileDialog::getSaveFileName(this, tr("Save Color Table"),
                                                    QString(dataDirectory.c_str()),
                                                    tr("Color Table (*.col)"));
    if (fileName.isNull()) {
      ui.statusBar->showMessage("User cancelled operation");
      return;
    }

    colorTableFilename = fileName.toStdString();
    ui.actionSave->trigger();

  }

}  // namespace color_table
