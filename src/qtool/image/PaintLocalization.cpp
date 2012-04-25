#include "PaintLocalization.h"

namespace qtool 
{
  namespace image 
  {

    PaintLocalization::PaintLocalization(QObject *parent):
      BMPImage(parent)
    {
      bitmap = QImage(FIELD_WIDTH, FIELD_HEIGHT, QImage::Format_ARGB32_Premultiplied);

      lastParticles.push_back(PF::LocalizationParticle(PF::Location(50.0f, 50.0f, 0.1f), 0.0f));
    }
    
    void PaintLocalization::updateWithParticles(PF::ParticleSet particles)
    {
      lastParticles = particles;

      buildBitmap();
    }

    void PaintLocalization::drawParticle(PF::LocalizationParticle particle, QImage& bitmap)
    {
      bitmap.fill(0x00000000);

      QPainter painter(&bitmap);

      QPoint particleCenter(particle.getLocation().x,
			    particle.getLocation().y);

      painter.drawEllipse(particleCenter,
			  10,
			  10);

      painter.drawLine(particle.getLocation().x,
		       particle.getLocation().y,
		       10 * std::cos(particle.getLocation().heading) + particle.getLocation().x,
		       10 * std::sin(particle.getLocation().heading) + particle.getLocation().y);
    }

    void PaintLocalization::buildBitmap()
    {
      PF::ParticleIt i = lastParticles.begin();
      for(; i != lastParticles.end(); ++i)
      {
	this->drawParticle(*i, bitmap);
      }
    }
  }
}
