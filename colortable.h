#ifndef COLORTABLE_H
#define COLORTABLE_H
#include <QString>
#include <QFile>
#include <QTextStream>
#include "stats.h"
#include "linefit.h"

class ColorTable
{
public:
    enum Colors {Orange, Blue, Yellow, Green, White, Pink, Navy, Black};
    ColorTable();
    unsigned*** getTable() {return table; }
    void read(QString filename);
    int tableIndex(int high7, int mid7, int low7);
    unsigned index(int y, int u, int v);
    Stats** colorStats();
    LineFit*  colorZones();
    bool isEnabled() {return enabled;}
    int getUpdatedColor(int y, int u, int v);

private:
    static const int mainColors = 5;
    int** colormap;
    unsigned*** table;
    bool enabled;
};

#endif // COLORTABLE_H
