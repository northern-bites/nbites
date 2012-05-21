/**
 * @class PaintLocalization
 *
 * This class paints localization information (particles,
 * estimated position) to a BMP image with dimensions equal
 * to those of the field. It should be overlayed on top of 
 * the field image.
 *
 * @author Ellis Ratner
 * @date April 2012
 *
 */

#ifndef PAINT_LOCALIZATION_H
#define PAINT_LOCALIZATION_H

#include <QtGui>
#include <vector>
#include <cmath>

#include "image/BMPImage.h"
#include "man/include/FieldConstants.h"
#include "man/noggin/ParticleFilter.h"

namespace qtool 
{
  namespace image 
  {
    class PaintLocalization : public image::BMPImage
    {
      Q_OBJECT
    public:
      PaintLocalization(QObject *parent = 0);

      unsigned getWidth() const { return FIELD_WIDTH; }
      unsigned getHeight() const { return FIELD_HEIGHT; }

      void updateWithParticles(PF::ParticleSet particles);
      void updateEstimates(float x, float y, float h);

    protected:
      void drawParticle(PF::LocalizationParticle particle, QPixmap& bitmap);
      void drawEstimate(QPixmap& bitmap);

      virtual void buildBitmap();

      PF::ParticleSet lastParticles;
      float lastXEst;
      float lastYEst;
      float lastHEst;

    };
  }
}

#endif // PAINT_LOCALIZATION_H
