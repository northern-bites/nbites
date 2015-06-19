#pragma once

#include <cstring>
#include <utility>
#include <limits>

#include "Images.h"

namespace man {
namespace vision {

template <typename T>
class MathMorphology {
public:
    MathMorphology();
    MathMorphology(int numIndices_, const std::pair<int, int>* structuringElement);
    ~MathMorphology();

    void setStructuringElement(int numIndices_, const std::pair<int, int>* structuringElement);
    void erosion(const messages::MemoryImage<T>& in, messages::PackedImage<T>& out) const;
    void dilation(const messages::MemoryImage<T>& in, messages::PackedImage<T>& out) const;
    void opening(const messages::MemoryImage<T>& in, messages::PackedImage<T>& out) const;
    void closing(const messages::MemoryImage<T>& in, messages::PackedImage<T>& out) const;

private:
    int numIndices;
    std::pair<int, int>* indices;
};

// TODO check for null structuring elem? default structuring elem?
template <typename T>
MathMorphology<T>::MathMorphology()
    : numIndices(0), indices(NULL)
{}

template <typename T>
MathMorphology<T>::MathMorphology(int numIndices_, const std::pair<int, int>* structuringElement)
    : numIndices(numIndices_), indices(NULL)
{
    setStructuringElement(numIndices, structuringElement);
}

template <typename T>
MathMorphology<T>::~MathMorphology()
{
    delete[] indices;
}


template <typename T>
void MathMorphology<T>::setStructuringElement(int numIndices_, const std::pair<int, int>* structuringElement)
{
    if (indices != NULL)
        delete[] indices;

    numIndices = numIndices_;
    indices = new std::pair<int, int>[numIndices];
    memcpy(indices, structuringElement, numIndices*sizeof(std::pair<int, int>));
}

// TODO optimize
template <typename T>
void MathMorphology<T>::erosion(const messages::MemoryImage<T>& in, messages::PackedImage<T>& out) const
{
    int wd = in.width();
    int ht = in.height();

    int ppitch = in.pixelPitch();
    int rpitch = in.rowPitch();

    T* inStart = in.pixelAddress(0, 0);
    T* inEnd = in.pixelAddress(wd-1, ht-1);

    out.makeMeCopyOf(in, messages::PackedImage<T>::ExactSize);
    for (int y = 0; y < ht; y++) {
        T* inRow = in.pixelAddress(0, y);
        T* outRow = out.pixelAddress(0, y);
        for (int x = 0; x < wd; x++, inRow += ppitch, outRow += ppitch) {
            T min = std::numeric_limits<T>::max();
            for (int i = 0; i < numIndices; i++) {
                T* pt = inRow + indices[i].first*ppitch + indices[i].second*rpitch;
                if (pt >= inStart && pt <= inEnd) {
                    T cur = *pt;
                    if (min > cur)
                        min = cur;
                }
            }
            *outRow = min;
        }
    }
}

template <typename T>
void MathMorphology<T>::dilation(const messages::MemoryImage<T>& in, messages::PackedImage<T>& out) const
{
    int wd = in.width();
    int ht = in.height();

    int ppitch = in.pixelPitch();
    int rpitch = in.rowPitch();

    T* inStart = in.pixelAddress(0, 0);
    T* inEnd = in.pixelAddress(wd-1, ht-1);

    out.makeMeCopyOf(in, messages::PackedImage<T>::ExactSize);
    for (int y = 0; y < ht; y++) {
        T* inRow = in.pixelAddress(0, y);
        T* outRow = out.pixelAddress(0, y);
        for (int x = 0; x < wd; x++, inRow += ppitch, outRow += ppitch) {
            T max = std::numeric_limits<T>::min();
            for (int i = 0; i < numIndices; i++) {
                T* pt = inRow + indices[i].first*ppitch + indices[i].second*rpitch;
                if (pt >= inStart && pt <= inEnd) {
                    T cur = *pt;
                    if (max < cur)
                        max = cur;
                }
            }
            *outRow = max;
        }
    }
}

template <typename T>
void MathMorphology<T>::opening(const messages::MemoryImage<T>& in, messages::PackedImage<T>& out) const
{
    messages::PackedImage<T> eroded;
    erosion(in, eroded);
    dilation(eroded, out);
}

template <typename T>
void MathMorphology<T>::closing(const messages::MemoryImage<T>& in, messages::PackedImage<T>& out) const
{
    messages::PackedImage<T> dilated;
    dilation(in, dilated);
    erosion(dilated, out);
}

}
}
