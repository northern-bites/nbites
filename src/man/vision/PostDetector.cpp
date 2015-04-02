#include "PostDetector.h"

#include <cmath>
#include <iostream>

namespace man {
namespace vision {

PostDetector::PostDetector(const messages::PackedImage16& yImage, 
                           const messages::PackedImage8& whiteImage)
    : wd(whiteImage.width()-2),
      postImage(wd, whiteImage.height()-2),
      candidates()
{
    unfilteredHistogram = new double[wd];
    filteredHistogram = new double[wd];

    buildPostImage(yImage, whiteImage);
    buildHistogram();
    filterHistogram();
    findPeaks();
}

PostDetector::~PostDetector()
{
    delete[] unfilteredHistogram;
    delete[] filteredHistogram;
}

void PostDetector::buildPostImage(const messages::PackedImage16& yImage, 
                                  const messages::PackedImage8& whiteImage)
{
    int ht = postImage.height();

    for (int y = 0; y < ht; y++) {
        uint16_t* yRow = yImage.pixelAddress(1, y+1);
        unsigned char* whiteRow = whiteImage.pixelAddress(1, y+1);
        double* postRow = postImage.pixelAddress(0, y);
        for (int x = 0; x < wd; x++) {
            Fool gradScore(calculateGradScore(yRow, yImage.rowPitch(), yImage.pixelPitch()));
            Fool whiteScore(static_cast<double>(*whiteRow) / 255);
            *postRow = (gradScore & whiteScore).get();

            yRow += yImage.pixelPitch();
            whiteRow += whiteImage.pixelPitch(); 
            postRow += postImage.pixelPitch();
        }
    }
}

Fool PostDetector::calculateGradScore(uint16_t* pt, int rowPitch, int pixelPitch) {
    double hle = static_cast<double>(*(pt-rowPitch-pixelPitch));
    double h = static_cast<double>(*(pt-rowPitch));
    double hr = static_cast<double>(*(pt-rowPitch+pixelPitch)); 
    double le = static_cast<double>(*(pt-pixelPitch)); 
    double m = static_cast<double>(*(pt));
    double r = static_cast<double>(*(pt+pixelPitch)); 
    double lle = static_cast<double>(*(pt+rowPitch-pixelPitch)); 
    double l = static_cast<double>(*(pt+rowPitch)); 
    double lr = static_cast<double>(*(pt+rowPitch+pixelPitch));

    double gradX = -1*hle + 1*hr - 2*le + 2*r - 1*lle + 1*lr;
	double gradY = -1*hle - 2*h - 1*hr + 1*lle + 2*l + 1*lr;

    double magn = sqrt(gradX*gradX + gradY*gradY);

    double angle = 0;
    if (!(gradX == 0 && gradY == 0)) {
        if (gradX == 0) {
            angle = (gradY > 0 ? 90.0 : -90.0);
        } else {
            angle = (atan(fabs(gradY / gradX))) * 180 / M_PI;
            if (gradX >= 0 && gradY >= 0)
                angle = angle;
            else if (gradX <= 0 && gradY <= 0)
                angle = angle + 180;
            else if (gradX <= 0)
                angle = 180 - angle;
            else
                angle = -1*angle;
        }
    }
    
    FuzzyThreshold sigmaMagnitudeLow(3, 7);
    FuzzyThreshold sigmaMagnitudeHigh(60, 80);
	FuzzyThreshold sigmaDirection(-60, -35);
		
    if (angle > 90) 
        angle -= 180;
    if (angle > 0) 
        angle *= -1;
    
    Fool magnLowScore(magn > sigmaMagnitudeLow);
    Fool magnHighScore(magn < sigmaMagnitudeHigh);
    Fool dirScore(angle > sigmaDirection);
    
    return magnLowScore & magnHighScore & dirScore;
}

void PostDetector::buildHistogram()
{
    int ht = postImage.height();

    for (int y = 0; y < ht; y++) {
        double* row = postImage.pixelAddress(0, y);
        for (int x = 0; x < wd; x++, row += postImage.pixelPitch()) {
            unfilteredHistogram[x] += *row;
        }
    }
}

void PostDetector::filterHistogram()
{
    double narrow[43];
    double wide[43];
    double diffOfGaussian[43];
    
    narrow[0] = 0;
    narrow[1] = 0;	
    narrow[2] = 0;	
    narrow[3] = 0;	
    narrow[4] = 0;	
    narrow[5] = 0;	
    narrow[6] = 0.000001;	
    narrow[7] = 0.000003;	
    narrow[8] = 0.000012;	
    narrow[9] = 0.000048;	
    narrow[10] = 0.000169;	
    narrow[11] = 0.000538;	
    narrow[12] = 0.001532;	
    narrow[13] = 0.003906;	
    narrow[14] = 0.00892;	
    narrow[15] = 0.018246;	
    narrow[16] = 0.033431;	
    narrow[17] = 0.054865;	
    narrow[18] = 0.080656;	
    narrow[19] = 0.106209;	
    narrow[20] = 0.125279;	
    narrow[21] = 0.132368;	
    narrow[22] = 0.125279;	
    narrow[23] = 0.106209;	
    narrow[24] = 0.080656;	
    narrow[25] = 0.054865;	
    narrow[26] = 0.033431;	
    narrow[27] = 0.018246;	
    narrow[28] = 0.00892;	
    narrow[29] = 0.003906;	
    narrow[30] = 0.001532;	
    narrow[31] = 0.000538;	
    narrow[32] = 0.000169;	
    narrow[33] = 0.000048;	
    narrow[34] = 0.000012;	
    narrow[35] = 0.000003;	
    narrow[36] = 0.000001;	
    narrow[37] = 0;	
    narrow[38] = 0;	
    narrow[39] = 0;	
    narrow[40] = 0;	
    narrow[41] = 0;	
    narrow[42] = 0;

    wide[0] = 0.016601;
    wide[1] = 0.017391;	
    wide[2] = 0.018177;	
    wide[3] = 0.018956;	
    wide[4] = 0.019723;	
    wide[5] = 0.020475;	
    wide[6] = 0.021207;	
    wide[7] = 0.021916;	
    wide[8] = 0.022597;	
    wide[9] = 0.023246;	
    wide[10] = 0.02386;	
    wide[11] = 0.024435;	
    wide[12] = 0.024967;	
    wide[13] = 0.025453;	
    wide[14] = 0.025889;	
    wide[15] = 0.026274;	
    wide[16] = 0.026603;	
    wide[17] = 0.026876;	
    wide[18] = 0.02709;	
    wide[19] = 0.027244;	
    wide[20] = 0.027337;	
    wide[21] = 0.027368;	
    wide[22] = 0.027337;	
    wide[23] = 0.027244;	
    wide[24] = 0.02709;	
    wide[25] = 0.026876;	
    wide[26] = 0.026603;	
    wide[27] = 0.026274;	
    wide[28] = 0.025889;	
    wide[29] = 0.025453;	
    wide[30] = 0.024967;	
    wide[31] = 0.024435;	
    wide[32] = 0.02386;	
    wide[33] = 0.023246;	
    wide[34] = 0.022597;	
    wide[35] = 0.021916;	
    wide[36] = 0.021207;	
    wide[37] = 0.020475;	
    wide[38] = 0.019723;	
    wide[39] = 0.018956;	
    wide[40] = 0.018177;	
    wide[41] = 0.017391;	
    wide[42] = 0.016601;

    for (int i = 0; i < 43; i++)
        diffOfGaussian[i] = narrow[i] - wide[i];

    convolve(unfilteredHistogram, diffOfGaussian, 43, filteredHistogram);
}

void PostDetector::convolve(double const* in, double const* kernel, 
                            int klength, double* out)
{
    int halfklength = klength / 2;
    for (int i = halfklength; i < wd - halfklength; i++, out++) {
        int offset = i - halfklength;
        for (int j = 0; j < klength; j++) {
            *out += in[offset+j]*kernel[j];
        }
    }
}

void PostDetector::findPeaks()
{
    int leftLimit = 0;
    bool inPeak = false;
    for (int i = 0; i < wd; i++) {
        if (filteredHistogram[i] >= PostDetector::peakThreshold && !inPeak) {
            leftLimit = i;
            inPeak = true;
        } else if (filteredHistogram[i] < PostDetector::peakThreshold && inPeak) {
            candidates.push_back((leftLimit + i) / 2);
            inPeak = false;
        }
    }
}

}
}
