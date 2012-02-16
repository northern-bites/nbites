/**
 * \file  main_window.cpp
 * \brief  Provides definitions for the MainWindow class.
 *
 * \author  Piyush Khandelwal (piyushk), piyushk@cs.utexas.edu
 * Copyright (C) 2011, The University of Texas at Austin, Piyush Khandelwal
 *
 * License: Modified BSD License
 *
 * $ Id: 08/11/2011 03:00:26 PM piyushk $
 */

#include <iostream>
#include <QtGui>
#include <QMessageBox>

#include <ros/package.h>
#include <rosbag/bag.h>
#include <rosbag/view.h>

#include <boost/foreach.hpp>
#include <boost/lexical_cast.hpp>

#include <color_table/main_window.h>

namespace color_table {

  using namespace Qt;

  /**
   * \brief  Constructor that initializes all elements + sets up the data 
   *         path for the classification window, as well as opens up the
   *         default color table
   */
  MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {
    ui.setupUi(this); // Calling this incidentally connects all ui's triggers to on_...() callbacks in this class.
    initialize();
    classWindow.loadDataDirectory(getBaseDirectory() + "/data/");
    classWindow.openDefaultColorTable();
  }

  void MainWindow::on_actionOpen_Bag_triggered() {

    images.clear();

    QString fileName = QFileDialog::getOpenFileName(this, tr("Open Bag"),
        QString((getBaseDirectory() + "/data/").c_str()),
        tr("ROS Bag Files (*.bag)"));

    if (fileName.isNull()) {
      ui.statusBar->showMessage("User cancelled operation");
      return;
    }
    rosbag::Bag bag;
    try {
      bag.open(fileName.toStdString(), rosbag::bagmode::Read);
    } catch (rosbag::BagIOException e) {
      ui.statusBar->showMessage("Error opening bag file!!");
      return;
    }

    std::vector<std::string> topics;
    topics.push_back(std::string("/camera/rgb/image_color"));

    rosbag::View view(bag, rosbag::TopicQuery(topics));
    BOOST_FOREACH(rosbag::MessageInstance const m, view) {
      sensor_msgs::ImageConstPtr image = m.instantiate<sensor_msgs::Image>();
      images.push_back(image);
    }

    ui.frameSlider->setEnabled(true);
    ui.frameSlider->setMaximum(images.size() - 1);
    ui.frameSlider->setMinimum(0);

    ui.currentFrameSpin->setEnabled(true);
    ui.currentFrameSpin->setMaximum(images.size() - 1);
    ui.currentFrameSpin->setMinimum(0);

    ui.numFrameEdit->setText(QString((boost::lexical_cast<std::string>(images.size() - 1)).c_str()));
    
    classWindow.changeImage(images[0]); 

    ui.statusBar->showMessage(("Bag file with " + boost::lexical_cast<std::string>(images.size()) + " frames").c_str());
    
  }

  /**
   * \brief  Gets the path of the package using the ROSPack API  
   */
  std::string MainWindow::getBaseDirectory() {
    return ros::package::getPath("color_table");
  }

  void MainWindow::on_currentFrameSpin_valueChanged(int value) {
    ui.frameSlider->setValue(value);
    classWindow.changeImage(images[value]);
  }

  void MainWindow::on_frameSlider_sliderMoved(int value) {
    ui.currentFrameSpin->setValue(value);  //implicitly calls the slot for currentFrameSpin -> value Changed
  }

  void MainWindow::initialize() {
    ui.frameSlider->setEnabled(false);
    ui.currentFrameSpin->setEnabled(false);
    ui.numFrameEdit->setText(QString("0"));    
  }

  /**
   * \brief   Close function has been overloaded to terminate the entire 
   *          program if this window is terminated
   */
  void MainWindow::closeEvent(QCloseEvent *event) {
    classWindow.close();
    //WriteSettings();
    event->accept();
  }

  /**
   * \brief  Overloaded function to show the classification window as well 
   *         at the same time
   */
  void MainWindow::show() {
    QMainWindow::show();
    classWindow.show();
  }

}  // namespace color_table
