/* Main file.  This is a color calibration tool (for now).  Pops up a window
  that can load image frames and display them in three ways:
  1) The raw image
  2) The image calibrated according to a color table, or this tool's controls
  3) Lots of other things (e.g. just the Y channel of the image)

  We make color tables with this tool by using its sliders.  The sliders control
  ranges (e.g. set a max and minimum value for Y) for each color.  Those define
  a region of color space that the color is defined for.  What this does is
  to quickly build a highly filled in color region.  For now we then go back
  to the old tool to refine the image.  In the future it would be nice to
  be able to refine directly in this tool.

  Some things we don't have yet:
  1) The ability to grab key images and save them into a "key image" directory
  2) The ability to run our vision code and display object results
  3) The ability to specify one color's dominance over another (e.g. if pink
  && orange then choose pink)
  4) Lots of other stuff I haven't thought of yet

  Things that aren't really done that well (this was intended as a prototype)
  1) Data management
  2) More or less everything - this tool is oddly slow, I don't know QT well enough
  to know why.
  */


#include "ColorCreator.h"
#include "ui_ColorCreator.h"
#include "math.h"

#include <QTextStream>
#include <QFileDialog>
#include <QString>
#include <QStringRef>
#include <QRgb>
#include <QImage>
#include <QPixmap>
#include <boost/shared_ptr.hpp>

