#include "VisionSimGUI.h"
#include <QBoxLayout>
#include <QLabel>

namespace tool{
namespace visionsim{

/*
 * Constructor is called by the main VisionSim module.
 *
 * @param top -- the top image, for one of the image viewers
 * @param bottom -- the bottom image, for the other image viewer
 * @param world -- the world model, for the world viewer
 */

VisionSimGUI::VisionSimGUI(Image& top, Image& bottom, World& world,
                           QWidget* parent) :
    QWidget(parent),
    topImageView(top),
    bottomImageView(bottom),
    worldView(world),
    controls(this)
{
    // A widget to hold the two images in a nice layout
    QWidget* imagesView = new QWidget(this);
    QVBoxLayout* imagesLayout = new QVBoxLayout(imagesView);

    // Extra container is so that the lines don't draw everywhere
    QWidget* topContainer = new QWidget(this);
    topContainer->setMinimumSize(IMAGE_WIDTH, IMAGE_HEIGHT);
    topImageView.setParent(topContainer);

    // Extra container is so that the lines don't draw everywhere
    QWidget* bottomContainer = new QWidget(this);
    bottomContainer->setMinimumSize(IMAGE_WIDTH, IMAGE_HEIGHT);
    bottomImageView.setParent(bottomContainer);

    // Labels for the images
    QLabel* topLabel = new QLabel(tr("Top Image"));
    QLabel* bottomLabel = new QLabel(tr("Bottom Image"));

    // Arrange the labels and images
    imagesLayout->addWidget(topLabel);
    imagesLayout->addWidget(topContainer);
    imagesLayout->addWidget(bottomLabel);
    imagesLayout->addWidget(bottomContainer);

    imagesView->setLayout(imagesLayout);

    // For the world view and controls
    QWidget* worldControlView = new QWidget(this);
    QVBoxLayout* worldControlLayout = new QVBoxLayout(worldControlView);

    // Arrange the world view and controls
    QLabel* worldLabel = new QLabel(tr("World State"));
    worldControlLayout->addWidget(worldLabel);
    worldControlLayout->addWidget(&worldView);
    worldControlLayout->addWidget(&controls);

    worldControlView->setLayout(worldControlLayout);

    // Combines the two parts into the main layout
    QHBoxLayout* mainLayout = new QHBoxLayout(this);
    mainLayout->addWidget(imagesView);
    mainLayout->addWidget(worldControlView);
    this->setLayout(mainLayout);
}

// Has all of the different views repaint; gets called if the model is updated
// and the views are showing out-of-date data
void VisionSimGUI::updateView()
{
    topImageView.repaint();
    bottomImageView.repaint();
    worldView.repaint();
}

}
}
