/**
 * \file  classification_window.h
 * \brief Header for the classification window 
 *
 * \author  Piyush Khandelwal (piyushk), piyushk@cs.utexas.edu
 *          Mohan Sridharan (smohan) and others.
 * Copyright (C) 2011, The University of Texas at Austin, Piyush Khandelwal
 *
 * License: Modified BSD License
 *
 * $ Id: 08/10/2011 03:09:11 PM piyushk $
 */

#ifndef CLASSIFICATION_WINDOW_GZ6WWOJ1
#define CLASSIFICATION_WINDOW_GZ6WWOJ1

#include <QtGui/QMainWindow>
#include <sensor_msgs/Image.h>

#include <color_table/common.h>
#include "ui_classification_window.h"

namespace color_table {

  typedef Rgb RgbImage[IMAGE_HEIGHT][IMAGE_WIDTH];
  typedef uint8_t SegImage[IMAGE_HEIGHT][IMAGE_WIDTH];

  enum Image {
    RGB,
    SEG
  };

  enum ClickMode {
    ADD,
    DELETE
  };
  
  /**
   * \class ClassificationWindow
   * \brief The Classification Window is the user's interface to assign
   * lookup values for the color table
   */
  class ClassificationWindow : public QMainWindow {
  Q_OBJECT

  public:

    ClassificationWindow(QWidget *parent = 0);

    /**
     * \brief  Get an updated image from the main window
     */
    void changeImage(sensor_msgs::ImageConstPtr image);

    /** 
     * \brief Function to draw an RgbImage onto an ImageWidget screen
     */
    void drawRgbImage(ImageWidget *widget);

    /** 
     * \brief Function to draw an RgbImage onto an ImageWidget screen
     */
    void drawSegImage(ImageWidget *widget);

    /** 
     * \brief Obtains seg image by segmenting the raw image using specified
     *        color table
     */
    void segmentImage(bool useTempColorTable);

    /**
     * \brief Redraw all the images in the 3 image widgets
     */
    void redrawImages(bool useTempColorTable = false);

    /**
     * \brief Sets currentColor if the user indicates a different color
     */
    void setColor(int color);

    /**
     * \brief Update the status bar based on current action
     */
    void updateStatus();

    /**
     * \brief Opens the color table specified by colorTableFilename
     * \return true if opened successfully, false otherwise
     */
    bool openColorTable();

    /**
     * \brief   Opens up the default color table - helpful during startup
     */
    void openDefaultColorTable();

    /**
     * \brief loads the base path of the data directory, allowing classWindow
     *        to get the default color table directory
     */
    void loadDataDirectory(std::string basePath);

    /**
     * \brief   Close function has been overloaded to terminate the entire 
     *          program if this window is terminated
     */
    void closeEvent(QCloseEvent *event);

  public slots:
    void on_bigImage_clicked(int x, int y, int button);
    void on_bigImage_mouseXY(int x, int y);
    void on_rawImage_clicked(int x, int y, int button);
    void on_segImage_clicked(int x, int y, int button);

    void on_addRadio_clicked();
    void on_deleteRadio_clicked();

    void on_orangeButton_clicked();
    void on_pinkButton_clicked();
    void on_blueButton_clicked();
    void on_greenButton_clicked();
    void on_whiteButton_clicked();
    void on_yellowButton_clicked();

    void on_colorCombo_currentIndexChanged(int index);

    void on_actionNew_triggered();
    void on_actionOpen_triggered();
    void on_actionSave_triggered();
    void on_actionSave_As_triggered();

  private:
    Ui::ClassificationWindow ui;
    std::string colorTableFilename;
    std::string dataDirectory;

    RgbImage rgbImage;
    SegImage segImage;
    ColorTable colorTable;                 ///< The actual color table that the user is editing
    ColorTable tempColorTable;             ///< Temp color table for user to visualize the pixels once before saving

    QRgb segColors[NUM_COLORS];
    std::string segColorNames[NUM_COLORS];

    Image imageSelected;
    ClickMode clickMode;
    Color currentColor;

  };

}  

#endif /* end of include guard: CLASSIFICATION_WINDOW_GZ6WWOJ1 */
