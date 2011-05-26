#include "stats.h"

Stats::Stats()
{
    clear();
}

void Stats::clear()
{
    n = 0;
    sum = 0;
    sum2 = 0;
    minimum = 0;
    maximum = 0;
}

void Stats::add(double x)
{
    ++n;
    sum += x;
    sum2 += x * x;
    if (n == 1)
    {
        minimum = x;
        maximum = x;
    }
    else{
        minimum = min(minimum, x);
        maximum = max(maximum, x);
    }
}

void Stats::add(Stats s)
{
    n += s.n;
    sum += s.sum;
    sum2 += s.sum2;
    if (s.n > 0)
    {
        if (n == 1)
        {
            minimum = s.minimum;
            maximum = s.maximum;
        }
        else{
            minimum = min(minimum, s.getMin());
            maximum = max(maximum, s.getMax());
        }
    }
}

void Stats::print(int x, int y)
{
    QTextStream out(stdout);
    out << "Stats for color " << x << " index " << y << " " << n << " entries.\n";
    out << "Min " << getMin() << " Max " << getMax() << " mean " << mean() << "\n";
    out << "StdDev " << stDev() << " " << rms() << "\n";
}
