#include "MainView.h"
#include <QBoxLayout>
#include <QLabel>

MainWidget::MainWidget(QWidget* parent) : QWidget(parent),
                                          world(),
                                          topImage(world, TOP),
                                          bottomImage(world, BOTTOM),
                                          topImageView(topImage),
                                          bottomImageView(bottomImage),
                                          worldView(world),
                                          controls(this)
{
    // For the two images
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

    // For the world view and controls
    QWidget* worldControlView = new QWidget(this);
    QVBoxLayout* worldControlLayout = new QVBoxLayout(worldControlView);

    QLabel* worldLabel = new QLabel(tr("World State"));
    worldControlLayout->addWidget(worldLabel);
    worldControlLayout->addWidget(&worldView);
    worldControlLayout->addWidget(&controls);

    worldControlView->setLayout(worldControlLayout);

    // Combines them into the main layout
    QHBoxLayout* mainLayout = new QHBoxLayout(this);
    mainLayout->addWidget(imagesView);
    mainLayout->addWidget(worldControlView);
    this->setLayout(mainLayout);

    QObject::connect(&controls, SIGNAL(robotMoved(float, float, float)),
                     this, SLOT(updateRobot(float, float, float)));
    QObject::connect(&controls, SIGNAL(headMoved(float, float)),
                     this, SLOT(updateHead(float, float)));
    QObject::connect(&controls, SIGNAL(ballMoved(float, float)),
                     this, SLOT(updateBall(float, float)));
}

void MainWidget::updateView()
{
    topImage.update();
    bottomImage.update();
    topImageView.repaint();
    bottomImageView.repaint();
    worldView.repaint();
}

MainView::MainView() : QMainWindow()
{
    this->setWindowTitle(tr("VisionSim"));
    this->setCentralWidget(new MainWidget(this));
}
