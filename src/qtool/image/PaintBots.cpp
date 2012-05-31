#include "PaintBots.h"
#include "viewer/FieldViewer.h"
#include "BMPImage.h"
#include <vector>
#include "image/Color.h"
#include "viewer/BotLocs.h"

namespace qtool {
  namespace image {

    using namespace viewer;

    PaintBots::PaintBots(QObject *parent):
      BMPImage(parent)
    {
      bitmap = QPixmap(FIELD_WIDTH, FIELD_HEIGHT);
      locs = new BotLocs();
    }
    // Paints the robots onto the field
    void PaintBots::buildBitmap()
    {          
      bitmap.fill(Qt::transparent);
      QPainter painter(&bitmap);

      QColor orange = QColor::fromRgb(Color_RGB[Orange]);
      painter.setPen(orange); //pen can always be orange

      for(int i= 1; i < locs->getSize(); i++) { //the first bot doesn't really exist
	//to draw bot, fill with solid brush
	painter.setBrush(orange);
	painter.drawEllipse(QPoint(locs->getX(i), locs->getY(i)), 10, 10);
	
	//to draw uncertainty, draw with pen, not brush
	painter.setBrush(Qt::NoBrush);
	painter.drawEllipse(QPoint(locs->getX(i), locs->getY(i)), locs->getXUncert(i), locs->getYUncert(i));
      }
    }

  }
}

