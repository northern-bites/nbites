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
    byte*** getTable() {return table; }
    void read(QString filename);
    void writeNewFormat(QString filename);
    byte getColor(byte y, byte u, byte v);
    void setColor(byte y, byte u, byte v, byte col);
    void unSetColor(byte y, byte u, byte v, byte col);

    //color table output methods
    static void write(QString filename, ColorSpace* colorSpaces);

private:
    byte*** table;
};

}
}

#endif // COLORTABLE_H
