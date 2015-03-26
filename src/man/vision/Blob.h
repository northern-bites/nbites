#pragma once

#include <vector>

namespace man {
namespace vision {

struct point {
    double x;
    double y;
};

class Blob {
public:
    Blob() : sw(0), sx(0), sy(0), sx2(0), sy2(0), sxy(0),
             mx(0), my(0), mxy(0), len(0), solved(false) {}
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
}

}
}
