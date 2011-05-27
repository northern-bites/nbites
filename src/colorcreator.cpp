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
    roboimage(WIDTH, HEIGHT),
    imageWindow(roboimage),
    ui(new Ui::ColorCreator),
    pImage(new memory::proto::PImage())
{
    hMin = new float[COLORS];
    hMax = new float[COLORS];
    sMin = new float[COLORS];
    sMax = new float[COLORS];
    zMin = new float[COLORS];
    zMax = new float[COLORS];
    yMin = new int[COLORS];
    yMax = new int[COLORS];
    cols = new QColor[COLORS];

    // initialize colors for drawing thresholds
    cols[Orange] = QColor(255, 128, 0);
    cols[Blue] = QColor(0, 0, 255);
    cols[Green] = QColor(0, 201, 87);
    cols[Yellow] = QColor(255, 255, 0);
    cols[White] = QColor(255, 255, 255);
    cols[Pink] = QColor(255, 181, 197);
    cols[Navy] = QColor(0, 0, 128);
    cols[Black] = QColor(0, 0, 0);

    ui->setupUi(this);
    baseDirectory = "/Users/ericchown/nbites/data/frames";
    baseColorTable = "/Users/ericchown/nbites/data/tables";
    haveFile = false;

    ui->colorSelect->addItem(tr("Orange"), Orange);
    ui->colorSelect->addItem(tr("Blue"), Blue);
    ui->colorSelect->addItem(tr("Yellow"), Yellow);
    ui->colorSelect->addItem(tr("Green"), Green);
    ui->colorSelect->addItem(tr("White"), White);
    ui->colorSelect->addItem(tr("Pink"), Pink);
    ui->colorSelect->addItem(tr("Navy"), Navy);
    ui->colorSelect->addItem(tr("Black"), Black);

    ui->viewChoice->addItem(tr("Single Color"), Single);
    ui->viewChoice->addItem(tr("All Colors"), Multiple);
    ui->viewChoice->addItem(tr("Use Table"), Table);
    mode = Single;

    currentColor = Orange;
    currentDirectory = baseDirectory;
    currentColorDirectory = baseColorTable;
    zSlice = 0.75f;
    for (int i = 0; i < COLORS; i++)
    {
        switch(i)
        {
        case Orange:
            hMin[i] = 0.80f;
            hMax[i] = 0.06f;
            sMin[i] = 0.25f;
            sMax[i] = 1.0f;
            zMin[i] = 0.12f;
            zMax[i] = 1.0f;
            yMin[i] = 34;
            yMax[i] = 145;
            break;
        case Green:
            hMin[i] = 0.45f;
            hMax[i] = 0.60f;
            sMin[i] = 0.3f;
            sMax[i] = 0.6f;
            zMin[i] = 0.0f;
            zMax[i] = 1.0f;
            yMin[i] = 41;
            yMax[i] = 124;
            break;
        case Yellow:
            hMin[i] = 0.13f;
            hMax[i] = 0.36f;
            sMin[i] = 0.08f;
            sMax[i] = 0.70f;
            zMin[i] = 0.17f;
            zMax[i] = 1.0f;
            yMin[i] = 62;
            yMax[i] = 145;
            break;
        case Blue:
            hMin[i] = 0.58f;
            hMax[i] = 0.69f;
            sMin[i] = 0.42f;
            sMax[i] = 0.65f;
            zMin[i] = 0.40f;
            zMax[i] = 1.0f;
            yMin[i] = 46;
            yMax[i] = 146;
            break;
        case White:
            hMin[i] = 0.15f;
            hMax[i] = 0.71f;
            sMin[i] = 0.0f;
            sMax[i] = 0.3f;
            zMin[i] = 0.0f;
            zMax[i] = 1.0f;
            yMin[i] = 150;
            yMax[i] = 250;
            break;
        default:
            hMin[i] = 0.0f;
            hMax[i] = 0.01f;
            sMin[i] = 0.0f;
            sMax[i] = 1.0f;
            zMin[i] = 0.0f;
            zMax[i] = 1.0f;
            yMin[i] = 30;
            yMax[i] = 230;
            break;
        }
    }
    ui->hMin->setValue(hMin[currentColor] * 100);
    ui->hMax->setValue(hMax[currentColor] * 100);
    ui->sMin->setValue(sMin[currentColor] * 100);
    ui->sMax->setValue(sMax[currentColor] * 100);
    ui->zMin->setValue(zMin[currentColor] * 100);
    ui->zMax->setValue(zMax[currentColor] * 100);
    ui->yMin->setValue(yMin[currentColor]);
    ui->yMax->setValue(yMax[currentColor]);
    ui->zSlice->setValue(zSlice);
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

    fp = new memory::log::FileParser(pImage, currentDirectory.toStdString().data());
    fp->getNextMessage();
    roboimage.read(pImage->image());
    int last = currentDirectory.lastIndexOf("/");
    int period = currentDirectory.lastIndexOf(".") - last - 1;
    QString temp = currentDirectory.mid(last+1, period);
    bool ok;
    currentFrameNumber = temp.toInt(&ok, 10);
    currentDirectory.chop(currentDirectory.size() - last);
    updateDisplays();
}

#include <iostream>

void ColorCreator::on_previousButton_clicked()
{
    //roboimage.read(previousFrame);
    fp->getNextMessage();
    std::cout << pImage->timestamp() << std::endl;
    roboimage.read(pImage->image());
    currentFrameNumber--;
    updateDisplays();
}

void ColorCreator::on_nextButton_clicked()
{
  //  string s = SimpleParser::getNextImage();
  //  roboimage.read(s);
    fp->getNextMessage();
    std::cout << pImage->timestamp() << std::endl;
    roboimage.read(pImage->image());
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
