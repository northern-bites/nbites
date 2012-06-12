#include "PaintLocalization.h"

namespace qtool 
{
  namespace image 
  {

    PaintLocalization::PaintLocalization(QObject *parent):
      BMPImage(parent), lastParticles(), lastXEst(FIELD_HEIGHT*0.5f), 
      lastYEst(FIELD_WIDTH*0.5f), lastHEst(0.0f)
    {
      bitmap = QImage(FIELD_WIDTH, FIELD_HEIGHT, QImage::Format_ARGB32_Premultiplied);
    }
    
    void PaintLocalization::updateWithParticles(PF::ParticleSet particles)
    {
      lastParticles = particles;

      buildBitmap();
    }
    
    void PaintLocalization::updateEstimates(float x, float y, float h)
    {
      lastXEst = x;
      lastYEst = y;
      lastHEst = h;
    }

    void PaintLocalization::drawParticle(PF::LocalizationParticle particle, QImage& bitmap)
    {
      QPainter painter(&bitmap);

      QPoint particleCenter(particle.getLocation().x,
			    particle.getLocation().y);

      painter.drawEllipse(particleCenter,
			  5,
			  5);

      painter.drawLine(particle.getLocation().x,
		       particle.getLocation().y,
		       5 * std::cos(particle.getLocation().heading) + particle.getLocation().x,
		       5 * std::sin(particle.getLocation().heading) + particle.getLocation().y);
    }

    void PaintLocalization::buildBitmap()
    {

      bitmap.fill(Qt::transparent);

      PF::ParticleIt i = lastParticles.begin();
      for(; i != lastParticles.end(); ++i)
      {
          this->drawParticle(*i, bitmap);
      }

      drawEstimate(bitmap);
    }

    void PaintLocalization::drawEstimate(QImage& bitmap)
    {
      QPainter painter(&bitmap);

      painter.setPen(Qt::red);

      painter.drawEllipse(QPoint(lastXEst, lastYEst),
			  10,
			  10);

      painter.drawLine(lastXEst,
		       lastYEst,
		       10 * std::cos(lastHEst) + lastXEst,
		       10 * std::sin(lastHEst) + lastYEst);
    }

  }
}
