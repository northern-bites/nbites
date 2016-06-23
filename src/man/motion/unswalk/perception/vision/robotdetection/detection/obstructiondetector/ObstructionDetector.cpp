#include "ObstructionDetector.hpp"



/**
 * HELPERS
 */
static int saliencyWidth(const Fovea &saliency) {
    return saliency.bb.width();
}

static int saliencyHeight(const Fovea &saliency) {
    return saliency.bb.height();
}


/**
 * Looks at a certain number of pixels below the field edge accumulating
 * the number of green pixels seen. It calculates a score based on the colour of
 * pixels there. e.g. if white or jersey colours are seen it goes up, however it
 * goes down if it sees field, goal or background colours.
 *
 * If there is a line of obscures, e.g. more than minObstructionWidth then it
 * considers it an obscure. This is to reduce single columns being considered an
 * obstruction.
 */
std::vector<Obstruction> ObstructionDetector::complete(VisionFrame &frame,
        const Fovea &saliency) {

    std::vector<Obstruction> fieldEdgeObscures;


    fieldEdges = getFieldEdges(frame, saliency);


    const static int BACKGROUND_SCORE = 3;
    const static int ROBOT_COLOUR_SCORE = 3;
    const static int UNCLASSIFIED_SCORE = 1;
    const static int FIELD_COLOUR_PENALTY = -3;
    const static unsigned int MIN_OBSTRUCTION_WIDTH = 3;
    const static unsigned int FIELD_EDGE_MIN_OBSTRUCTION_HEIGHT = 10;


    Point start(-1, -1);
    bool hasStart = false;
    unsigned int obstructionCount = 0;

    for (int saliency_col = 0; saliency_col < saliencyWidth(saliency); ++saliency_col) {
        const int saliency_row = fieldEdges[saliency_col];

        //Count number of green pixels below field edge.
        int confidenceOfRobot = 0;
        for (unsigned int i = 0; i < FIELD_EDGE_MIN_OBSTRUCTION_HEIGHT; ++i) {
            int row = saliency_row + i;

            //In case it reaches the end of the image
            if (row >= saliencyHeight(saliency)) {
                break;
            }

            Colour c = saliency.colour(saliency_col, row);
            if (c == cFIELD_GREEN || c == cGOAL_YELLOW) {
                confidenceOfRobot += FIELD_COLOUR_PENALTY;
            } else if (c == cWHITE || c == cROBOT_BLUE || c == cROBOT_RED) {
                confidenceOfRobot += ROBOT_COLOUR_SCORE;
            } else if (c == cUNCLASSIFIED) {
                confidenceOfRobot += UNCLASSIFIED_SCORE;
            } else if (c == cBACKGROUND) {
                confidenceOfRobot += BACKGROUND_SCORE;
            }
        }

        bool lastColumn = (saliency_col + 1 == saliencyWidth(saliency));

        //If it is a field point.
        if (confidenceOfRobot <= 0 || lastColumn) {
            if (hasStart) {
                if (obstructionCount >= MIN_OBSTRUCTION_WIDTH) {
                    Obstruction o;
                    o.start = start;
                    o.end = Point(saliency_col, saliency_row);

                    fieldEdgeObscures.push_back(o);
                }

                hasStart = false;
                obstructionCount = 0;
            }

        //If it is an obstruction
        } else {
            if (!hasStart) {
                start = Point(saliency_col, saliency_row);
                hasStart = true;
            }
            ++obstructionCount;
        }
    }

    return fieldEdgeObscures;
}




/**
 * Takes the frame start scan coordinates to determine the field edge and converts
 * it to points that are valid on the saliency image.
 */
std::vector<int> ObstructionDetector::getFieldEdges(VisionFrame &frame, const Fovea &saliency) {
    std::vector<int> fieldEdges;
    fieldEdges.reserve(saliencyWidth(saliency));

    for (int saliency_col = 0; saliency_col < saliencyWidth(saliency); ++saliency_col) {
        const int image_col = saliency.mapFoveaToImage(Point(saliency_col, 0)).x();

        int image_edge_row = (saliency.top) ? frame.topStartScanCoords[image_col]
                                            : frame.botStartScanCoords[image_col];
        int saliency_edge_row = saliency.mapImageToFovea(Point(image_col, image_edge_row)).y();

        if (saliency_edge_row < 0) {
            saliency_edge_row = 0;
        } else if (saliency_edge_row > saliencyHeight(saliency)) {
            saliency_edge_row = saliencyHeight(saliency);
        }
        fieldEdges.push_back(saliency_edge_row);
    }

    return fieldEdges;
}


