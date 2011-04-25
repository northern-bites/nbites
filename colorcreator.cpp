#include "colorcreator.h"
#include "ui_colorcreator.h"
#include "math.h"

#include <QTextStream>
#include <QFileDialog>
#include <QString>
#include <QStringRef>
#include <QRgb>
#include <QImage>
#include <QPixmap>

ColorCreator::ColorCreator(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ColorCreator),
    roboimage(WIDTH, HEIGHT),
    imageWindow(roboimage)

{
    img = new QImage(WIDTH, HEIGHT, QImage::Format_RGB32);
    wheel = new QImage(200, 200, QImage::Format_RGB32);

    hMin = new float[COLORS];
    hMax = new float[COLORS];
    sMin = new float[COLORS];
    sMax = new float[COLORS];
    zMin = new float[COLORS];
    zMax = new float[COLORS];
    yMin = new int[COLORS];
    yMax = new int[COLORS];
    cols = new QColor[COLORS];
    bitColor = new unsigned[COLORS];

    table = new ColorTable();

    // initialize colors for drawing thresholds
    cols[Orange] = QColor(255, 128, 0);
    cols[Blue] = QColor(0, 0, 255);
    cols[Green] = QColor(0, 201, 87);
    cols[Yellow] = QColor(255, 255, 0);
    cols[White] = QColor(255, 255, 255);
    cols[Pink] = QColor(255, 181, 197);
    cols[Navy] = QColor(0, 0, 128);
    cols[Black] = QColor(0, 0, 0);

    // initialize bitColors for generating color tables
    bitColor[Orange] = ORANGE;
    bitColor[Blue] = BLUE;
    bitColor[Yellow] = YELLOW;
    bitColor[Green] = GREEN;
    bitColor[White] = WHITE;
    bitColor[Pink] = PINK;
    bitColor[Navy] = NAVY;
    bitColor[Black] = UNDEFINED;

    ui->setupUi(this);
    baseDirectory = "/Users/ericchown/nbites/data/frames";
    baseColorTable = "/Users/ericchown/nbites/data/tables";
    haveFile = false;
    viewerEnabled = false;

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

    // initialize all of our values.  Ideally these will serve as a pretty good table
    // for virtually any environment
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
            hMin[i] = 0.42f;
            hMax[i] = 0.58f;
            sMin[i] = 0.35f;
            sMax[i] = 0.62f;
            zMin[i] = 0.22f;
            zMax[i] = 0.73f;
            yMin[i] = 47;
            yMax[i] = 112;
            break;
        case Yellow:
            hMin[i] = 0.10f;
            hMax[i] = 0.30f;
            sMin[i] = 0.11f;
            sMax[i] = 0.74f;
            zMin[i] = 0.26f;
            zMax[i] = 0.70f;
            yMin[i] = 54;
            yMax[i] = 170;
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
            hMin[i] = 0.01f;
            hMax[i] = 1.0f;
            sMin[i] = 0.0f;
            sMax[i] = 0.3f;
            zMin[i] = 0.55f;
            zMax[i] = 1.0f;
            yMin[i] = 150;
            yMax[i] = 250;
            break;
        case Pink:
            hMin[i] = 0.73f;
            hMax[i] = 0.98f;
            sMin[i] = 0.17f;
            sMax[i] = 0.4f;
            zMin[i] = 0.14f;
            zMax[i] = 0.31f;
            yMin[i] = 38;
            yMax[i] = 115;
            break;
        case Navy:
            hMin[i] = 0.64f;
            hMax[i] = 0.68f;
            sMin[i] = 0.38f;
            sMax[i] = 0.62f;
            zMin[i] = 0.29f;
            zMax[i] = 0.36f;
            yMin[i] = 35;
            yMax[i] = 105;
            break;
        default:
            hMin[i] = 0.0f;
            hMax[i] = 0.01f;
            sMin[i] = 0.99f;
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
    if (viewerEnabled)
    {
        imageWindow.repaint();
    }
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
    updateThresh();
}

