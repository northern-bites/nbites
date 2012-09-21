#include "MainView.h"
#include <QBoxLayout>
#include <QLabel>

MainWidget::MainWidget(QWidget* parent) : QWidget(parent),
                                          world(),
                                          topImage(world, TOP),
                                          bottomImage(world, BOTTOM),
                                          topImageView(topImage),
                                          bottomImageView(bottomImage),
                                          worldView(world)
{
    // For the image viewer and controls on the left
    QWidget* imagesView = new QWidget(this);
    QVBoxLayout* imagesLayout = new QVBoxLayout(imagesView);

    QWidget* topContainer = new QWidget(this);
    topContainer->setMinimumSize(IMAGE_WIDTH, IMAGE_HEIGHT);
    topImageView.setParent(topContainer);

    QWidget* bottomContainer = new QWidget(this);
    bottomContainer->setMinimumSize(IMAGE_WIDTH, IMAGE_HEIGHT);
    bottomImageView.setParent(bottomContainer);

    QLabel* topLabel = new QLabel(tr("Top Image"));
    QLabel* bottomLabel = new QLabel(tr("Bottom Image"));

    imagesLayout->addWidget(topLabel);
    imagesLayout->addWidget(topContainer);
    imagesLayout->addWidget(bottomLabel);
    imagesLayout->addWidget(bottomContainer);

    imagesView->setLayout(imagesLayout);

    // Combines them into the main layout
    QHBoxLayout* mainLayout = new QHBoxLayout(this);
    mainLayout->addWidget(imagesView);
    mainLayout->addWidget(&worldView);
    this->setLayout(mainLayout);
}

MainView::MainView() : QMainWindow()
{
    this->setWindowTitle(tr("VisionSim"));
    this->setCentralWidget(new MainWidget(this));
}
