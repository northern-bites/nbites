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
	//topImage(new BMPYUVImage(dataManager->getMemory()->get<MRawImages>(), Camera::TOP,
		//                                 BMPYUVImage::RGB, this)),
	//bottomImage(new BMPYUVImage(dataManager->getMemory()->get<MRawImages>(), Camera::BOTTOM,
	//                          BMPYUVImage::RGB, this)),
	// topChannelImage(topImage, this),
	// bottomChannelImage(bottomImage, this),
		currentColorSpace(&colorSpace[STARTING_COLOR]),
		colorSpaceWidget(currentColorSpace, this),
		colorWheel(currentColorSpace, this),

		currentCamera(Camera::TOP),
		topConverter(Camera::TOP),
		bottomConverter(Camera::BOTTOM),
		topDisplay(this),
		bottomDisplay(this),
		topImageListener(this),
		bottomImageListener(this),
		topImage(base()),
		bottomImage(base())
  {

	mainLayout = new QVBoxLayout;
  
	topLayout = new QHBoxLayout;

	//Adds modules to diagram then wires them together
	subdiagram.addModule(topConverter);
	subdiagram.addModule(bottomConverter);
	subdiagram.addModule(topImageListener);
	subdiagram.addModule(bottomImageListener);

	topConverter.imageIn.wireTo(&topImage, true);
	bottomConverter.imageIn.wireTo(&bottomImage, true);
	topImageListener.imageIn.wireTo(&topImage, true);
	bottomImageListener.imageIn.wireTo(&bottomImage, true);

    //connect all the color spaces to update the thresholded
    //image when their parameters change
    for (int i = 0; i < image::Color::NUM_COLORS; i++) {
        connect(&colorSpace[i], SIGNAL(parametersChanged()),
                this, SLOT(updateThresholdedImage()));
    }

	imageTabs->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Expanding);
    topLayout->addWidget(imageTabs);
	thresholdedImagePlaceholder.setAlignment(Qt::AlignCenter);
	topLayout->addWidget(&thresholdedImagePlaceholder);

    QObject::connect(&topDisplay, SIGNAL(mouseClicked(int, int, int, bool)),
                      this, SLOT(canvassClicked(int, int, int, bool)));
    QObject::connect(&bottomDisplay, SIGNAL(mouseClicked(int, int, int, bool)),
                      this, SLOT(canvassClicked(int, int, int, bool)));


    imageTabs->addTab(new QWidget, "Top Image");
    //dataManager->connectSlot(&topChannelImage, SLOT(updateView()), "MRawImages");

    imageTabs->addTab(new QWidget, "Bottom Image");
    //dataManager->connectSlot(&bottomChannelImage, SLOT(updateView()), "MRawImages");

    //update the threshold when the underlying image changes
    // dataManager->connectSlot(this, SLOT(updateThresholdedImage()), "MRawImages");
    // dataManager->connectSlot(this, SLOT(updateThresholdedImage()), "MRawImages");

    connect(imageTabs, SIGNAL(currentChanged(int)), this, SLOT(imageTabSwitched(int)));

    bottomLayout = new QHBoxLayout;
	colorButtons = new QVBoxLayout;
	leftJunk = new QVBoxLayout;

    //set up the color selection combo box
	for (int i = 0; i < image::Color::NUM_COLORS; i++) {
	  colorSelect.addItem(image::Color_label[i].c_str());
    }

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

	lastClickedX = -BOX;
	lastClickedY = -BOX;
}

// flip between one and all colors
	void ColorCalibrate::setFullColors(bool state) {
		displayAllColors = !displayAllColors;
		updateThresholdedImage();
	}

