#ifndef Blobs_h_defined
#define Blobs_h_defined

#include "Common.h"
#include "VisionStructs.h"
#include "VisionHelpers.h"
#include "Blob.h"

namespace man {
namespace vision {

static const int BADONE = -10000;

class Blobs {
public:
    Blobs(int howMany);
    virtual ~Blobs() {}

    void init();
    void init(int which) {blobs[which].init();}
    void blobIt(int x, int y, int h, bool merge);
    void add(Blob blob);
    void setLeft(int which, int a);
    void setRight(int which, int a);
    void setTop(int which, int a);
    void setBottom(int which, int a);
    Blob* getTopAndMerge(int maxY);
    Blob* getWidest();
    int getBiggest();
    void zeroTheBlob(int which);
    void mergeBlobs(int first, int second);
    void checkForMergers();
    bool blobsOverlap(int first, int second);
    void sort();
    void newBlobIt(int i, int j, bool newBlob); //to be used for robot blobbing

    // getters
    int number() const {return numBlobs;}
    Blob get(int which) const {return blobs[which];}
    int getBiggest(int i);

    //setters
    void set(int which, Blob blob) const {blobs[which] = blob;}

private:
    int total;
    int runsize;
    int biggestRun;
    int maxHeight;
    int maxOfBiggestRun;
    int numberOfRuns;
    int indexOfBiggestRun;
    int numBlobs;
    Blob* blobs;
};

}
}
#endif
