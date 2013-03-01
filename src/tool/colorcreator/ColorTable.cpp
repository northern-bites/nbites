/*  ColorTable class.  There are several things we need to do in this class.
    1. Read tables
    2. Write tables

    Our color table is currently a 128x128x128 table accessed by a pixel's
    yuv value (in the order
  */

#include "ColorTable.h"

#include <iostream>

namespace qtool {
namespace colorcreator {

using namespace image;
using namespace std;

ColorTable::ColorTable() {
    //initialize to all zeroes
    table = (byte*) calloc(sizeof(byte), TABLE_SIZE);
}

ColorTable::~ColorTable() {
    free(table);
}

void ColorTable::read(string filename) {

    FILE *tableFile = fopen(filename.c_str(), "r");   //open table for reading

    if (!tableFile) {
        cerr << "Could not open color table " << filename;
        return;
    }

    size_t bytesRead = fread(table, sizeof(byte), TABLE_SIZE, tableFile);
    if (bytesRead == 0) {
        cerr << "Error reading color table " << filename;
    }

    fclose(tableFile);
}

void ColorTable::write(string filename) {

    FILE *tableFile = fopen(filename.c_str(), "w");

    if (!tableFile) {
        cerr << "Could open file for saving a color table " << filename;
        return;
    }

    fwrite(table, sizeof(byte), TABLE_SIZE, tableFile);

    fclose(tableFile);
}

int ColorTable::countColor(byte color) {

    int count = 0;

    for (int i = 0; i < TABLE_SIZE; i++) {
        if ((table[i] & color) > 0) {
            count++;
        }
    }

    return count;
}

/* Write out a color table using bitwise definitions
 * using information from a set of NUM_COLORS colorSpace
 */
void ColorTable::writeFromSliders(QString filename, ColorSpace* colorSpaces) {

    QFile file(filename);
    QTextStream out(stdout);
    byte V_MAX = 128, U_MAX = 128, Y_MAX = 128;
    QByteArray table;

    if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        out << "Could not open file to write color table properly!" << "\n";
        return;
    }
    // loop through all possible table values - our tables are v-u-y
    int count = 0;
    for (int z = 0; z < V_MAX; z++)
    {
        for (int x = 0; x < U_MAX; x++)
        {
            for (int y = 0; y < Y_MAX; y++)
            {
                byte temp = 0;
                Color color;
                assert(2*y >=y && 2*x >= x && 2*z >= z); //overflow
                color.setYuv((byte) (2*y), (byte) (2*x), (byte) (2*z));
                for (int c = 0; c < image::NUM_COLORS; c++)
                {
                    if (colorSpaces[c].contains(color)) {
                        temp = temp | image::Color_bits[c];
                    }
                }
                table.append(temp);
            }
        }
    }
    file.write(table);
    file.close();
}

}
}
