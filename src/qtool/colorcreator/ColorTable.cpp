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

// Read table from a file and determine the format
void ColorTable::read(string filename)
{

    FILE *tableFile = fopen(filename.c_str(), "r");   //open table for reading

    if (tableFile == NULL) {
        cerr << "Could not open color table " << filename;
        return;
    }

    // Color table is in VUY ordering
    int rval;
    for(int v=0; v < V_SIZE; ++v) {
        for(int u=0; u < U_SIZE; ++u) {
            fread(&table[v * U_SIZE * Y_SIZE + u * Y_SIZE],
                    sizeof(unsigned char), Y_SIZE, tableFile);
        }
    }

    fclose(tableFile);
}

/* WHen we read in a table of the old format we automatically convert
  it to the new format.  Here we just write out the table directly.
  */
void ColorTable::write(QString filename) {

    QFile file(filename);
    QTextStream out(stdout);
    QByteArray temp;
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        out << "The file would not open properly" << "\n";
        return;
    }
    for (byte y = 0; y < 128; ++y)
    {
        for (byte x = 0; x < 128; x ++)
        {
            for (byte z = 0; z < 128; z++)
            {
                temp[0] = table[offset(y, x, z)];
                file.write(temp);
            }
        }
    }
    file.close();
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
                        if (c == image::Orange) {
                            count++;
                        }
                        temp = temp | image::Color_bits[c];
                    }
                }
                table.append(temp);
            }
        }
    }
    file.write(table);
    out << "Orange count was " << count << "\n" << endl;
    file.close();
}

/*Stats** ColorTable::colorStats()
{
    Stats** colorStats;
    colorStats = new Stats*[3];
    for (byte i = 0; i < 3; i++)
    {
        colorStats[i] = new Stats[mainColors];
    }

    // initialize stats
    for (byte h = 0; h < 3; ++h)
        for (byte c = 0; c < mainColors; ++c)
            colorStats[h][c] = Stats();

    // loop through the whole table collecting stats
    for (byte y = 0; y < 256; y += 2)
        for (byte v = 0; v < 256; v += 2)
            for (byte u = 0; u < 256; u += 2)
            {
                byte c = index(y, u, v);
                while (c < 0) {
                    c = 256 + c;
                }
                if (c > 256)
                {
                    c = c % 256;
                }
                // assumes old style table - convert values
                // e.g. ORANGERED hits both ORANGE and RED
                if (c < 12)
                    for (byte q = 0; q < 2; ++q)
                    {
                        byte ci = colormap[q][c];
                        if (ci < mainColors)
                        {
                            colorStats[0][ci].add(y);
                            colorStats[1][ci].add(u);
                            colorStats[2][ci].add(v);
                        }
                    }
            }

    return colorStats;
}*/

}
}
