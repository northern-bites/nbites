/*
 * V4L2ImageTranscriber.h
 *
 *  Created on: Jun 27, 2011
 *      Author: oneamtu
 */

#ifndef V4L2IMAGETRANSCRIBER_H_
#define V4L2IMAGETRANSCRIBER_H_

#include <ImageTranscriber.h>

namespace man {
namespace corpus {

class V4L2ImageTranscriber: public ThreadedImageTranscriber {
public:
    V4L2ImageTranscriber();
    virtual ~V4L2ImageTranscriber();
};

} /* namespace corpus */
} /* namespace man */
#endif /* V4L2IMAGETRANSCRIBER_H_ */
