#pragma once

#include <vector>
#include "structs.h"

namespace man {
namespace vision {

class Blob {
public:
    Blob() : sw(0), sx(0), sy(0), sx2(0), sy2(0), sxy(0),
             mx(0), my(0), mxy(0), len(0), solved(false), count(0),
             minX(10000), maxX(0), minY(10000), maxY(0) {}
    inline void add(double w, double x, double y);
    inline void addPerimeter(double x, double y);
    inline void clearPerimeter() { perimeter.clear(); }
    double area() const { return sw; }
    int    getCount () const { return count; }
    double xCenter() const { return sx / sw; }
    double yCenter() const { return sy / sw; }
    double density() const { return sw / count; }
    double angle();
    double principalLength1();
    double principalLength2();

    int getPerimeter() { return perimeter.size(); }
    std::vector<point> getPerimeterPoints() { return perimeter; }
    void setRating(double r) { rating = r; }
    double getRating() { return rating; }

    double minX, maxX, minY, maxY;
private:
    void solve();

    double sw, sx, sy, sx2, sy2, sxy;
    double mx, my, mxy;
    double len;
    bool solved;
    int count;
    double rating;

    std::vector<point> perimeter;

};

inline void Blob::add(double w, double x, double y)
{
    sw += w;
    sx += w * x;
    sy += w * y;
    sx2 += w * x * x;
    sy2 += w * y * y;
    sxy += w * x * y;
    count++;
    solved = false;
}

inline void Blob::addPerimeter(double x, double y)
{
    point p = {x, y};
    perimeter.push_back(p);
    if(x < minX) minX = x;
    if(x > maxX) maxX = x;
    if(y < minY) minY = y;
    if(y > maxY) maxY = y;
}

}
}
