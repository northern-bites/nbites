void LeastSquaresLine::addPoint(Point p)
{
   sumX2 += p.x() * p.x();
   sumY2 += p.y() * p.y();
   sumXY += p.x() * p.y();
   sumX  += p.x();
   sumY  += p.y();
   sum1  += 1;
}

void LeastSquaresLine::removePoint(Point p)
{
   sumX2 -= p.x() * p.x();
   sumY2 -= p.y() * p.y();
   sumXY -= p.x() * p.y();
   sumX  -= p.x();
   sumY  -= p.y();
   sum1  -= 1;
}