namespace qtool {
namespace colorcreator {

using data::DataManager;
using man::memory::RoboImage;

ColorCreator::ColorCreator(DataManager::ptr dataManager, QWidget *parent) :
    QWidget(parent),
    dataManager(dataManager),
    ui(new Ui::ColorCreator),
    roboImage(dataManager->getMemory()->getMImage()),
    yuvImage(roboImage)
{
    // Display images
    img = new QImage(640, 480, QImage::Format_RGB32);
    img2 = new QImage(320, 240, QImage::Format_RGB32);
    img3 = new QImage(320, 240, QImage::Format_RGB32);
    img4 = new QImage(320, 240, QImage::Format_RGB32);
    wheel = new QImage(200, 200, QImage::Format_RGB32);
    // Each color gets its own value for everything specified by sliders
    fltSliders = new float*[FLT_SLIDERS];
    intSliders = new int*[INT_SLIDERS];

    register int i;
    for (i = 0; i < FLT_SLIDERS; ++i) {
      fltSliders[i] = new float[COLORS];
      if (i < INT_SLIDERS)
        intSliders[i] = new int[COLORS];
    }

    cols = new QColor[COLORS+SOFT];
    bitColor = new unsigned[COLORS];

    table = new ColorTable();

    // initialize colors for drawing thresholds
    cols[Orange] = QColor(255, 128, 0);
    cols[Blue] = QColor(100, 149, 237);
    cols[Green] = QColor(0, 139, 0);
    cols[Yellow] = QColor(255, 255, 0);
    cols[White] = QColor(255, 255, 255);
    cols[Pink] = QColor(255, 181, 197);
    cols[Navy] = QColor(0, 0, 205);
    cols[Black] = QColor(100, 100, 100);
    cols[OrangeRed] = QColor(238, 64, 0);
    cols[BlueGreen] = QColor(72, 209, 234);
    cols[BlueNavy] =  QColor(105, 89, 205);

    // initialize bitColors for generating color tables
    bitColor[Orange] = ORANGE_COL;
    bitColor[Blue] = BLUE_COL;
    bitColor[Yellow] = YELLOW_COL;
    bitColor[Green] = GREEN_COL;
    bitColor[White] = WHITE_COL;
    bitColor[Pink] = RED_COL;
    bitColor[Navy] = NAVY_COL;
    bitColor[Black] = GREY_COL;

    ui->setupUi(this);
    //  default directories - should not be user specific
    baseDirectory = "/home/nemo/Documents/school/super_fall/robotics_cs320/nbites/data/frames";
    baseColorTable = "/home/nemo/Documents/school/super_fall/robotics_cs320/nbites/data/tables";
    haveFile = false;
    viewerEnabled = false;

    tableMode = false;
    defineMode = false;
    cornerStatus = true;

    ui->modeSelect->addItem(tr("Define Mode"), 0); 
    ui->modeSelect->addItem(tr("Table Mode"), 1);

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


    ui->channel->addItem(tr("Y"), Y);
    ui->channel->addItem(tr("U"), U);
    ui->channel->addItem(tr("V"), V);
    ui->channel->addItem(tr("Blue"), Bluec);
    ui->channel->addItem(tr("Red"), Redc);
    ui->channel->addItem(tr("Green"), Greenc);
    ui->channel->addItem(tr("H"), H);
    ui->channel->addItem(tr("S"), S);
    ui->channel->addItem(tr("Z"), Z);
    ui->channel->addItem(tr("Edge"), EDGE);
    ui->channel->addItem(tr("Table"), Table);
    mode = Single;
    shape = Y;

    currentColor = Orange;
    currentDirectory = baseDirectory;
    currentColorDirectory = baseColorTable;
    zSlice = 0.75f;

    edgediff = 12;

    // initialize all of our values.  Ideally these will serve as a pretty good table
    // for virtually any environment
    // Note: it would be nice to be able to save values from various locations and
    // load the appropriate one here
    for (int i = 0; i < COLORS; i++)
    {
        switch(i)
        {
        case Orange:
            fltSliders[hMin][i] = 0.80f;
            fltSliders[hMax][i] = 0.13f;
            fltSliders[sMin][i] = 0.25f;
            fltSliders[sMax][i] = 1.0f;
            fltSliders[zMin][i] = 0.12f;
            fltSliders[zMax][i] = 1.0f;
            intSliders[yMin][i] = 34;
            intSliders[yMax][i] = 145;
            intSliders[vMin][i] = 115;
            intSliders[vMax][i] = 171;
            break;
        case Green:
            fltSliders[hMin][i] = 0.37f;
            fltSliders[hMax][i] = 0.45f;
            fltSliders[sMin][i] = 0.28f;
            fltSliders[sMax][i] = 0.46f;
            fltSliders[zMin][i] = 0.22f;
            fltSliders[zMax][i] = 0.53f;
            intSliders[yMin][i] = 55;
            intSliders[yMax][i] = 105;
            intSliders[vMin][i] = 90;
            intSliders[vMax][i] = 131;
            break;
        case Yellow:
            fltSliders[hMin][i] = 0.17f;
            fltSliders[hMax][i] = 0.26f;
            fltSliders[sMin][i] = 0.32f;
            fltSliders[sMax][i] = 0.69f;
            fltSliders[zMin][i] = 0.27f;
            fltSliders[zMax][i] = 0.48f;
            intSliders[yMin][i] = 56;
            intSliders[yMax][i] = 105;
            intSliders[vMin][i] = 111;
            intSliders[vMax][i] = 128;
            break;
        case Blue:
            fltSliders[hMin][i] = 0.54f;
            fltSliders[hMax][i] = 0.67f;
            fltSliders[sMin][i] = 0.30f;
            fltSliders[sMax][i] = 0.65f;
            fltSliders[zMin][i] = 0.23f;
            fltSliders[zMax][i] = 0.48f;
            intSliders[yMin][i] = 33;
            intSliders[yMax][i] = 105;
            intSliders[vMin][i] = 109;
            intSliders[vMax][i] = 127;
            break;
        case White:
            fltSliders[hMin][i] = 0.01f;
            fltSliders[hMax][i] = 0.01f;
            fltSliders[sMin][i] = 0.0f;
            fltSliders[sMax][i] = 0.38f;
            fltSliders[zMin][i] = 0.39f;
            fltSliders[zMax][i] = 1.0f;
            intSliders[yMin][i] = 102;
            intSliders[yMax][i] = 250;
            intSliders[vMin][i] = 99;
            intSliders[vMax][i] = 128;
            break;
        case Pink:
            fltSliders[hMin][i] = 0.75f;
            fltSliders[hMax][i] = 0.22f;
            fltSliders[sMin][i] = 0.0f;
            fltSliders[sMax][i] = 0.29f;
            fltSliders[zMin][i] = 0.21f;
            fltSliders[zMax][i] = 0.54f;
            intSliders[yMin][i] = 58;
            intSliders[yMax][i] = 139;
            intSliders[vMin][i] = 127;
            intSliders[vMax][i] = 143;
            break;
        case Navy:
            fltSliders[hMin][i] = 0.57f;
            fltSliders[hMax][i] = 0.68f;
            fltSliders[sMin][i] = 0.23f;
            fltSliders[sMax][i] = 0.42f;
            fltSliders[zMin][i] = 0.17f;
            fltSliders[zMax][i] = 0.45f;
            intSliders[yMin][i] = 39;
            intSliders[yMax][i] = 105;
            intSliders[vMin][i] = 106;
            intSliders[vMax][i] = 132;
            break;
        default:
            fltSliders[hMin][i] = 0.0f;
            fltSliders[hMax][i] = 0.01f;
            fltSliders[sMin][i] = 0.99f;
            fltSliders[sMax][i] = 1.0f;
            fltSliders[zMin][i] = 0.0f;
            fltSliders[zMax][i] = 1.0f;
            intSliders[yMin][i] = 30;
            intSliders[yMax][i] = 230;
            intSliders[vMin][i] = 40;
            intSliders[vMax][i] = 150;
            break;
        }
    }
    // set the sliders to start at correct values
    ui->hMin->setValue(fltSliders[hMin][currentColor] * 100);
    ui->hMax->setValue(fltSliders[hMax][currentColor] * 100);
    ui->sMin->setValue(fltSliders[sMin][currentColor] * 100);
    ui->sMax->setValue(fltSliders[sMax][currentColor] * 100);
    ui->zMin->setValue(fltSliders[zMin][currentColor] * 100);
    ui->zMax->setValue(fltSliders[zMax][currentColor] * 100);
    ui->yMin->setValue(intSliders[yMin][currentColor]);
    ui->yMax->setValue(intSliders[yMax][currentColor]);
    ui->vMin->setValue(intSliders[vMin][currentColor]);
    ui->vMax->setValue(intSliders[vMax][currentColor]);
    ui->zSlice->setValue(zSlice);

    firstPoint.setX(-1);
    firstPoint.setY(-1);
    setMouseTracking(true);
}

ColorCreator::~ColorCreator()
{
    delete ui;
}

//TODO: hack hack hack
// we need to get IMAGE_WIDTH and HEIGHT from roboImage
// and X and Y are the offset of the large display image - get those from the widget
#define IMAGE_X 50
#define IMAGE_Y 60
#define IMAGE_WIDTH 640
#define IMAGE_HEIGHT 480

void ColorCreator::mouseMoveEvent(QMouseEvent *event)
{
    QTextStream out(stdout);
    QPoint thePoint = event->pos();
    int x = thePoint.x();
    int y = thePoint.y();
    if (x > IMAGE_X && x < IMAGE_X + IMAGE_WIDTH && y > IMAGE_Y && y < IMAGE_Y + IMAGE_HEIGHT && haveFile) {
        x = x - IMAGE_X;
        y = y - IMAGE_Y;
        int Y = yuvImage.getY(x, y);
        int U = yuvImage.getU(x, y);
        int V = yuvImage.getV(x, y);
        int H = yuvImage.getH(x, y);
        int S = yuvImage.getS(x, y);
        int Z = yuvImage.getZ(x, y);
        out << "YUV :" << Y << " " << U << " " << V << "\n";
        out << "HSZ :" << H << " " << S << " " << Z << "\n";
    }
}

void ColorCreator::mouseReleaseEvent(QMouseEvent *event)
{
    lastPoint = event->pos();
    lastPoint.setX(lastPoint.x() - IMAGE_X);
    lastPoint.setY(lastPoint.y() - IMAGE_Y);
    if (lastPoint.x() - firstPoint.x() > 20) {
        largeDisplay();
    }
}

void ColorCreator::mousePressEvent(QMouseEvent *event)
{
    firstPoint = event->pos();
    firstPoint.setX(firstPoint.x() - IMAGE_X);
    firstPoint.setY(firstPoint.y() - IMAGE_Y);
}

void ColorCreator::updateDisplays()
{
    QString next;
    next.setNum(currentFrameNumber+1, 10);
    QString prev;
    prev.setNum(currentFrameNumber-1, 10);
    QString plusTen;
    plusTen.setNum(currentFrameNumber+10, 10);
    QString minusTen;
    minusTen.setNum(currentFrameNumber-10, 10);
    nextFrame = currentDirectory + "/" + next + EXTENSION;
    previousFrame = currentDirectory + "/" + prev + EXTENSION;
    tenthFrame = currentDirectory + "/" + plusTen + EXTENSION;
    minusTenthFrame = currentDirectory + "/" + plusTen + EXTENSION;
    haveFile = true;
    updateThresh(true, false, false);
}

/* We display a color wheel to indicate the current color's region.  It is a 2D slice
  in 3D space (slicing on the V dimension (called Z here)) of HSV space.
  Here we just loop through the display area, calculate an angle and distance to the
  center (angle = H, distance = S) and determine whether any given pixel ought to
  be displayed for the current color settings.
  */
void ColorCreator::updateColors()
{
    // we draw by using a QImage - turn it into a Pixmap, then put it on a label
    //QImage img(200, 200, QImage::Format_RGB32);
    bool display;
    QColor c;
    if (!tableMode) {
      /* Our color wheel has a radius of 100.  Loop through the rectangle
      looking for pixels within that radius. For good pixels we calculate
      the H value based on the angle from the origin.  The S value is
      set according to the distance / radius, and the V is fixed (but
      settable by a slider). */
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
                    if (fltSliders[hMax][currentColor] > fltSliders[hMin][currentColor])
                    {
                        if (fltSliders[hMin][currentColor] > h || fltSliders[hMax][currentColor] < h)
                        {
                            display = false;
                        }
                    } else if (fltSliders[hMin][currentColor] > h && fltSliders[hMax][currentColor] < h )
                    {
                        display = false;
                    }
                    if (s < fltSliders[sMin][currentColor] || s > fltSliders[sMax][currentColor])
                    {
                        display = false;
                    }
                    ColorSpace col;
                    col.setHsz(h, s, zSlice);
                    int y = col.getYb();
                    int v = col.getVb();
                    if (y < intSliders[yMin][currentColor] || y > intSliders[yMax][currentColor])
                    {
                        display = false;
                    }
                    if (v < intSliders[vMin][currentColor] || v > intSliders[vMax][currentColor])
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
                wheel->setPixel(i, j, c.rgb());
            }
        }
        QPixmap pix;
        pix.convertFromImage(*wheel);
        ui->colorWheel->setPixmap(pix);
        ui->colorWheel->repaint();
        updateThresh(false, false, true);
    }
}

