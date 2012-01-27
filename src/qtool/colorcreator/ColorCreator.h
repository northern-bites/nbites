#ifndef COLORCREATOR_H
#define COLORCREATOR_H

#include <qfile.h>
#include <qtextstream.h>

#include <QMouseEvent>
#include <QWidget>

//qtool
#include "image/BMPYUVImage.h"
#include "data/DataManager.h"
//colorcreator
#include "ColorEdit.h"
#include "ColorTable.h"
#include "ColorSpace.h"

#define  NEWFRAMES
#ifdef   NEWFRAMES
#define  COLORS 8
#define  SOFT 3
#define  WIDTH 640
#define  HEIGHT 480
#define  EXTENSION ".frm"
#define  EXTENSIONLENGTH = 4
#else
#define  COLORS 8
#define  WIDTH 320
#define  HEIGHT 240
#define  EXTENSION ".NBFRM"
#define  EXTENSIONLENGTH = 6
#endif
#define GREY_COL 0x00
#define WHITE_COL 0x01
#define GREEN_COL 0x02
#define BLUE_COL 0x04
#define YELLOW_COL 0x08
#define ORANGE_COL 0x10
#define YELLOWWHITE_COL 0x09
#define BLUEGREEN_COL 0x06
#define ORANGERED_COL 0x30
#define ORANGEYELLOW_COL 0x18
#define RED_COL 0x20
#define NAVY_COL 0x40

#define FLT_SLIDERS 6
#define INT_SLIDERS 4

namespace Ui {
    class ColorCreator;
}

namespace qtool {
namespace colorcreator {

class ColorCreator : public QMainWindow
{
    Q_OBJECT

public:
    enum Colors {Orange, Blue, Yellow, Green, White, Pink, Navy, Black, BlueGreen, BlueNavy, OrangeRed};
    enum Choices {Single, Multiple};
    enum Shape { Y, U, V, Bluec, Redc, Greenc, H, S, Z, EDGE, Table};
    static const QColor RGBcolorValue[];

    ColorCreator(qtool::data::DataManager::ptr dataManager,
            QWidget *parent = 0);
    ~ColorCreator();
    void updateDisplays();
    void updateColors();
    void updateThresh(bool imageChanged, bool choiceChanged, bool colorsChanged);

    void initStats();
    void collectStats(int x, int y);
    void outputStats();
    float min(float a, float b) {if (a < b) return a; return b;}
    float max(float a, float b) {if (a > b) return a; return b;}
    int min(int a, int b) {if (a < b) return a; return b;}
    int max(int a, int b) {if (a > b) return a; return b;}
    void writeNewFormat(QString filename);
    void writeOldFormat(QString filename);
    QColor getChannelView(int i, int j);
    void largeDisplay();
    QColor displayColorTable(int i, int j);
    bool testValue(float h, float s, float z, int y, int u, int v, int color);
    void modeChanged();
    short setInitialColorValuesFromFile(QString filename);
    void writeInitialColorValues(QString filename);

public slots:
    void updatedImage() {
        this->QWidget::update();
    }

protected:
    void mouseMoveEvent(QMouseEvent* event);
    void mouseReleaseEvent(QMouseEvent* event);
    void mousePressEvent(QMouseEvent* event);
    void paintEvent(QPaintEvent *event);

private slots:

    //sliders
    void on_hMin_valueChanged(int value);
    void on_hMax_valueChanged(int value);
    void on_sMin_valueChanged(int value);
    void on_sMax_valueChanged(int value);
    void on_zMin_valueChanged(int value);
    void on_zMax_valueChanged(int value);
    void on_yMin_valueChanged(int value);
    void on_yMax_valueChanged(int value);
    void on_vMin_valueChanged(int value);
    void on_vMax_valueChanged(int value);
    void on_zSlice_valueChanged(int value);

    //load/save slider values
    void on_readSliders_clicked();
    void on_writeSliders_clicked();

    //load/save color tables
    void on_readTable_clicked();
    void on_writeTable_clicked();
    void on_writeOldTable_clicked();

    //threshold image
    void on_channel_currentIndexChanged(int index);
    void on_edgeDiff_valueChanged(int value);
    void on_edgeDiff_actionTriggered(int action);

    //other
    void on_modeSelect_currentIndexChanged(int index);
    void on_changeColor_clicked();
    void on_colorSelect_currentIndexChanged(int index);
    void on_viewChoice_currentIndexChanged(int index);

    void on_cornerDefine_clicked();

private:
    Ui::ColorCreator *ui;
    YUVImage yuvImage;
    qtool::data::DataManager::ptr dataManager;
    ColorTable *table;
    QString baseDirectory;
    QString baseFrameDirectory;
    QString baseSliderDirectory;
    QString currentDirectory;
    QString baseColorTable;
    QString currentColorDirectory;
    QString nextFrame;
    QString previousFrame;
    QString tenthFrame;
    QString minusTenthFrame;
    QImage *img;
    QImage *img2;
    QImage *img3;
    QImage *img4;
    QImage *wheel;
    bool tableMode;
    bool defineMode;
    bool cornerStatus;
    QPoint firstPoint;
    QPoint lastPoint;
    int shape;
    int currentFrameNumber;
    int edgediff;
    int mode;
    QColor *cols;
    ColorSpace colorSpace[COLORS];
    ColorSpace* currentColorSpace;
    float **fltSliders;
    int   **intSliders;
    enum fltChannels {hMin, hMax,
                      sMin, sMax,
                      zMin, zMax};
    enum intChannels {yMin, yMax,
                      vMin, vMax};
    float zSlice;
    float statsHMin, statsHMax, statsSMin, statsSMax, statsZMin, statsZMax;
    int statsYMin, statsYMax, statsUMin, statsUMax, statsVMin, statsVMax;
    int currentColor;
    ColorEdit *green;
    unsigned *bitColor;
    bool haveFile;
    bool viewerEnabled;
};

}
}

#endif // COLORCREATOR_H
