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
    imageShown(true)
{
    toggleButton = new QPushButton((title + " <<<"), this);

    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->addWidget(toggleButton);
    layout->addWidget(image);

    connect(toggleButton, SIGNAL(clicked()), this, SLOT(toggle()));
}

void CollapsibleImageViewer::toggle()
{
    if(imageShown)
    {
        image->hide();
        QString buttonText = title + " >>>";
        toggleButton->setText(buttonText);
    }
    else
    {
        image->show();
        QString buttonText = title + " <<<";
        toggleButton->setText(buttonText);
    }

    imageShown = !imageShown;
}
}
