#ifndef COLORTABLE_H
#define COLORTABLE_H
#include <QString>
#include <QFile>
#include <QTextStream>

#include "Stats.h"
#include "image/Color.h"

#include "VisionDef.h"

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