/* One of our displays is typically used to display a single color
  channel of our images.  That is controlled here.  Can also display
  edge information if desired.
  */
QColor ColorCreator::getChannelView(int j, int i)
{
    bool found;
    int red, green, blue, edge;
    switch (shape) {
    case V:
        red = green = blue = yuvImage.getV(j, i);
        break;
    case Bluec:
        red = green = 0;
        blue = yuvImage.getBlue(j, i);
        break;
    case Y:
        red = green = blue = yuvImage.getY(j, i);
        break;
    case U:
        red = green = blue = yuvImage.getU(j, i);
        break;
    case Greenc:
        red = blue = 0;
        green = yuvImage.getGreen(j, i);
        break;
    case Redc:
        blue = green = 0;
        red = yuvImage.getRed(j, i);
        break;
    case H:
        red = green = blue = yuvImage.getH(j, i);
        break;
    case S:
        red = green = blue = yuvImage.getS(j, i);
        break;
    case Z:
        red = green = blue = yuvImage.getZ(j, i);
        break;
    case Table:
        if (table->isEnabled())
        {
            return displayColorTable(j, i);
        }
        break;
    case EDGE:
        red = green = blue = 255; //yuvImage.getY(j, i);
        found = false;
        if (j > 0 && i > 1) {
            edge = abs(yuvImage.getY(j - 1, i) - yuvImage.getY(j, i));
            edge = max(abs(yuvImage.getY(j, i) - yuvImage.getY(j, i - 1)), edge);
            if (edge  > edgediff) {
                red = 255;
                green = 0;
                blue = 0;
                found = true;
            }
            edge = abs(yuvImage.getU(j - 1, i) - yuvImage.getU(j, i));
            edge = max(abs(yuvImage.getU(j, i) - yuvImage.getU(j, i - 1)), edge);
            if (edge > edgediff) {
                green = 255;
                blue = 0;
                if (!found) {
                    red = 0;
                }
                found = true;
            }
            edge = abs(yuvImage.getV(j - 1, i) - yuvImage.getV(j, i));
            edge = max(abs(yuvImage.getV(j, i) - yuvImage.getV(j, i - 1)), edge);
            if (edge > edgediff) {
                blue = 255;
                if (!found) {
                    red = 0;
                    green = 0;
                }
            }
        }
        break;
    }
    QColor col(red, green, blue);
    return col;
}