void ColorCalibrate::canvassClicked(int x, int y, int brushSize, bool leftClick) {
	std::cout << "Clicked " << x << " " << y << " " << std::endl;
	messages::YUVImage image;

    if (currentCamera == Camera::TOP) {
	  image = topImageIn.message();
    } else {
	  image = bottomImageIn.message();
    }


	const messages::MemoryImage8 yImage = image.yImage();
	const messages::MemoryImage8 uImage = image.uImage();
	const messages::MemoryImage8 vImage = image.vImage();

	byte yi = (byte)image.yImage().getPixel(x, y);
	byte u = (byte)image.uImage().getPixel(x, y);
	byte v = (byte)image.vImage().getPixel(x, y);

	std::cout << "YUV " << (int)yi << " " << (int)u << " " <<
		(int)v << std::endl;

	image::Color color;
	color.setYuv(yi, u, v);
	std::cout << "HSZ " << color.getH() << " " << color.getS() <<
		" " << color.getZ() << std::endl;

	currentColorSpace->verboseContains(color);
	if (!leftClick) {
		std::cout << "Right click " << std::endl;
		bool changed = currentColorSpace->expandToFit(color);
		if (changed) {
			colorSpaceWidget.setColorSpace(currentColorSpace);
		}
	}
	lastClickedX = x;
	lastClickedY = y;
	updateThresholdedImage();
	std::cout << std::endl;
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

    messages::YUVImage image;

    if (currentCamera == Camera::TOP) {
	  image = topImageIn.message();
    } else {
	  image = bottomImageIn.message();
    }

    //check for size changes and make sure
    //the thresholded image is the same size as the image
    if (thresholdedImage.width() != image.width()
            || thresholdedImage.height() != image.height()) {
        //TODO: should be ARGB premultiplied?
        thresholdedImage = QImage(image.width(),
                                  image.height(),
                                  QImage::Format_RGB32);
    }

	//const messages::YUVImage* yuvImage = image->getYUVImage();

	const messages::MemoryImage8 yImage = image.yImage();
	const messages::MemoryImage8 uImage = image.uImage();
	const messages::MemoryImage8 vImage = image.vImage();
    //threshold the image
    for (int j = 0; j < thresholdedImage.height(); j++) {
        QRgb* thresholdedImageLine = (QRgb*) (thresholdedImage.scanLine(j));
    
        for (int i = 0; i < thresholdedImage.width(); i++) {
		  image::Color color;
		  color.setYuv((byte)yImage.getPixel(i,j), (byte)uImage.getPixel(i,j),
					   (byte)vImage.getPixel(i,j));
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
			// lame hack to show a "cursor" on the thresholded image
			// TODO do a true cursor mirroring on the thresholded image
			if ((abs(j - lastClickedY) < BOX) && (abs(i - lastClickedX) < BOX)) {
			  tempColor = image::Color_RGB[3];
			}
            if (tempColor) {
                thresholdedImageLine[i] = (QRgb) tempColor;
            }
        }
    }
    //set it
    thresholdedImagePlaceholder.setPixmap(QPixmap::fromImage(thresholdedImage));
}

  void ColorCalibrate::run_()
  {
	topImageIn.latch();
	bottomImageIn.latch();

	topImage.setMessage(portals::Message<messages::YUVImage>(
										   &bottomImageIn.message()));
	bottomImage.setMessage(portals::Message<messages::YUVImage>(
										   &bottomImageIn.message()));
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

        //____FOR_FORMATTING____//
        //Write the first line
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

void ColorCalibrate::imageTabSwitched() {
    if (imageTabs->currentWidget() == &topDisplay) {
        currentCamera = Camera::TOP;
        this->updateThresholdedImage();
    }
    if (imageTabs->currentWidget() == &bottomDisplay) {
        currentCamera = Camera::BOTTOM;
        this->updateThresholdedImage();
    }
}

void ColorCalibrate::loadSlidersBtnPushed() {
  //QString base_directory = QString(NBITES_DIR) + "/data/sliders";
	//replaces NBITES_DIR with what (most of us) should have it as.
	//QUICK AND DIRTY
	QString base_directory = QString("~/nbites") + "/data/sliders";
    QString filename =
        QFileDialog::getOpenFileName(this,
                     tr("Load Sliders from File"),
                     base_directory,
                     tr("Slider files (*.sld)"));
    //this->loadColorSpaces(filename);
}

void ColorCalibrate::saveSlidersBtnPushed() {
    QString base_directory = QString("~/nbites") + "/data/sliders";
    QString filename =
            QFileDialog::getSaveFileName(this,
                    tr("Save Sliders to File"),
                    base_directory + "/new_sliders.sld",
                    tr("Slider files (*.sld)"));
    //this->writeColorSpaces(filename);
}

void ColorCalibrate::saveColorTableBtnPushed() {
    QString base_directory = QString("~/nbites") + "/data/tables";
    QString filename =
            QFileDialog::getSaveFileName(this,
                    tr("Save Color Table to File"),
                    base_directory + "/new_table.mtb",
                    tr("Color Tables(*.mtb)"));
	color::ColorTable::writeFromSliders(filename, colorSpace);
}

}
}
