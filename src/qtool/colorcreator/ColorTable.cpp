/*  ColorTable class.  There are several things we need to do in this class.
    1. Read the old color tables and convert them to the newest format.
    Currently that means going from a number for each color to a bit for each
    primary color.  That allows bit operations on the tables.
    2. Write new color tables based upon the parameters set in the UI.

    Our color table is currently a 128x128x128 table accessed by a pixel's
    yuv value (in the order
  */

#include "ColorTable.h"

namespace qtool {
namespace colorcreator {

using namespace image;

ColorTable::ColorTable()
{
    table = new byte**[128];
    for (byte i = 0; i < 128; i++)
    {
        table[i] = new byte*[128];
    }
    for (byte i = 0; i < 128; i++)
    {
        for (byte j = 0; j < 128; j++)
        {
            table[i][j] = new byte[128];
        }
    }
}

// Read table from a file and determine the format
void ColorTable::read(QString filename)
{
    QFile file(filename);
    QTextStream out(stdout);
    QByteArray temp;
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
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
                temp = file.read(1);
                table[y][x][z] = temp[0];
            }
        }
    }
    file.close();
    /*Stats** stats = colorStats();
    for (byte i = 0; i < mainColors; i++)
    {
        for (byte j = 0; j < 3; j++)
        {
            stats[j][i].prbyte(i, j);
        }
    }*/
    filename.chop(4);
    QString newName = filename + "uvy.mtb";
    out << "Filename is :" << newName << "\n";
    write(newName);
}

/* WHen we read in a table of the old format we automatically convert
  it to the new format.  Here we just write out the table directly.
  */
void ColorTable::write(QString filename)
{
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
                temp[0] = table[y][x][z];
                file.write(temp);
            }
        }
    }
    file.close();
}

byte ColorTable::getColor(byte y, byte u, byte v)
{
    return table[v / 2][u / 2][y / 2];
}

void ColorTable::setColor(byte y, byte u, byte v, byte col)
{
    table[v / 2][u / 2][y / 2] = table[v / 2][u / 2][y / 2] | col;
}

void ColorTable::unSetColor(byte y, byte u, byte v, byte col)
{
    byte allCol = 0xFF;
    col = col ^ allCol;
    table[v / 2][u / 2][y / 2] = table[v / 2][u / 2][y / 2] & col;
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
                byte temp = GREY_BIT;
                Color color;
                color.setYuv((byte) y, (byte) x, (byte) z);
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
