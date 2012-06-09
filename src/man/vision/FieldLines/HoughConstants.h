#ifndef _HOUGHCONSTANTS_H_
#define _HOUGHCONSTANTS_H_

namespace HoughConstants
{

enum
{
    // Hough Space size parameters
    // 256 for full 8 bit angle, radius is for 320x240 image
    r_span = 320,
    t_span = 256,

    // 5 rows on either side of the hough space to account for
    // angle wrap around in edge marking.
    hs_t_dim = t_span+10,
#ifdef USE_MMX
    first_smoothing_row = 3,
#else
    first_smoothing_row = 0,
#endif
    first_peak_row = first_smoothing_row + 1,

    default_accept_thresh = 43,
    default_angle_spread  = 5,
    peak_points = 4,
    hough_max_peaks = r_span * t_span / 4,
    active_line_buffer = 200,
    opp_line_thresh = 5,
    suppress_r_bound = 4
};
}

#endif /* _HOUGHCONSTANTS_H_ */
