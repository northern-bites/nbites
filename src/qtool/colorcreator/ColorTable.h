#ifndef COLORTABLE_H
#define COLORTABLE_H
#include <QString>
#include <QFile>
#include <QTextStream>

#include "Stats.h"
#include "image/ColorSpace.h"

#define UNDEFINED 0x00
#define WHITE     0x01
#define GREEN     0x02
#define BLUE      0x04
#define YELLOW    0x08
#define ORANGE    0x10
#define PINK      0x20
#define NAVY      0x40

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

namespace qtool {
namespace colorcreator {

class ColorTable
{
public:
    enum Colors {Orange, Blue, Yellow, Green, White, Pink, Navy, Black};
    ColorTable();
    unsigned*** getTable() {return table; }
    void read(QString filename);
    void readOld(QString filename);
    void writeNewFormat(QString filename);
    int tableIndex(int high7, int mid7, int low7);
    unsigned index(int y, int u, int v);
    void setColor(int y, int u, int v, unsigned col);
    void unSetColor(int y, int u, int v, unsigned col);
    Stats** colorStats();
    bool isEnabled() {return enabled;}
    int getUpdatedColor(int y, int u, int v);

    //color table output methods
    void write(QString filename, float** fltSliders,
               int** intSliders, unsigned* bitColor);
    void writeOld(QString filename, float** fltSliders,
                  int** intSliders);



private:
    enum fltChannels {hMin, hMax,
                      sMin, sMax,
                      zMin, zMax};
    enum intChannels {yMin, yMax,
                      vMin, vMax};
    static const int mainColors = 5;
    int** colormap;
    unsigned*** table;
    bool enabled;
};

}
}

#endif // COLORTABLE_H
