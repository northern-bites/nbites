#include "ColorCalibrate.h"
#include "Camera.h"

#include <QtDebug>

#include <QFileDialog>
#include <QCheckBox>
#include <QMouseEvent>

typedef unsigned char byte;

namespace tool {
namespace colorcreator{

static const int BOX = 5;

ColorCalibrate::ColorCalibrate(QWidget *parent) :
    QWidget(parent),
    imageTabs(new QTabWidget(this)),
    currentColorSpace(&colorSpace[STARTING_COLOR]),
    colorSpaceWidget(currentColorSpace, this),
    colorWheel(currentColorSpace, this),
    currentCamera(Camera::TOP),
    topConverter(Camera::TOP),
    bottomConverter(Camera::BOTTOM),
    topDisplay(this),
    bottomDisplay(this),
    topThrDisplay(this),
    botThrDisplay(this),
    topImage(base()),
    bottomImage(base())
{

    mainLayout = new QVBoxLayout;

    topLayout = new QHBoxLayout;

    //Adds modules to diagram then wires them together
    subdiagram.addModule(topConverter);
    subdiagram.addModule(bottomConverter);
    subdiagram.addModule(topDisplay);
    subdiagram.addModule(bottomDisplay);
    subdiagram.addModule(topThrDisplay);
    subdiagram.addModule(botThrDisplay);

    topConverter.imageIn.wireTo(&topImage, true);
    bottomConverter.imageIn.wireTo(&bottomImage, true);
    topDisplay.imageIn.wireTo(&topImage, true);
    bottomDisplay.imageIn.wireTo(&bottomImage, true);
    topThrDisplay.imageIn.wireTo(&topConverter.thrImage);
    botThrDisplay.imageIn.wireTo(&bottomConverter.thrImage);

    //connect all the color spaces to update the thresholded
    //image when their parameters change
    for (int i = 0; i < image::Color::NUM_COLORS; i++) {
        connect(&colorSpace[i], SIGNAL(parametersChanged()),
                this, SLOT(updateThresholdedImage()));
    }

    imageTabs->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Expanding);
    topLayout->addWidget(imageTabs);
 
	//Used so that we can get two widgets in the same tab
	topImageLayout = new QHBoxLayout;
	bottomImageLayout = new QHBoxLayout;

	topImageLayout->addWidget(&topDisplay);
	topImageLayout->addWidget(&topThrDisplay);
	bottomImageLayout->addWidget(&bottomDisplay);
	bottomImageLayout->addWidget(&botThrDisplay);

	QWidget* topImagesTogether = new QWidget;
	QWidget* bottomImagesTogether = new QWidget; 
	topImagesTogether->setLayout(topImageLayout);
	bottomImagesTogether->setLayout(bottomImageLayout);

	imageTabs->addTab(topImagesTogether, "Top Image");
	imageTabs->addTab(bottomImagesTogether, "Bottom Image");

    connect(imageTabs, SIGNAL(currentChanged(int)),
            this, SLOT(imageTabSwitched(int)));

    bottomLayout = new QHBoxLayout;
    colorButtons = new QVBoxLayout;
    leftJunk = new QVBoxLayout;

    //set up the color selection combo box
    for (int i = 0; i < image::Color::NUM_COLORS; i++) {
        colorSelect.addItem(image::Color_label[i].c_str());
    }

	//All of the other Qt stuff
    colorSelect.setCurrentIndex(0);
    connect(&colorSelect, SIGNAL(currentIndexChanged(int)),
            this, SLOT(selectColorSpace(int)));
    leftJunk->addWidget(&colorSelect);
    leftJunk->addWidget(&colorWheel);

    bottomLayout->addLayout(leftJunk);
    bottomLayout->addWidget(&colorSpaceWidget);

    loadSlidersBtn.setText("Load Sliders");
    colorButtons->addWidget(&loadSlidersBtn);
    connect(&loadSlidersBtn, SIGNAL(clicked()),
            this, SLOT(loadSlidersBtnPushed()));

    saveSlidersBtn.setText("Save Sliders");
    colorButtons->addWidget(&saveSlidersBtn);
    connect(&saveSlidersBtn, SIGNAL(clicked()),
            this, SLOT(saveSlidersBtnPushed()));

