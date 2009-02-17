#ifndef VisualLandmark_h_defined__
#define VisualLandmark_h_defined__

/*
 * The abstract superclass to any object vision can identify.  Subclasses
 * include VisualCorner and VisualFieldObject.
 */
class VisualLandmark;

#include "ConcreteLandmark.h"
#include "Structs.h"

// What is the certainty associated with the identification of the object
enum certainty {
    NOT_SURE,
    MILDLY_SURE,
    _SURE
};

// Are the heights and widths from Vision to be trusted? Used in localization
enum distanceCertainty {
    BOTH_UNSURE,
    WIDTH_UNSURE,
    HEIGHT_UNSURE,
    BOTH_SURE
};



class VisualLandmark {


public:
    // Constructor
    VisualLandmark(certainty _idCertainty = NOT_SURE,
                   distanceCertainty _distanceCertainty = BOTH_UNSURE,
                   ConcreteLandmark * _concreteLandmark = 0);

    // Copy constructor
    VisualLandmark(const VisualLandmark&);

    // Destructor
    virtual ~VisualLandmark();


protected:
    // How sure are we that we identified the correct object
    // (for example, blue goal left post versus blue goal right post)
    certainty idCertainty;
    distanceCertainty distanceCert;
    ConcreteLandmark * concreteLandmark;

public:
    // Getters
    const certainty getIDCertainty() const { return idCertainty; }
    const distanceCertainty getDistanceCertainty() const {
        return distanceCert;
    }
    const ConcreteLandmark * getConcreteLandmark() const {
        return concreteLandmark;
    }


    // Setters
    virtual void setIDCertainty(certainty c) { idCertainty = c; }
    void setDistanceCertainty(distanceCertainty c) { distanceCert = c; }
    void setConcreteLandmark(ConcreteLandmark * c) { concreteLandmark = c; }
    // Calculate and set the standard deviation of the measurement while
    // setting the distance; should be used whenever a legitimate distance
    // measurement is set
    // pure virtual; to be defined by implementing subclass
};


#endif
