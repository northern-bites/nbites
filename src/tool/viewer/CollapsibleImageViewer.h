/*
 * Implements a "collapsible" widget so that images can be displayed
 * only if the user wants them to be displayed.
 *
 */

#pragma once

#include <QWidget>
#include <QPushButton>
#include <QVBoxLayout>
#include "BMPImageViewer.h"

namespace qtool {


class CollapsibleImageViewer : public QWidget{
    Q_OBJECT

public:
    CollapsibleImageViewer(viewer::BMPImageViewer* _image,
                           QString _title,
                           QWidget* parent = 0);
    ~CollapsibleImageViewer() {};

public slots:
    void toggle();
    void show();
    void hide();

private:
    viewer::BMPImageViewer* image;
    QPushButton* toggleButton;
    QString title;
    bool imageShown;
};

}
