#pragma once

#include <QString>
#include <QFile>
#include <QTextStream>

#include <iostream>

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
    int offset(byte y, byte u, byte v) {
        return ((int) v/2)*Y_SIZE*U_SIZE + ((int) u/2)*Y_SIZE + ((int) y/2); }

    byte getColor(byte y, byte u, byte v) { return table[offset(y, u, v)]; }
    void setColor(byte y, byte u, byte v, byte col) { table[offset(y, u, v)] |= col; }
    void unSetColor(byte y, byte u, byte v, byte col) {
        byte allCol = 0xFF;
        col = col ^ allCol;
        table[offset(y, u, v)] &= col;
    }

    void read(std::string filename);
    void write(std::string filename);

    int countColor(byte color);

    //     NOTE: Left this in for Octavians stuff to work, he should verify
    static void writeFromSliders(QString filename, ColorSpace* colorSpaces);

private:
    byte* table;
};

}
}

