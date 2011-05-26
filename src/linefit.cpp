#include "linefit.h"

LineFit::LineFit()
{
    clear();
}

void LineFit::clear()
{
    sumW = sumX = sumY = sumXY = sumX2 = sumY2 = 0;
          solved = false;
}

void LineFit::add(double w, double x, double y)
{
    sumW  += w;;
    sumX  += w * x;
    sumY  += w * y;
    sumX2 += w * x * x;
    sumY2 += w * y * y;
    sumXY += w * x * y;
    solved = false;
}

void LineFit::sub(double w, double x, double y)
{
    sumW  -= w;;
    sumX  -= w * x;
    sumY  -= w * y;
    sumX2 -= w * x * x;
    sumY2 -= w * y * y;
    sumXY -= w * x * y;
    solved = false;
}

void LineFit::add(double w, double y)
{
    sub(1.0, w, y);
}

void LineFit::draw()
{
    /*System.Drawing.Drawing2D.Matrix m = g.Transform;
    g.TranslateTransform((float)CenterX, (float)CenterY);
    g.RotateTransform((float)(FirstPrincipalAngle * 180.0 / Math.PI));
    g.DrawEllipse(pen, (float)    -FirstPrincipalLength,  (float)    -SecondPrincipalLength,
                  (float)(2 * FirstPrincipalLength), (float)(2 * SecondPrincipalLength));
    g.Transform = m;*/
}

void LineFit::solve()
{
    if (solved)
        return;

    double a = sumW * sumX2 - sumX * sumX;
    double b = sumW * sumY2 - sumY * sumY;
    double c = 2.0 * ((double)sumW * sumXY - (double)sumX * sumY);
    double d = sqrt(c * c + (a - b) * (a - b));

    pLen1 = sqrt(1.5 * (a + b + d)) / sumW;
    pLen2 = sqrt(1.5 * (a + b - d)) / sumW;

    if (d == 0.0)
        // No principal monemts
        uPAI = vPAI = 0.0;
    else
    {
        uPAI = sqrt(((a - b) / d + 1) / 2);
        vPAI = sqrt((1 - (a - b) / d) / 2);
        if (c < 0)
            uPAI = -uPAI;
    }
    solved = true;
}
