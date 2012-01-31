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
    enabled = false;
    table = new unsigned**[128];
    for (int i = 0; i < 128; i++)
    {
        table[i] = new unsigned*[128];
    }
    for (int i = 0; i < 128; i++)
    {
        for (int j = 0; j < 128; j++)
        {
            table[i][j] = new unsigned[128];
        }
    }
    colormap = new int*[2];
    for (int i = 0; i < 2; i++)
    {
        colormap[i] = new int[12];
    }
    // map of our old "soft color" values to new colors
    colormap[0][0] = Black;
    colormap[0][1] = White;
    colormap[0][2] = Green;
    colormap[0][3] = Blue;
    colormap[0][4] = Yellow;
    colormap[0][5] = Orange;
    colormap[0][6] = Yellow;
    colormap[0][7] = Blue;
    colormap[0][8] = Orange;
    colormap[0][9] = Orange;
    colormap[0][10] = Pink;
    colormap[0][11] = Navy;
    colormap[1][0] = Black;
    colormap[1][1] = Black;
    colormap[1][2] = Black;
    colormap[1][3] = Black;
    colormap[1][4] = Black;
    colormap[1][5] = Black;
    colormap[1][6] = White;
    colormap[1][7] = Green;
    colormap[1][8] = Pink;
    colormap[1][9] = Yellow;
    colormap[1][10] = Black;
    colormap[1][11] = Black;
}

void ColorTable::readOld(QString filename)
{
    QFile file(filename);
    QTextStream out(stdout);
    QByteArray temp;
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        out << "The file would not open properly" << "\n";
        enabled = false;
        return;
    }
    for (int y = 0; y < 128; ++y)
    {
        for (int x = 0; x < 128; x ++)
        {
            for (int z = 0; z < 128; z++)
            {
                temp = file.read(1);
                switch(temp[0])
                {
                case 0:
                    table[y][x][z] = UNDEFINED;
                    break;
                case 1:
                    table[y][x][z] = WHITE;
                    break;
                case 2:
                    table[y][x][z] = GREEN;
                    break;
                case 3:
                    table[y][x][z] = BLUE;
                    break;
                case 4:
                    table[y][x][z] = YELLOW;
                    break;
                case 5:
                    table[y][x][z] = ORANGE;
                    break;
                case 6:
                    table[y][x][z] = YELLOW | WHITE;
                    break;
                case 7:
                    table[y][x][z] = BLUE & GREEN;
                    break;
                case 8:
                    table[y][x][z] = ORANGE | PINK;
                    break;
                case 9:
                    table[y][x][z] = ORANGE | YELLOW;
                    break;
                case 10:
                    table[y][x][z] = PINK;
                    break;
                case 11:
                    table[y][x][z] = NAVY;
                    break;
                default:
                    table[y][x][z] = UNDEFINED;
                    break;
                }
            }
        }
    }
    file.close();
    /*Stats** stats = colorStats();
    for (int i = 0; i < mainColors; i++)
    {
        for (int j = 0; j < 3; j++)
        {
            stats[j][i].print(i, j);
        }
    }*/
    enabled = true;
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
        enabled = false;
        return;
    }
    for (int y = 0; y < 128; ++y)
    {
        for (int x = 0; x < 128; x ++)
        {
            for (int z = 0; z < 128; z++)
            {
                temp = file.read(1);
                table[y][x][z] = temp[0];
            }
        }
    }
    file.close();
    /*Stats** stats = colorStats();
    for (int i = 0; i < mainColors; i++)
    {
        for (int j = 0; j < 3; j++)
        {
            stats[j][i].print(i, j);
        }
    }*/
    enabled = true;
    filename.chop(4);
    QString newName = filename + "uvy.mtb";
    out << "Filename is :" << newName << "\n";
    writeNewFormat(newName);
}

/* WHen we read in a table of the old format we automatically convert
  it to the new format.  Here we just write out the table directly.
  */
void ColorTable::writeNewFormat(QString filename)
{
    QFile file(filename);
    QTextStream out(stdout);
    QByteArray temp;
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        out << "The file would not open properly" << "\n";
        enabled = false;
        return;
    }
    for (int y = 0; y < 128; ++y)
    {
        for (int x = 0; x < 128; x ++)
        {
            for (int z = 0; z < 128; z++)
            {
                temp[0] = table[y][x][z];
                file.write(temp);
            }
        }
    }
    file.close();
}

int ColorTable::tableIndex(int high7, int mid7, int low7)
{
    return ((high7 & 0xFE) << 13) | ((mid7 & 0xFE) << 6) | ((low7 & 0xFE) >> 1);
}

unsigned ColorTable::index(int y, int u, int v)
{
    //return table[tableIndex(v + 128, u + 128, y)];
    return table[v / 2][u / 2][y / 2];
}

void ColorTable::setColor(int y, int u, int v, unsigned col)
{
    table[v / 2][u / 2][y / 2] = table[v / 2][u / 2][y / 2] | col;
}

