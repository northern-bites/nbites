#ifndef COLORTABLE_H
#define COLORTABLE_H
/**
 * TODO: this needs cleanup
 *
 * -Octavian
 */
#include <QString>
#include <QFile>
#include <QTextStream>

#include "Stats.h"
#include "image/Color.h"
#include "ColorSpace.h"

#include "VisionDef.h"

namespace qtool {
namespace colorcreator {

class ColorTable
{
public:
    ColorTable();
    byte*** getTable() {return table; }
    uint8_t* getLinearTable();
    void read(QString filename);
    byte getColor(byte y, byte u, byte v);
    void setColor(byte y, byte u, byte v, byte col);
    void unSetColor(byte y, byte u, byte v, byte col);

    //color table output methods
    void write(QString filename);

    //     NOTE: Left this in for Octavians stuff to work, he should verify
    static void writeFromSliders(QString filename, ColorSpace* colorSpaces);

private:
    byte*** table;
};

}
}

#endif // COLORTABLE_H

// public:
//     enum Colors {Orange, Blue, Yellow, Green, White, Pink, Navy, Black};
//     ColorTable();
//     unsigned*** getTable() {return table; }
//     void read(QString filename);
//     void readOld(QString filename);
//     void writeNewFormat(QString filename);
//     int tableIndex(int high7, int mid7, int low7);
//     unsigned index(int y, int u, int v);
//     void setColor(int y, int u, int v, unsigned col);
//     void unSetColor(int y, int u, int v, unsigned col);
//     Stats** colorStats();
//     bool isEnabled() {return enabled;}
//     int getUpdatedColor(int y, int u, int v);

//     //color table output methods
//     static void write(QString filename, ColorSpace* colorSpaces);

// private:
//     enum fltChannels {hMin, hMax,
//                       sMin, sMax,
//                       zMin, zMax};
//     enum intChannels {yMin, yMax,
//                       vMin, vMax};
//     static const int mainColors = 5;
//     int** colormap;
//     unsigned*** table;
//     bool enabled;
