#include "YUVtoRGBModule.h"

rgbvalue convertPixel(byte y, byte u, byte v)
{
    byte r = y + 1.13983f * v;
    byte g = y - 0.39465f * u - 0.58060f * v;
    byte b = y + 2.03211f * u;

    rgbvalue result;
    ((byte*) &result)[3] = 0xFF;
    ((byte*) &result)[2] = r;
    ((byte*) &result)[1] = g;
    ((byte*) &result)[0] = b;
    return result;
}

YUVtoRGBModule::YUVtoRGBModule() : Module(),
                                   rgbOut(base())
{
}

void YUVtoRGBModule::run_()
{
    yuvIn.latch();
    portals::Message<RGBImage> outMsg(convert(yuvIn.message()));
    rgbOut.setMessage(outMsg);
}

RGBImage* YUVtoRGBModule::convert(messages::YUVImage in)
{
    messages::MemoryImage8 y = in.yImage();
    messages::MemoryImage8 u = in.uImage();
    messages::MemoryImage8 v = in.vImage();

    RGBImage* out = new RGBImage(yuvIn.message().width()/2,
                                yuvIn.message().height());

    for(int i = 0; i < in.width(); i++)
    {
        for (int j = 0; j < in.height(); j++)
        {
            out->putPixel(i, j, convertPixel(y.getPixel(i, j),
                                             u.getPixel(i/2, j),
                                             v.getPixel(i/2, j)));
        }
    }

    return out;
}
