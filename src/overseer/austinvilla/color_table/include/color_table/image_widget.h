/**
 * \file  image_widget.h
 * \brief Header for a separate ImageWidget to control 
 * images in the gui window effectively
 *
 * \author  Piyush Khandelwal (piyushk), piyushk@cs.utexas.edu
 *          Mohan Sridharan (smohan) and others.
 * Copyright (C) 2011, The University of Texas at Austin, Piyush Khandelwal
 *
 * License: Modified BSD License
 *
 * $ Id: 08/11/2011 02:54:56 PM piyushk $
 */

#ifndef IMAGE_WIDGET_MERDB9QX
#define IMAGE_WIDGET_MERDB9QX

#include <QWidget>
#include <QImage>

class QPaintEvent;

namespace color_table {

  /**
   * \class ImageWidget
   * \brief ImageWidget adds additional functionality to a standard widget
   *        such as obtaining mouse click events and easy repaint
   */
  class ImageWidget : public QWidget {
  Q_OBJECT
      
    public:
     ImageWidget(QWidget *parent);
     void reduceImageSize(int factor);

     void paintEvent(QPaintEvent *event);
     void mousePressEvent(QMouseEvent *event);
     void mouseMoveEvent(QMouseEvent *event);
    
    QImage* img;
     
    signals:
      void clicked(int x, int y, int button);
      void mouseXY(int x, int y);
  };

}

#endif /* end of include guard: IMAGE_WIDGET_MERDB9QX */