/* One of our displays normally displays a thresholded image that is thresholded
  according to our sliders.  However, we can also display an image that is
  thresholded according to a color table.  This is mainly to check that our
  tables/tools are working as desired.
  @param i      The x value in the image
  @param j      The y value in the image
  @return       The thresholded color for that pixel
  */
QColor ColorCreator::displayColorTable(int i, int j)
{
    QColor c;

    int y = yuvImage.getY(i, j);
    int u = yuvImage.getU(i, j);
    int v = yuvImage.getV(i, j);
    unsigned col = table->index(y, u, v);
    if ((col & ORANGE_COL) && (col & RED_COL))
    {
        c = cols[OrangeRed];
    }
    else if (col & ORANGE_COL) {
        c = cols[Orange];
    }
    else if ((col & GREEN_COL) && (col & BLUE_COL))
    {
        c = cols[BlueGreen];
    } else if ((col & GREEN_COL))
    {
        c = cols[Green];
    }
    else if ((col & WHITE_COL))
    {
        c = cols[White];
    }
    else if ((col & BLUE) && (col & NAVY))
    {
        c = cols[BlueNavy];
    }
    else if ((col & BLUE_COL))
    {
        c = cols[Blue];
    }
    else if ((col & NAVY_COL))
    {
        c = cols[Navy];
    }
    else if ((col & RED_COL))
    {
        c = cols[Pink];
    }
    else if ((col & YELLOW_COL))
    {
        c = cols[Yellow];
    }
    else {
        c = cols[Black];
    }
    return c;
}