void ColorTable::unSetColor(int y, int u, int v, unsigned col)
{
    unsigned allCol = 0xFF;
    col = col ^ allCol;
    table[v / 2][u / 2][y / 2] = table[v / 2][u / 2][y / 2] & col;
}

int ColorTable::getUpdatedColor(int y, int u, int v)
{
    unsigned temp = index(y, u, v);
    switch (temp)
    {
    case UNDEFINED:
        return Black;
    case WHITE:
        return White;
    case GREEN:
        return Green;
    case YELLOW:
        return Yellow;
    case BLUE:
        return Blue;
    case ORANGE:
        return Orange;
    default:
        return Black;
    }

    /*int temp = index(y, u, v);
    while (temp < 0)
    {
        temp = 256 + temp;
    }
    temp = temp % 256;
    return colormap[0][temp];*/
}

Stats** ColorTable::colorStats()
{
    Stats** colorStats;
    colorStats = new Stats*[3];
    for (int i = 0; i < 3; i++)
    {
        colorStats[i] = new Stats[mainColors];
    }

    // initialize stats
    for (int h = 0; h < 3; ++h)
        for (int c = 0; c < mainColors; ++c)
            colorStats[h][c] = Stats();

    // loop through the whole table collecting stats
    for (int y = 0; y < 256; y += 2)
        for (int v = 0; v < 256; v += 2)
            for (int u = 0; u < 256; u += 2)
            {
                int c = index(y, u, v);
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
                    for (int q = 0; q < 2; ++q)
                    {
                        int ci = colormap[q][c];
                        if (ci < mainColors)
                        {
                            colorStats[0][ci].add(y);
                            colorStats[1][ci].add(u);
                            colorStats[2][ci].add(v);
                        }
                    }
            }

    return colorStats;
}

/* Writes out a color table.  The "new" part of the format is that it
  writes the color table using bitwise color definitions instead of the
  old integer definitions.
  @param filename        the name to write
  */
void ColorTable::write(QString filename, float** fltSliders,
                                int** intSliders, unsigned* bitColor)
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
    for (byte z = 0; z < 255; z+=2)
    {
        for (byte x = 0; x < 255; x+=2)
        {
            for (byte y = 0; y < 255; y+=2)
            {
                temp[0] = GREY_COL;
                Color col;
                col.setYuv(y, x, z);
                for (int c = Orange; c < Black; c++)
                {
                    bool ok = false;
                    if (fltSliders[hMin][c] >= fltSliders[hMax][c])
                    {
                        if (col.getH() >= fltSliders[hMin][c] || col.getH() <= fltSliders[hMax][c])
                        {
                            ok = true;
                        }
                    } else
                    {
                        if (col.getH() >= fltSliders[hMin][c] && col.getH() <= fltSliders[hMax][c])
                        {
                            ok = true;
                        }
                    }
                    if (ok && y >= intSliders[yMin][c] && y <= intSliders[yMax][c] &&
                            col.getS() >= fltSliders[sMin][c] && col.getS() <= fltSliders[sMax][c] && col.getZ() >= fltSliders[zMin][c] &&
                            col.getZ() <= fltSliders[zMax][c])
                    {
                        if (c == Orange) {
                            count++;
                        }
                        temp[0] = temp[0] | bitColor[c];
                    }
                }
                file.write(temp);
            }
        }
    }
    out << "Count was " << count << "\n" << endl;
    file.close();
}

/* Writes a color table of the old format.  Old meaning integer definitions.
  So we should never use this anymore.
  @param filename        the name of the file to write
  */
void ColorTable::writeOld(QString filename, float** fltSliders,
                                int** intSliders)
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
    for (byte z = 0; z < 255; z+=2)
    {
        for (byte x = 0; x < 255; x+=2)
        {
            for (byte y = 0; y < 255; y+=2)
            {
                temp[0] = GREY_COL;
                Color col;
                col.setYuv(y, x, z);
                bool orange = false;
                bool yellow = false;
                bool blue = false;
                for (int c = Orange; c < Black; c++)
                {
                    bool ok = false;
                    if (fltSliders[hMin][c] > fltSliders[hMax][c])
                    {
                        if (col.getH() >= fltSliders[hMin][c] || col.getH() <= fltSliders[hMax][c])
                        {
                            ok = true;
                        }
                    } else
                    {
                        if (col.getH() >= fltSliders[hMin][c] && col.getH() <= fltSliders[hMax][c])
                        {
                            ok = true;
                        }
                    }
                    if (ok && y >= intSliders[yMin][c] && y <= intSliders[yMax][c] &&
                            col.getS() >= fltSliders[sMin][c] && col.getS() <= fltSliders[sMax][c] && col.getZ() >= fltSliders[zMin][c] &&
                            col.getZ() <= fltSliders[zMax][c])
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

}
}
