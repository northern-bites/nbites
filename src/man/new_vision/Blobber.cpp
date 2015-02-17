#include "Blobber.h"

namespace man {
namespace vision {

template <typename T>
Blobber<T>::Blobber(T const *pixels_, int width_, int height_, 
                    int pixelPitch_, int rowPitch_)
    : pixels(pixels_), width(width_), height(height_), 
      pixelPitch(pixelPitch_), rowPitch(rowPitch_)
{
    mark = new bool[(width + 2)*(height + 2)];

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
            if (i == 0 || j == 0 || i == height + 1 || j == width + 1)
                mark[i*(rowPitch+2) + j] = true;
            else
                mark[i*(rowPitch+2) + j] = false;
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
void Blobber<T>::run(NeighborRule rule, double lowThreshold, double highThreshold, double minArea)
{
    initializeMark();
    initializeNeighborRule(rule);
    list.clear();
    results.clear();
    
    T const *p;
    bool *m = mark + width + 1;
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
                        bool *mNeighbor = (mark + width + 3) +
                                          calculateYIndex(pNeighbor) * (width + 2) +
                                          calculateXIndex(pNeighbor);
                        explore(blob, pNeighbor, mNeighbor,
                                lowThreshold, highThreshold);
                    }
                } while (!list.empty());

                if (blob.area() >= minArea)
                    results.push_back(blob);
            }
            p += pixelPitch;
            m++;
        }
    }
}

template <typename T>
inline void Blobber<T>::explore(Blob &blob, T const *p, bool *m,
                                double lowThreshold, double highThreshold)
{
    if (!*m) {
        *m = true;
        double w = weight(static_cast<double>(*p), 
                               lowThreshold, highThreshold);
        if (w) {
            int y = calculateYIndex(p);
            int x = calculateXIndex(p);
            blob.add(w, static_cast<double>(x), static_cast<double>(y));
            list.push_front(p);
        }
    }
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