// Tests if the given parameters are legal for the given color.
bool ColorCreator::testValue(float h, float s, float z, int y, int u, int v, int color)
{
    if (!tableMode || !haveFile) {
        if (fltSliders[hMax][color] > fltSliders[hMin][color])
        {
            if (fltSliders[hMin][color] > h || fltSliders[hMax][color] < h)
            {
                return false;
            }
        } else if (fltSliders[hMin][color] > h && fltSliders[hMax][color] < h )
        {
            return false;
        }
        if (s < fltSliders[sMin][color] || s > fltSliders[sMax][color])
        {
            return false;
        }
        else if (z < fltSliders[zMin][color] || z > fltSliders[zMax][color])
        {
            return false;
        }
        else if (y < intSliders[yMin][color] || y > intSliders[yMax][color])
        {
            return false;
        } else if (v < intSliders[vMin][color] || v > intSliders[vMax][color])
        {
            return false;
        }
        return true;
    } else {
        unsigned col = table->index(y, u, v);
        return col & bitColor[color];
    }
}

/* Controller for the big display.  Normally just displays the current image
   in its raw form. */
void ColorCreator::largeDisplay()
{
    bool display;
    QColor c;
    bool regionSet = firstPoint.x() > -1;
    for (int i = 0; i < WIDTH; i++)
    {
        for (int j = 0; j < HEIGHT; j++)
        {
            bool looping = true;
            int start = Orange;
            if (mode == Single) {
                looping = false;
                start = currentColor;
            }
            c.setRgb(255,255,255);
            do {
                display = true;
                int y = yuvImage.getY(i, j);
                int u = yuvImage.getU(i, j);
                int v = yuvImage.getV(i, j);
                float s = (float)yuvImage.getS(i, j) / 256.0f;
                float h = (float)yuvImage.getH(i, j) / 256.0f;
                float z = (float)yuvImage.getZ(i, j) / 256.0f;
                // Since H is an angle the math is modulo.
                display = testValue(h, s, z, y, u, v, start);
                c = cols[start];
                if (display)
                {
                    looping = false;
                    // check for some common overlaps
                    if (start == Orange)
                    {
                        if (testValue(h, s, z, y, u, v, Pink))
                        {
                            c = cols[OrangeRed];
                        }
                    } else if (start == Green)
                    {
                        if (testValue(h, s, z, y, u, v, Blue))
                        {
                            c = cols[BlueGreen];
                        }
                    }
                    else if (start == Blue)
                    {
                        if (testValue(h, s, z, y, u, v, Navy))
                        {
                            c = cols[BlueNavy];
                        }
                    }
                } else{
                    c = cols[Black];
                }
                img->setPixel(i, j, c.rgb());
                start++;
                if (start == Black)
                {
                    looping = false;
                }
            } while (looping);
        }
    }
    if (regionSet && tableMode) {
        c = cols[Black];
        for (int k = firstPoint.x(); k < lastPoint.x(); k++)
        {
            img->setPixel(k, firstPoint.y(), c.rgb());
            img->setPixel(k, firstPoint.y()+1, c.rgb());
            img->setPixel(k, lastPoint.y(), c.rgb());
            img->setPixel(k, lastPoint.y()+1, c.rgb());
        }
        for (int l = firstPoint.y(); l < lastPoint.y(); l++)
        {
            img->setPixel(firstPoint.x(), l, c.rgb());
            img->setPixel(firstPoint.x()+1, l, c.rgb());
            img->setPixel(lastPoint.x(), l, c.rgb());
            img->setPixel(lastPoint.x()+1, l, c.rgb());
        }
    }
    QPixmap pix;
    pix.convertFromImage(*img);
    ui->thresh->setPixmap(pix);
    ui->thresh->repaint();

}

