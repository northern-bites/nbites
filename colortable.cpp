#include "colortable.h"

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
        colormap[i] = new int[10];
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
    Stats** stats = colorStats();
    for (int i = 0; i < mainColors; i++)
    {
        for (int j = 0; j < 3; j++)
        {
            stats[j][i].print(i, j);
        }
    }
    enabled = true;
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

int ColorTable::getUpdatedColor(int y, int u, int v)
{
    int temp = index(y, u, v);
    while (temp < 0)
    {
        temp = 256 + temp;
    }
    temp = temp % 256;
    return colormap[0][temp];
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
                if (c < 10)
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

LineFit* ColorTable::colorZones()
{
    LineFit* colorZones =  new LineFit[mainColors];

    for (int c = 0; c < mainColors; ++c)
        colorZones[c] = LineFit();

    for (int y = 0; y < 256; y += 2)
        for (int v = -128; v < 128; v += 2)
            for (int u = -128; u < 128; u += 2)
            {
                int c = index(y, u, v);
                if (c < mainColors)
                    for (int q = 0; q < 2; ++q)
                    {
                        int ci = colormap[q][c];
                        if (ci >= 0)
                            colorZones[ci].add(u, v);
                    }
            }

    return colorZones;
}
