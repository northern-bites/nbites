#ifndef Blobs_h_defined
#define Blobs_h_defined

#include "Common.h"
#include "VisionStructs.h"
#include "VisionHelpers.h"
#include "Blob.h"

static const int BADONE = -10000;

class Blobs {
public:
    Blobs(int howMany);
    virtual ~Blobs() {}

	void init();
	void init(int which) {blobs[which].init();}
	void blobIt(int x, int y, int h, bool merge);
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

// getters
	int number() const {return numBlobs;}
	Blob get(int which) const {return blobs[which];}

private:
	int total;
    int runsize;
    int biggestRun;
    int maxHeight;
    int maxOfBiggestRun;
    int numberOfRuns;
    int indexOfBiggestRun;
    int numBlobs;
    //blob checker, obj, pole, leftBox, rightBox;
    Blob* blobs;
};
#endif