/* This is called when something happens that requires updating the
  thresholded image (e.g. a color definition changed).  It makes sure
  that everything gets update accordingly.
  */
void ColorCreator::updateThresh(bool imageChanged, bool choiceChanged, bool colorsChanged)
{
    if (haveFile)
    {
        bool display;
        bool stats = false;
        QColor c, c2;
        int red, blue, green;
        initStats();
        largeDisplay();
        for (int i = 0; i < WIDTH; i+=2)
        {
            for (int j = 0; j < HEIGHT; j+=2)
            {
                bool looping = true;
                int start = Orange;
                if (mode == Single) {
                    looping = false;
                    stats = true;
                    start = currentColor;
                }
                if (imageChanged) {
                    red = yuvImage.getRed(i, j);
                    green = yuvImage.getGreen(i, j);
                    blue = yuvImage.getBlue(i, j);
                    c.setRgb(red, green, blue);
                    img2->setPixel(i/2, j/2, c.rgb());
                }
                if (imageChanged || choiceChanged) {
                    c = getChannelView(i, j);
                    img3->setPixel(i/2, j/2, c.rgb());
                    c.setRgb(0, 0, 0);
                    c.setRgb(255,255,255);
                }
            }
        }
        if (imageChanged) {
            QPixmap pix2;
            pix2.convertFromImage(*img2);
            ui->view2->setPixmap(pix2);
            ui->view2->repaint();
        }
        if (imageChanged || choiceChanged) {
            QPixmap pix3;
            pix3.convertFromImage(*img3);
            ui->view3->setPixmap(pix3);
            ui->view3->repaint();
        }
        if (stats)
        {
            outputStats();
        }
    }
}

/* We can keep stats on the colors.  Init them for use.
  */
void ColorCreator::initStats()
{
    statsSMin = 1.0f;
    statsSMax = 0.0f;
    statsHMin = 1.0f;
    statsHMax = 0.0f;
    statsZMin = 1.0f;
    statsZMax = 0.0f;
    statsYMin = 255;
    statsYMax = 0;
    statsUMin = 255;
    statsUMax = 0;
    statsVMin = 255;
    statsVMax = 0;
}

/* Collect the stats.  THis is called in a loop.  Each pixel
  is added to the stats.
  Note: check if needed or if this is slowing things down.
  @param x    x value of pixel
  @param y    y value of pixel
  */
void ColorCreator::collectStats(int x, int y)
{
    float s = (float)yuvImage.getS(x, y) / 256.0f;
    float h = (float)yuvImage.getH(x, y) / 256.0f;
    float z = (float)yuvImage.getZ(x, y) / 256.0f;
    int yy = yuvImage.getY(x, y);
    int u = yuvImage.getU(x, y);
    int v = yuvImage.getV(x, y);
    statsSMin = min(statsSMin, s);
    statsSMax = max(statsSMax, s);
    statsHMin = min(statsHMin, h);
    statsHMax = max(statsHMax, h);
    statsZMin = min(statsZMin, z);
    statsZMax = max(statsZMax, z);
    statsYMin = min(statsYMin, yy);
    statsYMax = max(statsYMax, yy);
    statsUMin = min(statsUMin, u);
    statsUMax = max(statsUMax, u);
    statsVMin = min(statsVMin, v);
    statsVMax = max(statsVMax, v);
}

void ColorCreator::outputStats()
{
    QTextStream out(stdout);
    out << "Stats for current color\n";
    out << "H: " << statsHMin << " " << statsHMax << "\n";
    out << "S: " << statsSMin << " " << statsSMax << "\n";
    out << "Z: " << statsZMin << " " << statsZMax << "\n";
    out << "Y: " << statsYMin << " " << statsYMax << "\n";
    out << "U: " << statsUMin << " " << statsUMax << "\n";
    out << "V: " << statsVMin << " " << statsVMax << "\n";
}

