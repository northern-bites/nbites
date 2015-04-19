#pragma once

#include <iostream>
#include <list>
#include <vector>

#include "Blob.h"
#include "structs.h"

// TODO add histeresis thresholding
// TODO more serious testing
// TODO add documentation

namespace man {
namespace vision {

enum class NeighborRule { four, eight };

template <typename T>
class Blobber {
public:
    Blobber(T const *pixels_, int width_, int height_,
            int pixelPitch_, int rowPitch_);
    ~Blobber();
    void run(NeighborRule rule, double lowThreshold,
             double highThreshold, double minArea, double walkThresh);
    std::vector<Blob>& getResult() { return results; }

private:
    void initializeMark();
    void initializeNeighborRule(NeighborRule rule);

    void walkPerimeter(int x, int y, Blob* b);
    int neighborhoodState(int x, int y);
    int nextDirection(int x, int y, int prevDir);


    // TODO pass efficient threshold object, instead of just low and high doubles
    // NOTE Daniel, FuzzyLogic.h includes such an object, we are using this
    //      in hough stuff, so makes sense share code
    inline void explore(Blob &blob, T const *p, uint8_t *m,
                        double lowThreshold, double highThreshold);
    inline uint8_t indexIntoMark(int x, int y);
    inline int calculateXIndex(T const *pixelPt);
    inline int calculateYIndex(T const *pixelPt);
    inline double weight(double value, double lowThreshold, double highThreshold);

    // TODO refactor into image struct?
    T const *pixels;
    int width;
    int height;
    int pixelPitch;
    int rowPitch;

    // Trinary data type
    // 0 is unexplored, 1 is explored non-blob, all else is part of blob
    uint8_t *mark;

    // TODO stop using stl::list
    // TODO stop using stl::vector
    std::list<T const *> list;
    std::vector<Blob> results;

    int neighbors;
    int table[8];

