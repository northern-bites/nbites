/*
 * CollapsibleImageViewer
 */

#include "CollapsibleImageViewer.h"

namespace qtool {

using namespace viewer;

CollapsibleImageViewer::CollapsibleImageViewer(BMPImageViewer* _image,
                                               QString _title,
                                               QWidget* parent) :
    QWidget(parent),
    image(_image),
    title(_title),
    imageShown(false)
{
    toggleButton = new QPushButton("", this);

    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->addWidget(toggleButton);
    layout->addWidget(image);
    connect(toggleButton, SIGNAL(clicked()), this, SLOT(toggle()));
    hide();
}

void CollapsibleImageViewer::hide()
{
    image->hide();
    QString buttonText = title + " >>>";
    toggleButton->setText(buttonText);
    imageShown = false;
}

void CollapsibleImageViewer::show()
{
    image->show();
    QString buttonText = title + " <<<";
    toggleButton->setText(buttonText);
    imageShown = true;
}

void CollapsibleImageViewer::toggle()
{
    if(imageShown)
    {
        hide();
    }
    else
    {
        show();
    }
}
}
