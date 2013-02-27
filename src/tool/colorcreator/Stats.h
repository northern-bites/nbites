#ifndef STATS_H
#define STATS_H
#include <QTextStream>
#include <cmath>

namespace qtool {
namespace colorcreator {

class Stats
{
public:
    Stats();
    void clear();
    void add(double x);
    void add(Stats s);
    int count() {return n;}
    int mean() {return n > 0 ? sum / n : 0;}
    double stDev() { return n > 1 ? sqrt(n * sum2 - sum * sum) / n : 0;}
    double rms() { return sqrt(sum2 / n);}
    double getMin() { return minimum;}
    double getMax() { return maximum;}
    double min(double a, double b) { if (a < b) return a; return b;}
    double max(double a, double b) { if (a < b) return b; return a;}
    void print(int x, int y);

private:
    int n;
    double sum;
    double sum2;
    double minimum;
    double maximum;
};

}
}

#endif // STATS_H