void ColorCreator::modeChanged()
{
    if (tableMode && firstPoint.x() > -1)
    {
        int y, u, v, yHigh, uHigh, vHigh, yLow, uLow, vLow;
        if (defineMode)
        {
            // collect up all of the pixels in the region that are not the right color
            y = u = v = 255;
            yLow = uLow = vLow = 255;
            yHigh = uHigh = vHigh = 0;
            for (int k = firstPoint.x(); k < lastPoint.x(); k++)
            {
                for (int l = firstPoint.y(); l < lastPoint.y(); l++)
                {
                    y = yuvImage.getY(k, l);
                    u = yuvImage.getU(k, l);
                    v = yuvImage.getV(k, l);
                    if (y < yLow)
                    {
                        yLow = y;
                    }
                    if (u < uLow)
                    {
                        uLow = u;
                    }
                    if (v < vLow)
                    {
                        vLow = v;
                    }
                    if (y > yHigh)
                    {
                        yHigh = y;
                    }
                    if (u > uHigh)
                    {
                        uHigh = u;
                    }
                    if (v > vHigh)
                    {
                        vHigh = v;
                    }
                }
            }
            QTextStream out(stdout);
            out << "Set Y Stuff value to " << yLow << " " << yHigh << "\n";
            // we have defined a region in the space to make that color
            for (y = yLow; y <= yHigh; y++)
            {
                for (u = uLow; u <= uHigh; u++)
                {
                    for (v = vLow; v <= vHigh; v++)
                    {
                        table->setColor(y, u, v, bitColor[currentColor]);
                    }
                }
            }
        }
        else
        {
            for (int k = firstPoint.x(); k < lastPoint.x(); k++)
            {
                for (int l = firstPoint.y(); l < lastPoint.y(); l++)
                {
                    y = yuvImage.getY(k, l);
                    u = yuvImage.getU(k, l);
                    v = yuvImage.getV(k, l);
                    table->unSetColor(y, u, v, bitColor[currentColor]);
                }
            }
        }
    }
    firstPoint.setX(-1);
    largeDisplay();
}

void ColorCreator::on_pushButton_clicked()
{
    currentDirectory = QFileDialog::getOpenFileName(this, tr("Open Image"),
                                            currentDirectory,
                                            tr("Image Files (*.log)"));
    yuvImage.updateFromRoboImage();
    updateDisplays();
}

void ColorCreator::on_previousButton_clicked()
{
    //TODO: parsing doesn't work backwards atm
    //yuvImage.read(previousFrame);
    currentFrameNumber--;
    updateDisplays();
}

void ColorCreator::on_nextButton_clicked()
{
    dataManager->getNext();
    currentFrameNumber++;
}

void ColorCreator::on_plusTen_clicked()
{
    //yuvImage.read(tenthFrame);
    currentFrameNumber += 10;
    updateDisplays();
}

void ColorCreator::on_minusTen_clicked()
{
    //yuvImage.read(minusTenthFrame);
    currentFrameNumber -= 10;
    updateDisplays();
}

void ColorCreator::on_hMin_valueChanged(int value)
{
    fltSliders[hMin][currentColor] = (float)value / 100.0f;
    updateColors();
    QTextStream out(stdout);
    out << "Set H Min value to " << value << "\n";
}

void ColorCreator::on_hMax_valueChanged(int value)
{
    fltSliders[hMax][currentColor] = (float)value / 100.0f;
    updateColors();
    QTextStream out(stdout);
    out << "Set H Max value to " << value << "\n";
}

void ColorCreator::on_sMin_valueChanged(int value)
{
    fltSliders[sMin][currentColor] = (float)value / 100.0f;
    updateColors();
    QTextStream out(stdout);
    out << "Set S Min value to " << value << "\n";
}

void ColorCreator::on_sMax_valueChanged(int value)
{
    fltSliders[sMax][currentColor] = (float)value / 100.0f;
    updateColors();
    QTextStream out(stdout);
    out << "Set S Max value to " << value << "\n";
}

void ColorCreator::on_zMin_valueChanged(int value)
{
    fltSliders[zMin][currentColor] = (float)value / 100.0f;
    updateColors();
    QTextStream out(stdout);
    out << "Set Z Min value to " << value << "\n";
}

void ColorCreator::on_zMax_valueChanged(int value)
{
    fltSliders[zMax][currentColor] = (float)value / 100.0f;
    updateColors();
    QTextStream out(stdout);
    out << "Set Z Max value to " << value << "\n";
}

