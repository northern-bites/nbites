#include "MainView.h"
#include <QBoxLayout>
#include <QLabel>

MainView::MainView() : QMainWindow()
{
    this->setWindowTitle(tr("VisionSim"));
    this->setCentralWidget(new MainWidget(this));
}

MainWidget::MainWidget(QWidget* parent) : QWidget(parent),
                                          world(),
                                          topImage(world, TOP),
                                          bottomImage(world, BOTTOM),
                                          topImageView(topImage),
                                          bottomImageView(bottomImage)
{
    // For the image viewer and controls on the left
    QWidget imagesView(this);
    QVBoxLayout imagesLayout(&imagesView);

    QWidget topContainer(this);
    topContainer.setMinimumSize(IMAGE_WIDTH, IMAGE_HEIGHT);
    topImageView.setParent(&topContainer);

    QWidget bottomContainer(this);
    bottomContainer.setMinimumSize(IMAGE_WIDTH, IMAGE_HEIGHT);
    bottomImageView.setParent(&bottomContainer);

    QLabel topLabel(tr("Top Image"));
    QLabel bottomLabel(tr("Bottom Image"));

    imagesLayout.addWidget(&topLabel);
    imagesLayout.addWidget(&topContainer);
    imagesLayout.addWidget(&bottomLabel);
    imagesLayout.addWidget(&bottomContainer);

    imagesView.setLayout(&imagesLayout);

    // Combines them into the main layout
    QHBoxLayout mainLayout(this);
    mainLayout.addWidget(&imagesView);
    this->setLayout(&mainLayout);
}


