#ifndef VisualLandmark_h_defined__
#define VisualLandmark_h_defined__

/*
 * The abstract superclass to any object vision can identify that can be used
 * for localization.
 * Subclasses include VisualCorner, VisualFieldObject, VisualLine
 */

#include "ConcreteLandmark.h"

#include <list>

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



class VisualLandmark
{
public:
    // Constructor
    VisualLandmark(int _id ,certainty _idCertainty = NOT_SURE,
                   distanceCertainty _distanceCertainty = BOTH_UNSURE,
                   ConcreteLandmark* _concreteLandmark = 0)
        : id(_id), idCertainty(_idCertainty), distanceCert(_distanceCertainty),
          concreteLandmark(_concreteLandmark) { }

    // Copy constructor
    VisualLandmark(const VisualLandmark& other)
        : id(other.id), idCertainty(other.idCertainty),
          distanceCert(other.distanceCert),
          concreteLandmark(other.concreteLandmark) { }
    // Destructor
    virtual ~VisualLandmark() {}

protected:
    // How sure are we that we identified the correct object
    // (for example, blue goal left post versus blue goal right post)
    int id;
    certainty idCertainty;
    distanceCertainty distanceCert;
    ConcreteLandmark * concreteLandmark;
    std::list<const ConcreteLandmark*> possibilities;

public:
    // Getters
    const int getID() const { return id; }
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
    void setID(int _id) { id = _id; }
    // Calculate and set the standard deviation of the measurement while
    // setting the distance; should be used whenever a legitimate distance
    // measurement is set

	virtual const bool hasPositiveID() = 0;
};


#endif