void ColorCreator::updateColors()
{
    // we draw by using a QImage - turn it into a Pixmap, then put it on a label
    //QImage img(200, 200, QImage::Format_RGB32);
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
                ColorSpace col;
                col.setHsz(h, s, zSlice);
                int y = col.getYb();
                if (y < yMin[currentColor] || y > yMax[currentColor])
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
    updateThresh();
}

void ColorCreator::updateThresh()
{
    if (haveFile)
    {
        bool display;
        bool stats = false;
        QColor c;
        initStats();
        for (int i = 0; i < WIDTH; i++)
        {
            for (int j = 0; j < HEIGHT; j++)
            {
                bool looping = true;
                int start = Orange;
                if (mode == Single) {
                    looping = false;
                    stats = true;
                    start = currentColor;
                }
                do {
                    display = true;

                    int y = roboimage.getY(i, j);
                    if (mode == Table && table->isEnabled())
                    {
                        int u = roboimage.getU(i, j);
                        int v = roboimage.getV(i, j);
                        int col = table->getUpdatedColor(y, u, v);
                        if (col >= Black) {
                            display = false;
                        } else{
                            c = cols[col];
                        }
                    } else{
                        float s = (float)roboimage.getS(i, j) / 256.0f;
                        float h = (float)roboimage.getH(i, j) / 256.0f;
                        float z = (float)roboimage.getZ(i, j) / 256.0f;
                        // Since H is an angle the math is modulo.
                        if (hMax[start] > hMin[start])
                        {
                            if (hMin[start] > h || hMax[start] < h)
                            {
                                display = false;
                            }
                        } else if (hMin[start] > h && hMax[start] < h )
                        {
                            display = false;
                        }
                        if (s < sMin[start] || s > sMax[start])
                        {
                            display = false;
                        }
                        else if (z < zMin[start] || z > zMax[start])
                        {
                            display = false;
                        }
                        else if (y < yMin[start] || y > yMax[start])
                        {
                            display = false;
                        }
                        c = cols[start];
                    }
                    if (display)
                    {
                        looping = false;
                        if (stats)
                        {
                            collectStats(i, j);
                        }
                    } else{
                        c.setRgb(y, y, y);
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
        QPixmap pix;
        pix.convertFromImage(*img);
        ui->thresh->setPixmap(pix);
        ui->thresh->repaint();
        if (stats)
        {
            outputStats();
        }
    }
}

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

void ColorCreator::collectStats(int x, int y)
{
    float s = (float)roboimage.getS(x, y) / 256.0f;
    float h = (float)roboimage.getH(x, y) / 256.0f;
    float z = (float)roboimage.getZ(x, y) / 256.0f;
    int yy = roboimage.getY(x, y);
    int u = roboimage.getU(x, y);
    int v = roboimage.getV(x, y);
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

void ColorCreator::on_pushButton_clicked()
{
    currentDirectory = QFileDialog::getOpenFileName(this, tr("Open Image"),
                                            currentDirectory,
                                            tr("Image Files (*.frm)"));
    roboimage.read(currentDirectory);
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
    roboimage.read(previousFrame);
    currentFrameNumber--;
    updateDisplays();
}

void ColorCreator::on_nextButton_clicked()
{
    roboimage.read(nextFrame);
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
    ui->zSlice->setValue((zMin[currentColor] + zMax[currentColor]) * 50);
}

void ColorCreator::on_viewChoice_currentIndexChanged(int index)
{
    mode = index;
    updateThresh();
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


void ColorCreator::on_getColorTable_clicked()
{
    currentColorDirectory = QFileDialog::getOpenFileName(this, tr("Open Color Table"),
                                            currentColorDirectory,
                                            tr("Table Files (*.mtb)"));
    table->read(currentColorDirectory);
    int last = currentColorDirectory.lastIndexOf("/");
    currentColorDirectory.chop(currentColorDirectory.size() - last);
}

// NOte: this won't work.  Its just a rough template
void ColorCreator::writeNewFormat(QString filename)
{
    QFile file(filename);
    QTextStream out(stdout);
    QByteArray temp;
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        out << "The file would not open properly" << "\n";
        return;
    }
    for (int y = 0; y < 128; ++y)
    {
        for (int x = 0; x < 128; x ++)
        {
            for (int z = 0; z < 128; z++)
            {
                temp[0] = UNDEFINED;
                ColorSpace col;
                col.setYuv(y * 2, x * 2, z * 2);
                for (int c = 0; c < COLORS; c++)
                {
                    if (y * 2 >= yMin[c] && y * 2 <= yMax[c] && col.getHb() >= hMin[c] && col.getHb() <= hMax[c] &&
                            col.getSb() >= sMin[c] && col.getSb() <= sMax[c] && col.getZb() >= zMin[c] &&
                            col.getZb() <= zMax[c])
                    {
                        temp[0] = temp[0] | bitColor[c];
                    }
                }
                file.write(temp);
            }
        }
    }
    file.close();
}

void ColorCreator::writeOldFormat(QString filename)
{
    QFile file(filename);
    QTextStream out(stdout);
    QByteArray temp;
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        out << "The file would not open properly" << "\n";
        return;
    }
    // loop through all possible table values - our tables are v-u-y
    int count = 0;
    for (int z = 0; z < 128; ++z)
    {
        for (int x = 0; x < 128; x ++)
        {
            for (int y = 0; y < 128; y++)
            {
                temp[0] = GREY_COL;
                ColorSpace col;
                col.setYuv(y * 2, x * 2, z * 2);
                bool orange = false;
                bool yellow = false;
                bool blue = false;
                for (int c = Orange; c < Black; c++)
                {
                    bool ok = false;
                    if (hMin[c] > hMax[c])
                    {
                        if (col.getH() >= hMin[c] || col.getH() <= hMax[c])
                        {
                            ok = true;
                        }
                    } else
                    {
                        if (col.getH() >= hMin[c] && col.getH() <= hMax[c])
                        {
                            ok = true;
                        }
                    }
                    if (ok && y * 2 >= yMin[c] && y * 2 <= yMax[c] &&
                            col.getS() >= sMin[c] && col.getS() <= sMax[c] && col.getZ() >= zMin[c] &&
                            col.getZ() <= zMax[c])
                    {
                        switch (c)
                        {
                        case Orange:
                            temp[0] = ORANGE_COL;
                            orange = true;
                            count++;
                            break;
                        case Blue:
                            temp[0] = BLUE_COL;
                            blue = true;
                            break;
                        case Yellow:
                            if (orange)
                            {
                                temp[0] = ORANGEYELLOW_COL;
                            } else
                            {
                                temp[0] = YELLOW_COL;
                            }
                            yellow = true;
                            break;
                        case Green:
                            if (blue)
                            {
                                temp[0] = BLUEGREEN_COL;
                            } else{
                                temp[0] = GREEN_COL;
                            }
                            break;
                        case White:
                            if (yellow)
                            {
                                temp[0] = YELLOWWHITE_COL;
                            } else
                            {
                                temp[0] = WHITE;
                            }
                            break;
                        case Pink:
                            if (orange)
                            {
                                temp[0] = ORANGERED_COL;
                            } else
                            {
                                temp[0] = RED_COL;
                            }
                            break;
                        case Navy:
                            temp[0] = NAVY_COL;
                            break;
                        }
                    }
                }
                file.write(temp);
            }
        }
    }
    out << "Count was " << count << "\n";
    file.close();
}


void ColorCreator::on_writeNew_clicked()
{
    QString filename = baseColorTable + "/new.mtb";
    writeOldFormat(filename);
}

void ColorCreator::on_viewer_clicked()
{
    imageWindow.show();
    viewerEnabled = true;
}

void ColorCreator::on_plusTen_clicked()
{
    roboimage.read(tenthFrame);
    currentFrameNumber += 10;
    updateDisplays();
}

void ColorCreator::on_minusTen_clicked()
{
    roboimage.read(minusTenthFrame);
    currentFrameNumber -= 10;
    updateDisplays();
}