    loadColorTableBtn.setText("Load Color Table");
    colorButtons->addWidget(&loadColorTableBtn);
    connect(&loadColorTableBtn, SIGNAL(clicked()),
            this, SLOT(loadColorTableBtnPushed()));

    saveColorTableBtn.setText("Save Color Table");
    colorButtons->addWidget(&saveColorTableBtn);
    connect(&saveColorTableBtn, SIGNAL(clicked()),
            this, SLOT(saveColorTableBtnPushed()));

    bottomLayout->addLayout(colorButtons);

    QCheckBox* fullColors = new QCheckBox(tr("All colors"));
    colorButtons->addWidget(fullColors);
    connect(fullColors, SIGNAL(toggled(bool)), this, SLOT(setFullColors(bool)));
    displayAllColors = false;

    mainLayout->addLayout(topLayout);
    bottomLayout->setAlignment(Qt::AlignBottom);
    mainLayout->addLayout(bottomLayout);

    this->setLayout(mainLayout);
}

// flip between one and all colors
void ColorCalibrate::setFullColors(bool state) {
    displayAllColors = state;
    updateThresholdedImage();
}

void ColorCalibrate::selectColorSpace(int index) {
    currentColorSpace = &colorSpace[index];
    colorWheel.setColorSpace(currentColorSpace);
    colorSpaceWidget.setColorSpace(currentColorSpace);
    if (!displayAllColors) {
        updateThresholdedImage();
    }
}

// TODO: Ideally we'd want to have this in a separate class
// or unify this with our regular thresholding process (maybe
// by converting the color space parameters to a table continuously?
void ColorCalibrate::updateThresholdedImage() {
    subdiagram.run();

    topConverter.initTable(colorTable.getTable());
    bottomConverter.initTable(colorTable.getTable());


    //threshold the top image
    messages::YUVImage image;
    image = topImageIn.message();

    //check for size changes and make sure
    //the thresholded image is the same size as the image
    if (thresholdedImage.width() != image.width()
        || thresholdedImage.height() != image.height()) {
        //TODO: should be ARGB premultiplied?
	  thresholdedImage = QImage(image.width()/2,
                                  image.height(),
                                  QImage::Format_RGB32);
    }

	//Seperates out the 3 parts of the YUV image
    const messages::MemoryImage8 yImage = image.yImage();
    const messages::MemoryImage8 uImage = image.uImage();
    const messages::MemoryImage8 vImage = image.vImage();

    // Get the image being thresholded on
    // messages::YUVImage image;
    if (currentCamera == Camera::TOP) {
        image = topImageIn.message();
    } else {
        image = bottomImageIn.message();
    }

    //threshold the image
    for (int j = 0; j < thresholdedImage.height(); j++) {
	    //We threshold an image by grabbing each line and thresholding that line
        QRgb* thresholdedImageLine = (QRgb*) (thresholdedImage.scanLine(j));
		
        for (int i = 0; i < thresholdedImage.width(); i++) {
            image::Color color;
			//The division by two is because the u and v images are not as wide
			//as the thresholded image. Due to making the threshodedImage a QImage
		    color.setYuv((byte)yImage.getPixel(i,j), (byte)uImage.getPixel(i/2,j),
                         (byte)vImage.getPixel(i/2,j));
            //default color
            thresholdedImageLine[i] = image::Color::Grey;
            //temporary variables for blending colors
            int count = 0;
            long long tempColor = 0;
            for (int c = 0; c < image::Color::NUM_COLORS; c++) {
                if (colorSpace[c].contains(color) &&
                    (displayAllColors || currentColorSpace == &colorSpace[c])) {
                    //blend colors in by averaging them
                    tempColor *= count;
                    tempColor += image::Color_RGB[c];
                    count++;
                    tempColor /= count;
                }
            }
			//We now have a color for this pixel
			thresholdedImageLine[i] = tempColor;
        }
    }

    // //set it
    //thresholdedImagePlaceholder.setPixmap(QPixmap::fromImage(thresholdedImage));
}

void ColorCalibrate::run_()
{
    topImageIn.latch();
    bottomImageIn.latch();

    topImage.setMessage(portals::Message<messages::YUVImage>(
                            &topImageIn.message()));
    bottomImage.setMessage(portals::Message<messages::YUVImage>(
                               &bottomImageIn.message()));

  	subdiagram.run();
    updateThresholdedImage();
}


void ColorCalibrate::loadColorSpaces(QString filename) {

    QFile dataFile(filename);
    qDebug() << "Attempt to open filename = " << filename << endl;
    if (dataFile.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        qDebug() << "Succeed" << endl;
        QString nextString;
        QTextStream dataFileStream(&dataFile);

        //_____FOR_FORMATTING____//
        //read the first line
        dataFileStream.readLine();

        for (int i = 0; i < image::Color::NUM_COLORS; i++)
        {
            //read the dummy label
            dataFileStream >> nextString;
            float parameters[ColorSpace::NUM_CHANNELS];
            for (int j=0; j < ColorSpace::NUM_CHANNELS; j++)
            {
                dataFileStream >> nextString;
                parameters[j] = nextString.toFloat();
            }
            colorSpace[i].setParametersSilently(parameters);
        }
    }
    //reset to the default color space
    colorSelect.setCurrentIndex(STARTING_COLOR);
    selectColorSpace(STARTING_COLOR);
    updateThresholdedImage();
}

void ColorCalibrate::writeColorSpaces(QString filename) {
    //Create the file to store the current values
    QFile newFile(filename);
    if (newFile.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        QTextStream newFileStream(&newFile);

        //Write the first line for FORMATTING
        for (int i = 0; i < ColorSpace::NUM_CHANNELS; i++) {
            newFileStream << color::fltChannel_names[i].c_str() << " ";
        }
        newFileStream << endl;

        for (int i = 0; i < image::Color::NUM_COLORS; i++)
        {
            newFileStream << image::Color_label[i].c_str() << " ";
            for (int j = 0; j < ColorSpace::NUM_CHANNELS; j++) {
                newFileStream << colorSpace[i].
                    getParameter(ColorSpace::Channel(j)) << " ";
            }
            newFileStream << endl;
        }
    }
}

void ColorCalibrate::imageTabSwitched(int i) {
    if (imageTabs->currentWidget() == &topDisplay) {
        currentCamera = Camera::TOP;
        topThrDisplay.imageIn.wireTo(&topConverter.thrImage);
    }
    if (imageTabs->currentWidget() == &bottomDisplay) {
        currentCamera = Camera::BOTTOM;
        botThrDisplay.imageIn.wireTo(&bottomConverter.thrImage);
    }
    updateThresholdedImage();
}

// NOTE: use ~/nbites to find directory instead of NBITES_DIR
// ## HACK ##

void ColorCalibrate::loadSlidersBtnPushed() {
    QString base_directory = QString(NBITES_DIR) + "/data/sliders";
    QString filename =
        QFileDialog::getOpenFileName(this,
                                     tr("Load Sliders from File"),
                                     base_directory,
                                     tr("Slider files (*.sld)"));
    this->loadColorSpaces(filename);
}

void ColorCalibrate::saveSlidersBtnPushed() {
    QString base_directory = QString(NBITES_DIR) + "/data/sliders";
    QString filename =
        QFileDialog::getSaveFileName(this,
                                     tr("Save Sliders to File"),
                                     base_directory + "/new_sliders.sld",
                                     tr("Slider files (*.sld)"));
    this->writeColorSpaces(filename);
}

void ColorCalibrate::loadColorTableBtnPushed() {
    QString base_directory = QString(NBITES_DIR) + "/data/tables";
    QString filename = QFileDialog::getOpenFileName(this,
                    tr("Load Color Table from File"),
                    base_directory,
                    tr("Color Table files (*.mtb)"));
    colorTable.read(filename.toStdString());
}

void ColorCalibrate::saveColorTableBtnPushed() {
    QString base_directory = QString(NBITES_DIR) + "/data/tables";
    QString filename =
        QFileDialog::getSaveFileName(this,
                                     tr("Save Color Table to File"),
                                     base_directory + "/new_table.mtb",
                                     tr("Color Tables(*.mtb)"));
    color::ColorTable::writeFromSliders(filename, colorSpace);
}

}
}
