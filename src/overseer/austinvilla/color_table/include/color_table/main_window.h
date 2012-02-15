/**
 * \file  main_window.h
 * \brief  Header for the MainWindow class. 
 *
 * This class is responsible for handling a bit of ros stuff. Also uses
 * the rosbag api to open bag and control which frame of the bag is being
 * currently run.
 *
 * \author  Piyush Khandelwal (piyushk), piyushk@cs.utexas.edu
 * Copyright (C) 2011, The University of Texas at Austin, Piyush Khandelwal
 *
 * License: Modified BSD License
 *
 * $ Id: 08/11/2011 02:41:48 PM piyushk $
 */

#ifndef MAIN_WINDOW_D9KV76TW
#define MAIN_WINDOW_D9KV76TW

#include <QtGui/QMainWindow>
#include <sensor_msgs/Image.h>

#include <color_table/classification_window.h>
#include "ui_main_window.h"

namespace color_table {

  /**
   * \class MainWindow
   * \brief MainWindow opens up bag files and selects the frame of interst 
   */
  class MainWindow : public QMainWindow {
  Q_OBJECT

  public:

    /**
     * \brief  Constructor that initializes all elements + sets up the data 
     *         path for the classification window, as well as opens up the
     *         default color table
     */
    MainWindow(QWidget *parent = 0);

    void initialize(); 

    /**
     * \brief   Close function has been overloaded to terminate the entire 
     *          program if this window is terminated
     */
    void closeEvent(QCloseEvent *event);

    /**
     * \brief  Overloaded function to show the classification window as well 
     *         at the same time
     */
    void show();

    /**
     * \brief  Gets the path of the package using the ROSPack API 
     */
    std::string getBaseDirectory();

  public slots:

    void on_actionOpen_Bag_triggered();
    void on_currentFrameSpin_valueChanged(int value);
    void on_frameSlider_sliderMoved(int value);

  private:
      Ui::ClassificationTool ui;
      ClassificationWindow classWindow;

      std::vector<sensor_msgs::ImageConstPtr> images;  ///< Current array of images retrieved from the bag file
  };

} 

#endif /* end of include guard: MAIN_WINDOW_D9KV76TW */
