#ifndef COLORTABLE_H
#define COLORTABLE_H
#include <QString>
#include <QFile>
#include <QTextStream>

#include "Stats.h"

#define UNDEFINED 0x00
#define WHITE     0x01
#define GREEN     0x02
#define BLUE      0x04
#define YELLOW    0x08
#define ORANGE    0x10
#define PINK      0x20
#define NAVY      0x40

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

private:
    static const int mainColors = 5;
    int** colormap;
    unsigned*** table;
    bool enabled;
};

}
}

#endif // COLORTABLE_H
