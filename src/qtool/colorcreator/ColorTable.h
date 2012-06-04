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
    static const int Y_MAX = 256, U_MAX = 256, V_MAX = 256;
    static const int SCALE = 2;
    static const int Y_SIZE = Y_MAX/SCALE, U_SIZE = U_MAX/SCALE, V_SIZE = V_MAX/SCALE;
    static const int TABLE_SIZE = Y_SIZE * U_SIZE * V_SIZE;

public:
    ColorTable();
    ~ColorTable();

    byte* getTable() { return table; }
    //computes the offset of the color in the table for a y, u, v value
    int offset(byte y, byte u, byte v) { return (v/2)*Y_SIZE*U_SIZE + (u/2)*Y_SIZE + (y/2); }

    void read(std::string filename);

    byte getColor(byte y, byte u, byte v) { return table[offset(y, u, v)]; }
    void setColor(byte y, byte u, byte v, byte col) { table[offset(y, u, v)] |= col; }
    void unSetColor(byte y, byte u, byte v, byte col) {
        byte allCol = 0xFF;
        col = col ^ allCol;
        table[offset(y, u, v)] &= col;
    }

    //color table output methods
    void write(QString filename);

    //     NOTE: Left this in for Octavians stuff to work, he should verify
    static void writeFromSliders(QString filename, ColorSpace* colorSpaces);

private:
    byte* table;
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
