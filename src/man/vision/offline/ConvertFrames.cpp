#include <iostream>
#include <fstream>
#include <stdlib.h>
#include <stdio.h>

#include "ConvertFrames.h"
#include "VisionDef.h"
#include "SensorDef.h"
#include "Kinematics.h"

using namespace std;

/**
 * A tool to convert our old 320x240 images into shiny new
 * 640x480 images so we can run our whole codebase on them.
 *
 */

int main(int argc, char *argv[])
{
    if (argc < 2){
        printf("Requires file name(s).");
        exit(1);
    }

    cout << argc << endl;

    for (int i=1; i < argc; ++i){
        string path(argv[i]);

        Frame f;
        loadFrame(path, f);

        uint8_t *newImg = new uint8_t[640*480*2];

        convertFrame(f.image, newImg);

        delete f.image;
        f.image = newImg;

        path += ".NEW";
        saveFrame(path, f);
    }
}

void loadFrame(string path, Frame& frame)
{
    fstream fin(path.c_str() , fstream::in);
    if (fin.fail()){
        cout << "Frame load failed: " << path << endl;
        return ;
    }

    frame.image = new uint8_t[320 * 240 * 2];

    // Load the image from the file, puts it straight into Sensors'
    // image buffer so it doesn't have to allocate its own buffer and
    // worry about deleting it
    fin.read((char*)frame.image, 320 * 240 * 2);


    fin >> frame.version;

    vector<float> vba;
    float v;

    // Read in the body angles
    for (unsigned int i = 0; i < Kinematics::NUM_JOINTS; ++i) {
        fin >> v;
        vba.push_back( v );
    }
    frame.vba = vba;

    // Read sensor values
    vector<float> sensor_data;
    for (int i = 0; i < NUM_SENSORS; ++i) {
        fin >> v;
        sensor_data.push_back( v );
    }
    frame.sensors = sensor_data;

    fin.close();
}


/**
 * Convert
 * |y1|u|y2|v|
 *
 * into
 *
 * |y1|u|y1|v|y2|u|y2|v|
 * |y1|u|y1|v|y2|u|y2|v|
 *
 * So that the averaging done by _acquire_image ends up with the
 * original 320x240 image.
 */
void convertFrame(uint8_t *old, uint8_t *newImg)
{
    for (int i = 0; i < 240; ++i) {
        for (int j = 0; j < 320 * 2; j += 4) { // Each row is 320 * 2 bytes long
            int oldIndex = i * 320 * 2 + j;

            uint8_t y1 = old[oldIndex + YOFFSET1];
            uint8_t y2 = old[oldIndex + YOFFSET2];
            uint8_t u = old[oldIndex + UOFFSET];
            uint8_t v = old[oldIndex + VOFFSET];

            // We write out 4 new pixels for each old so we need to
            // double the rows /and/ cols of the old index
            int newIndex = i * 2 * 640 * 2 + j * 2;

            // Copy to first row
            newImg[newIndex + YOFFSET1] = y1;
            newImg[newIndex + YOFFSET2] = y1;
            newImg[newIndex + UOFFSET] = u;
            newImg[newIndex + VOFFSET] = v;

            // Second set of YUV pixels in first row
            newIndex += 4;
            newImg[newIndex + YOFFSET1] = y2;
            newImg[newIndex + YOFFSET2] = y2;
            newImg[newIndex + UOFFSET] = u;
            newImg[newIndex + VOFFSET] = v;

            // Next row, first set of pixels
            newIndex -= 4;          // Reset to first pixel
            newIndex += 640 * 2;        // Next row

            // Y1
            newImg[newIndex + YOFFSET1] = y1;
            newImg[newIndex + YOFFSET2] = y1;
            newImg[newIndex + UOFFSET] = u;
            newImg[newIndex + VOFFSET] = v;

            // Second set of YUV pixels in first row
            newIndex += 4;
            newImg[newIndex + YOFFSET1] = y2;
            newImg[newIndex + YOFFSET2] = y2;
            newImg[newIndex + UOFFSET] = u;
            newImg[newIndex + VOFFSET] = v;
        }
    }
}

void saveFrame(string path, Frame& f)
{
    fstream fout(path.c_str(), fstream::out);

    fout.write(reinterpret_cast<const char*>(f.image),
               640 * 480 * 2);

    fout << f.version << " ";

    // Write joints
    for (vector<float>::const_iterator i = f.vba.begin();
         i < f.vba.end(); i++) {
        fout << *i << " ";
    }

    // Write sensors
    for (vector<float>::const_iterator i = f.sensors.begin();
         i != f.sensors.end(); i++) {
        fout << *i << " ";
    }
    fout.close();
}
