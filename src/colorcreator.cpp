#include "colorcreator.h"
#include "ui_colorcreator.h"

#include <QTextStream>
#include <QFileDialog>
#include <QString>
#include <QStringRef>
#include <QRgb>
#include <QImage>
#include <QPixmap>
#include <boost/shared_ptr.hpp>

ColorCreator::ColorCreator(QWidget *parent) :
    QWidget(parent),
    roboImage(new RoboImage(WIDTH, HEIGHT)),
    yuvImage(new YUVImage(roboImage)),
    imageWindow(),
    ui(new Ui::ColorCreator),
    pImage(new memory::proto::PImage())
{

    ui->setupUi(this);
}

ColorCreator::~ColorCreator()
{
    delete ui;
}

void ColorCreator::updateDisplays()
{
    /*RoboImage::DisplayModes mode = RoboImage::Color;
    QImage img = roboimage.fast();
    QPixmap display;
    display.convertFromImage(img);
    //ui->BaseImage->setPixmap(display);*/
    imageWindow.show();
    imageWindow.repaint();
    QString next;
    next.setNum(currentFrameNumber+1, 10);
    QString prev;
    prev.setNum(currentFrameNumber-1, 10);
    nextFrame = currentDirectory + "/" + next + EXTENSION;
    previousFrame = currentDirectory + "/" + prev + EXTENSION;
    haveFile = true;
    QTextStream out(stdout);
//    updateThresh();
}

void ColorCreator::updateColors()
{
    // we draw by using a QImage - turn it into a Pixmap, then put it on a label
    QImage img(200, 200, QImage::Format_RGB32);
    bool display;
    QColor c;
    /* Our color wheel has a radius of 100.  Loop through the rectangle
      looking for pixels within that radius. For good pixels we calculate
      the H value based on the angle from the origin.  The S value is
      set according to the distance / radius, and the V is fixed (but
      settable by a slider).
     */
    for (int i = 0; i < 200; i++)
    {
        for (int j = 0; j < 200; j++)
        {
            float dist = sqrt((i - 100) * (i - 100) + (j - 100) * (j - 100));
            if (dist < 100.0)
            {
                display = true;
                float s = dist / 100.0f;
                float h = atan2(i - 100, j - 100) / (2.0f * 3.14156);
                if (h < 0)
                {
                    h = 1.0f + h;
                }
                // Since H is an angle the math is modulo.
                if (hMax[currentColor] > hMin[currentColor])
                {
                    if (hMin[currentColor] > h || hMax[currentColor] < h)
                    {
                        display = false;
                    }
                } else if (hMin[currentColor] > h && hMax[currentColor] < h )
                {
                    display = false;
                }
                if (s < sMin[currentColor] || s > sMax[currentColor])
                {
                    display = false;
                }
                if (display)
                {
                    c.setHsvF(h, s, zSlice);
                } else{
                    c.setHsvF(0.0, 0.0, 1.0f);
                }
            } else{
                c.setHsvF(0.0, 0.0, 1.0f);
            }
            img.setPixel(i, j, c.rgb());
        }
    }
    QPixmap pix;
    pix.convertFromImage(img);
    ui->colorWheel->setPixmap(pix);
    ui->colorWheel->repaint();
//    updateThresh();
}

void ColorCreator::on_pushButton_clicked()
{
    currentDirectory = QFileDialog::getOpenFileName(this, tr("Open Image"),
                                            currentDirectory,
                                            tr("Image Files (*.log)"));
    //    roboimage.read(currentDirectory);

    fp = new memory::log::MessageParser(pImage, currentDirectory.toStdString().data());
    fp->getNextMessage();
    roboImage->updateImage(pImage->image().data());
    yuvImage->updateFromRoboImage();
    int last = currentDirectory.lastIndexOf("/");
    int period = currentDirectory.lastIndexOf(".") - last - 1;
    QString temp = currentDirectory.mid(last+1, period);
    bool ok;
    currentFrameNumber = temp.toInt(&ok, 10);
    currentDirectory.chop(currentDirectory.size() - last);
    updateDisplays();
}

void ColorCreator::on_previousButton_clicked()
{
    //roboimage.read(previousFrame);
	roboImage->updateImage(pImage->image().data());
	    yuvImage->updateFromRoboImage();
	    currentFrameNumber--;
    updateDisplays();
}

void ColorCreator::on_nextButton_clicked()
{
  //  string s = SimpleParser::getNextImage();
  //  roboimage.read(s);
	roboImage->updateImage(pImage->image().data());
	    yuvImage->updateFromRoboImage();
	    currentFrameNumber++;
    updateDisplays();
}


void ColorCreator::on_hMin_valueChanged(int value)
{
    hMin[currentColor] = (float)value / 100.0f;
    updateColors();
    QTextStream out(stdout);
    out << "Set H Min value to " << value << "\n";
}

void ColorCreator::on_hMax_valueChanged(int value)
{
    hMax[currentColor] = (float)value / 100.0f;
    updateColors();
    QTextStream out(stdout);
    out << "Set H Max value to " << value << "\n";
}

void ColorCreator::on_sMin_valueChanged(int value)
{
    sMin[currentColor] = (float)value / 100.0f;
    updateColors();
    QTextStream out(stdout);
    out << "Set S Min value to " << value << "\n";
}

void ColorCreator::on_sMax_valueChanged(int value)
{
    sMax[currentColor] = (float)value / 100.0f;
    updateColors();
    QTextStream out(stdout);
    out << "Set S Max value to " << value << "\n";
}

void ColorCreator::on_yMin_valueChanged(int value)
{
    yMin[currentColor] = value;
    updateColors();
    QTextStream out(stdout);
    out << "Set Y Min value to " << value << "\n";
}

void ColorCreator::on_yMax_valueChanged(int value)
{
    yMax[currentColor] = value;
    updateColors();
    QTextStream out(stdout);
    out << "Set Y Max value to " << value << "\n";
}

void ColorCreator::on_zSlice_valueChanged(int value)
{
    zSlice = (float)value / 100.0f;
    updateColors();
}

void ColorCreator::on_colorSelect_currentIndexChanged(int index)
{
    currentColor = index;
    ui->hMin->setValue(hMin[currentColor] * 100);
    ui->hMax->setValue(hMax[currentColor] * 100);
    ui->sMin->setValue(sMin[currentColor] * 100);
    ui->sMax->setValue(sMax[currentColor] * 100);
    ui->zMin->setValue(zMin[currentColor] * 100);
    ui->zMax->setValue(zMax[currentColor] * 100);
    ui->yMin->setValue(yMin[currentColor]);
    ui->yMax->setValue(yMax[currentColor]);
}

void ColorCreator::on_viewChoice_currentIndexChanged(int index)
{
    mode = index;
//    updateThresh();
}

void ColorCreator::on_zMin_valueChanged(int value)
{
    zMin[currentColor] = (float)value / 100.0f;
    updateColors();
    QTextStream out(stdout);
    out << "Set Z Min value to " << value << "\n";
}

void ColorCreator::on_zMax_valueChanged(int value)
{
    zMax[currentColor] = (float)value / 100.0f;
    updateColors();
    QTextStream out(stdout);
    out << "Set Z Max value to " << value << "\n";
}
