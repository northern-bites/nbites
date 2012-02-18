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
    unsigned*** getTable() {return table; }
    void read(QString filename);
    void writeNewFormat(QString filename);
    unsigned getColor(int y, int u, int v);
    void setColor(int y, int u, int v, unsigned col);
    void unSetColor(int y, int u, int v, unsigned col);

    //color table output methods
    static void write(QString filename, ColorSpace* colorSpaces);

private:
    unsigned*** table;
};

}
}

#endif // COLORTABLE_H