void ColorCreator::on_yMin_valueChanged(int value)
{
    intSliders[yMin][currentColor] = value;
    updateColors();
    QTextStream out(stdout);
    out << "Set Y Min value to " << value << "\n";
}

void ColorCreator::on_yMax_valueChanged(int value)
{
    intSliders[yMax][currentColor] = value;
    updateColors();
    QTextStream out(stdout);
    out << "Set Y Max value to " << value << "\n";
}

void ColorCreator::on_vMin_valueChanged(int value)
{
    intSliders[vMin][currentColor] = value;
    updateColors();
    QTextStream out(stdout);
    out << "Set V Min value to " << value << "\n";
}

void ColorCreator::on_vMax_valueChanged(int value)
{
    intSliders[vMax][currentColor] = value;
    updateColors();
    QTextStream out(stdout);
    out << "Set V Max value to " << value << "\n";
}

void ColorCreator::on_zSlice_valueChanged(int value)
{
    zSlice = (float)value / 100.0f;
    updateColors();
}


void ColorCreator::on_readTable_clicked()
{
    currentColorDirectory =
      QFileDialog::getOpenFileName(this, tr("Open Color Table"),
                                            currentColorDirectory,
                                            tr("Table Files (*.mtb)"));
    table->read(currentColorDirectory);
    int last = currentColorDirectory.lastIndexOf("/");
    currentColorDirectory.chop(currentColorDirectory.size() - last);

    if (ui->channel->currentIndex() == Table)
      updateThresh(false, true, false);
}

/* The user wants to write a color table.
  */
void ColorCreator::on_writeTable_clicked()
{
    QString filename = baseColorTable + "/new.mtb";
    //writeOldFormat(filename);
    table->write(filename, fltSliders, intSliders, bitColor);
}

/* Loads and old style color table.  Note: it will be automatically
  converted to the new format (which is the main reason to do this).
  */
void ColorCreator::on_writeOldTable_clicked()
{
    currentColorDirectory = QFileDialog::getOpenFileName(this, tr("Open Old Color Table"),
                                            currentColorDirectory,
                                            tr("Table Files (*.mtb)"));
    table->readOld(currentColorDirectory);
    int last = currentColorDirectory.lastIndexOf("/");
    currentColorDirectory.chop(currentColorDirectory.size() - last);
}

void ColorCreator::on_channel_currentIndexChanged(int index)
{
    shape = index;
    updateThresh(false, true, false);
}

/* User changes the edge threshold.  When displaying edge
  images we use an int to determine what constitutes an edge.
  @param value      the new value to use
  */
void ColorCreator::on_edgeDiff_valueChanged(int value)
{
    QTextStream out(stdout);
    out << "Set threshold to " << value << "\n";
    edgediff = value;
    updateThresh(false, true, false);
}

/* User wants a new edge thing.  This was inadvertently added.
  */
void ColorCreator::on_edgeDiff_actionTriggered(int action)
{

}

void ColorCreator::on_modeSelect_currentIndexChanged(int index)
{
  defineMode = !index;
  tableMode = index;
  modeChanged();
}

void ColorCreator::on_changeColor_clicked()
{
  modeChanged();
}

/* Called when the user picks a new color to work on.
  */
void ColorCreator::on_colorSelect_currentIndexChanged(int index)
{
    currentColor = index;
    ui->hMin->setValue(fltSliders[hMin][currentColor] * 100);
    ui->hMax->setValue(fltSliders[hMax][currentColor] * 100);
    ui->sMin->setValue(fltSliders[sMin][currentColor] * 100);
    ui->sMax->setValue(fltSliders[sMax][currentColor] * 100);
    ui->zMin->setValue(fltSliders[zMin][currentColor] * 100);
    ui->zMax->setValue(fltSliders[zMax][currentColor] * 100);
    ui->yMin->setValue(intSliders[yMin][currentColor]);
    ui->yMax->setValue(intSliders[yMax][currentColor]);
    ui->vMin->setValue(intSliders[vMin][currentColor]);
    ui->vMax->setValue(intSliders[vMax][currentColor]);
    ui->zSlice->setValue((fltSliders[zMin][currentColor] + fltSliders[zMax][currentColor]) * 50);
}

/* Called when the user wants to view something different.
  @param value    the new value selected
  */
void ColorCreator::on_viewChoice_currentIndexChanged(int index)
{
    mode = index;
    updateThresh(false, true, false);
}

void ColorCreator::on_cornerDefine_clicked()
{
    cornerStatus = !cornerStatus;
}

}
}
