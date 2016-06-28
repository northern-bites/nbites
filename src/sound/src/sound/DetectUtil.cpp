#include "Detect.hpp"
#include <cmath>

namespace detect {
    size_t Channel::_iteration_ = 0;

    const Channel::Range Channel::FULL_RANGE = {0, FREQUENCY_LENGTH};
    const Channel::Range Channel::WHISTLE_RANGE = {70, 160};
    const Channel::Peak Channel::NO_PEAK = Peak(-1, 0.0);

    double Channel::sum_over (float * spectrum, const Range& range ) {

        double sum = 0;
        for (int i = range.left; i < range.right; ++i) {
            sum += spectrum[i];
        }

        return sum;
    }

    double Channel::sum_over( const Range& range ) {
        double sum = 0;
        for (int i = range.left; i < range.right; ++i) {
            sum += this_spectrum[i];
        }

        return sum;
    }

    double Channel::sum_except(const detect::Channel::Range &except,
                               const detect::Channel::Range& over ) {
        NBL_ASSERT(over.contains(except))
        double sum_o = sum_over(over);
        double sum_e = sum_over(except);

        return sum_o - sum_e;
    }

    double Channel::mean_over(const Channel::Range& over) {
        return sum_over(over) / over.count();
    }

    double Channel::mean_except(const detect::Channel::Range &except,
                                const detect::Channel::Range &over) {
        NBL_ASSERT(over.contains(except))

        return (sum_over(over) - sum_over(except)) / (over.count() - except.count());
    }

    double Channel::sdev_over( const detect::Channel::Range &over ) {
        double mean = mean_over( over );
        double sdevsum = 0;

        for (int i = over.left; i < over.right; ++i) {
            double val = this_spectrum[i] - mean;
            sdevsum += val*val;
        }

        return std::sqrt(sdevsum);
    }

    double Channel::sdev_except(const detect::Channel::Range &over,
                                const detect::Channel::Range &except) {
        NBL_INFO("{%d %d} {%d %d}",
                 over.left, over.right, except.left, except.right)
        NBL_ASSERT(over.contains(except))

        double with = sum_over(over);
        double without = with - sum_over(except);

        double mean = without / (over.count() - except.count());

        double sdevsum = 0;

        for (int i = over.left; i < except.left; ++i) {
            double val = this_spectrum[i] - mean;
            sdevsum += val*val;
        }

        for (int i = except.right; i < over.right; ++i) {
            double val = this_spectrum[i] - mean;
            sdevsum += val*val;
        }

        return std::sqrt(sdevsum);
    }
}
