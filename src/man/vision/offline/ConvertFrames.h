#ifndef _ConvertFrames_h_DEFINED
#define _ConvertFrames_h_DEFINED

#include <vector>
#include <stdint.h>

typedef struct _frame_struct {
    int version;
    std::vector<float> vba;
    std::vector<float> sensors;
    uint8_t * image;
} Frame;

void loadFrame(std::string path, Frame& frame);

void convertFrame(Frame& f);
void saveFrame(std::string path, Frame& f);


#endif /* _ConvertFrames_h_DEFINED */