    static const int N = 0;
    static const int E = 1;
    static const int S = 2;
    static const int W = 3;
};

template <typename T>
Blobber<T>::Blobber(T const *pixels_, int width_, int height_,
                    int pixelPitch_, int rowPitch_)
    : pixels(pixels_), width(width_), height(height_),
      pixelPitch(pixelPitch_), rowPitch(rowPitch_)
{
    mark = new uint8_t[(width + 2)*(height + 2)];

    table[0] = -pixelPitch;
    table[1] = pixelPitch;
    table[2] = -rowPitch;
    table[3] = rowPitch;
    table[4] = -rowPitch - pixelPitch;
    table[5] = -rowPitch + pixelPitch;
    table[6] = rowPitch - pixelPitch;
    table[7] = rowPitch + pixelPitch;
}

template <typename T>
Blobber<T>::~Blobber()
{
    delete[] mark;
}

template <typename T>
void Blobber<T>::initializeMark()
{
    for (int i = 0; i < height + 2; i++) {
        for (int j = 0; j < width + 2; j++) {
            if (i == 0 || j == 0 || i == height + 1 || j == width + 1) {
                mark[i*(width+2) + j] = 1;

            }
            else {
                mark[i*(width+2) + j] = 0;
            }
        }
    }
}

template <typename T>
void Blobber<T>::initializeNeighborRule(NeighborRule rule)
{
    if (rule == NeighborRule::four)
        neighbors = 4;
    else if (rule == NeighborRule::eight)
        neighbors = 8;
    else
        throw;
}

template <typename T>
void Blobber<T>::run(NeighborRule rule, double lowThreshold, double highThreshold,
                     double minArea, double walkThresh)
{
    initializeMark();
    initializeNeighborRule(rule);
    list.clear();
    results.clear();

    T const *p;
    uint8_t *m = mark + width + 1;
    for (int y = 0; y < height; y++) {
        p = pixels + y*rowPitch;
        m += 2;
        for (int x = 0; x < width; x++) {
            Blob blob;
            explore(blob, p, m, lowThreshold, highThreshold);

            if (!list.empty()) {
                do {
                    T const *pFromList = list.back();
                    list.pop_back();
                    for (int i = 0; i < neighbors; i++) {
                        T const *pNeighbor = pFromList + table[i];
                        uint8_t *mNeighbor = (mark + width + 3) +
                                          calculateYIndex(pNeighbor) * (width + 2) +
                                          calculateXIndex(pNeighbor);
                        explore(blob, pNeighbor, mNeighbor,
                                lowThreshold, highThreshold);
                    }
                } while (!list.empty());

                if (blob.area() >= minArea) {
                    if(blob.area() > walkThresh) {
                        walkPerimeter(x, y, &blob);
                    }
                    results.push_back(blob);
                }
            }
            p += pixelPitch;
            m++;
        }
    }
}

template <typename T>
inline void Blobber<T>::explore(Blob &blob, T const *p, uint8_t *m,
                                double lowThreshold, double highThreshold)
{
    if (!*m) {
        *m = 1;
        double w = weight(static_cast<double>(*p),
                               lowThreshold, highThreshold);
        if (w) {
            *m = 2;
            int y = calculateYIndex(p);
            int x = calculateXIndex(p);

            blob.add(w, static_cast<double>(x), static_cast<double>(y));
            list.push_front(p);
        }
    }
}

template <typename T>
void Blobber<T>::walkPerimeter(int x, int y, Blob* b)
{
    int startX = x;
    int startY = y;
    int currentX = x;
    int currentY = y;

    int dir = N;

    do {
        b->addPerimeter(currentX, currentY);
        dir = nextDirection(currentX, currentY, dir);

        switch(dir)
        {
        case N:
            currentY -= 1;
            break;
        case E:
            currentX += 1;
            break;
        case S:
            currentY += 1;
            break;
        case W:
            currentX -= 1;
            break;
        default:
            std::cout << "BUG IN PERIMETER WALKING" << std::endl;
            b->clearPerimeter();
            return;
        }
    } while(currentX != startX || currentY != startY);

}

template <typename T>
int Blobber<T>::neighborhoodState(int x, int y)
{
    int state = 0;
    if(indexIntoMark(x-1, y-1) > 1) state |= 1;
    if(indexIntoMark(x, y-1) > 1) state |= 2;
    if(indexIntoMark(x-1, y) > 1) state |= 4;
    if(indexIntoMark(x, y) > 1) state |= 8;

    return state;
}

template <typename T>
int Blobber<T>::nextDirection(int x, int y, int prevDir)
{
    int state = neighborhoodState(x, y);

    switch(state){
    case 0:
        return -1;
    case 1: return W;
    case 2: return N;
    case 3: return W;
    case 4: return S;
    case 5: return S;
    case 6:
        if(prevDir == E) return N;
        if(prevDir == W) return S;
        return -1;
    case 7: return S;
    case 8: return E;
    case 9:
        if(prevDir == S) return E;
        if(prevDir == N) return W;
        return -1;
    case 10: return N;
    case 11: return W;
    case 12: return E;
    case 13: return E;
    case 14: return N;
    default:
        return -1;
    }
}

template <typename T>
inline uint8_t Blobber<T>::indexIntoMark(int x, int y)
{
    return mark[width + 2 + y * (width + 2) + x + 1];
}

template <typename T>
inline int Blobber<T>::calculateXIndex(T const *p)
{
    return (p - pixels - rowPitch*calculateYIndex(p)) / pixelPitch;
}

template <typename T>
inline int Blobber<T>::calculateYIndex(T const *p)
{
    return (p - pixels) / (pixelPitch * width);
}

template <typename T>
inline double Blobber<T>::weight(double value, double lowThreshold, double highThreshold)
{
    if (value < lowThreshold)
        return 0;
    else if (value > highThreshold)
        return 1;
    return (1 / (highThreshold - lowThreshold)) * (value - lowThreshold);
}

}
}
