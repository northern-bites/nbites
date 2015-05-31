#include "nbfuncs.h"

#include "RoboGrams.h"
#include "Images.h"
#include "vision/VisionModule.h"

#include <assert.h>
#include <vector>
#include <string>
#include <iostream>

using nblog::Log;
using nblog::SExpr;

int Edges_func() {
    assert(args.size() == 1);
    printf("Edges_func()\n");

    size_t length = args[0]->data().size();
    uint8_t buf[length];
    memcpy(buf, args[0]->data().data(), length);

    messages::YUVImage image(buf, 640, 480, 640);
    portals::Message<messages::YUVImage> message(&image);

    man::vision::VisionModule module = man::vision::VisionModule();
    module.topIn.setMessage(message);
    module.bottomIn.setMessage(message);
    module.run();
}
