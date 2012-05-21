/**
 * Basic implementation of a particle filter for 
 * robot localization purposes. 
 *
 * @todo Future work includes adding an 
 *       Augmented MCL algorithm implementation
 *       to better address issues of robot
 *       kidnapping. 
 *
 * @author Ellis Ratner <eratner@bowdoin.edu>
 */
#ifndef PARTICLE_FILTER_H
#define PARTICLE_FILTER_H

#include <vector>
#include <ctime>
#include <cmath>
#include <iostream>
#include <map>
#include <boost/shared_ptr.hpp>
#include <boost/random.hpp>
#include <boost/random/normal_distribution.hpp>
#include <boost/random/uniform_real.hpp>
#include <boost/math/constants/constants.hpp>

#include "LocSystem.h"

namespace PF
{
    /**
     * Contains a complete field location (x, y, angle).
     * Note that the angle is defined as increasing in the 
     * counter-clockwise direction, and is 0 and 2*PI
     * at 12 o'clock (parallel to the positive y axis.)
     */
    struct Location
    {
	Location(float X = 0.0f, float Y = 0.0f, float H = 0.0f)
	: x(X), y(Y), heading(H)
	{ }

	float x;
	float y;
	float heading;

	friend std::ostream& operator<<(std::ostream& out, Location l)
	{
	    out << "location (" << l.x << ", "
		<< l.y << ", " << l.heading << ")" << "\n";
	    return out;
	}
    };

    /**
     * Contains a two-dimensional spatial vector defined
     * by a magnitude and direction (position vector).
     */
    struct Vector2D
    {
	Vector2D(float mag = 0.0f, float dir = 0.0f)
	: magnitude(mag), direction(dir)
	{ }

	float magnitude;
	float direction;
    };

    struct ParticleFilterParams
    {
	int fieldHeight;                // Field height. 
	int fieldWidth;                 // Field width.
	int numParticles;               // Size of particle population.
	float sigma_d;                  // Variance for distance sampling.
	float sigma_h;                  // Variance for heading sampling.
	float alpha_fast;               // Weight factor for fast exponential weight filter.
	float alpha_slow;               // Weight factor for slow exponential weight filter.
    };

    static const ParticleFilterParams DEFAULT_PARAMS = 
    {
	FIELD_WHITE_HEIGHT,
	FIELD_WHITE_WIDTH,
	125,
	15.00f,
	1.40f,
	0.2f,
	0.05f
    };

    class MotionModel;
    class SensorModel;

    /**
     * Defines a localization particle with a weight
     * (not normalized), and a complete location, which
     * represents a single localization hypothesis.
     */
    class LocalizationParticle
    { 
    public:
	LocalizationParticle(Location l, float w);
	LocalizationParticle();
	
	~LocalizationParticle() { }

	Location getLocation() const { return location; }
	void setLocation(Location nl) { location = nl; }

	float getWeight() const { return weight; }
	void setWeight(float nw) { weight = nw; }

	void setX(float x) { location.x = x; }
	void setY(float y) { location.y = y; }
	void setH(float h) { location.heading = h; }

	friend bool operator <(const LocalizationParticle& first, 
			       const LocalizationParticle& second);

	friend std::ostream& operator<<(std::ostream& out, LocalizationParticle p)
	{
	    out << "Particle with weight " << p.getWeight() << " with "
		<< p.getLocation() << std::endl;
	    return out;
	}

    private:
	float weight;
	Location location;
    };

    typedef std::vector<LocalizationParticle> ParticleSet;
    typedef ParticleSet::iterator ParticleIt;

    /**
     * The main particle filter localization class. Handles
     * functionality for constructing a posterior belief
     * based on a prior belief function as well as latest
     * sensor and control data.
     */
    class ParticleFilter : public LocSystem
    {
    public:
        ParticleFilter(int particles, float w, float h,
		       boost::shared_ptr<MotionModel> motion, 
		       boost::shared_ptr<SensorModel> sensor);
	~ParticleFilter();

	void run(bool motionUpdate = true, bool sensorUpdate = true);

	ParticleSet getParticles() const { return particles; }

	LocalizationParticle getBestParticle();


	/**
	 * Methods inherited from LocSystem. For now, they don't really
	 * do anything, since LocSystem will change soon.
	 */
	void updateLocalization(const ::MotionModel& u_t,
				const std::vector<PointObservation>& pt_z,
				const std::vector<CornerObservation>& c_z)
	{
	    // Do nothing with this data.
	    run();
	    /* std::cout << "Best particle " */
	    /*           << this->getBestParticle(); */
	}

	/**
	 * @todo implement a reset method for the particle filter.
	 */
	void reset();

	void blueGoalieReset() { }
	void redGoalieReset() { }
	void resetLocTo(float x, float y, float h);

	PoseEst getCurrentEstimate() const { return PoseEst(); }
	PoseEst getCurrentUncertainty() const { return PoseEst(); }
	float getXEst() const { return xEstimate; }
	float getYEst() const { return yEstimate; }
	float getHEst() const { return hEstimate; }
	float getHEstDeg() const { return 0.0f; }
	float getXUncert() const { return 0.0f; }
	float getYUncert() const { return 0.0f; }
	float getHUncert() const { return 0.0f; }
	float getHUncertDeg() const { return 0.0f; }
	::MotionModel getLastOdo() const { return ::MotionModel(); }

	std::vector<PointObservation> getLastPointObservations() const { return std::vector<PointObservation>(); }

	
	std::vector<CornerObservation> getLastCornerObservations() const { return std::vector<CornerObservation>(); }

	bool isActive() const { return true; }
	
        void setXEst(float xEst) { }
	void setYEst(float yEst) { }
	void setHEst(float hEst) { }
	void setXUncert(float uncertX) { }
	void setYUncert(float uncertY) { }
	void setHUncert(float uncertH) { }
	void activate() { }
	void deactivate() { }

    private:
	void resample();

	int numParticles;

	// Spatial dimensions
	float width;
	float height;

	float xEstimate;
	float yEstimate;
	float hEstimate;

	float averageWeight;
	float wFast;
	float wSlow;

	ParticleSet particles;
	boost::shared_ptr<MotionModel> motionModel;
	boost::shared_ptr<SensorModel> sensorModel;
    };

    /**
     * The abstract interface for providing motion updates
     * to the particle filter. Must implement a control 
     * update method.
     */
    class MotionModel
    {
    public:
	MotionModel() { }
	virtual ~MotionModel() { }

	virtual ParticleSet update(ParticleSet particles) = 0;
    };

    /**
     * The abstract interface for providing sensor (i.e., vision)
     * updates to the particle filter.
     */
    class SensorModel
    {
    public:
	SensorModel() { }
	virtual ~SensorModel() { }

	virtual ParticleSet update(ParticleSet particles) = 0;

	/**
	 * These methods allow the client to access information as
	 * to whether or not the SensorModel has performed an 
	 * update on the latest iteration.
	 */
	bool hasUpdated() const { return updated; }
	void setUpdated(bool u) { updated = u; }

    private:
	bool updated;
    };

    /**
     * Samples a Gaussian normal distribution of specified
     * mean and standard deviation (sigma.)
     * @param mean the mean of the data.
     * @param sigma the standard deviation of the data.
     * @return A random sample of the specified normal 
     *         distribution.
     */
    static float sampleNormal(float mean, float sigma)
    {
	// Seed the random number generator.
        static boost::mt19937 rng(static_cast<unsigned>(std::time(0)));

	boost::normal_distribution<float> dist(mean, sigma);

	boost::variate_generator<boost::mt19937&, 
	    boost::normal_distribution<float> > sample(rng, dist);

	return sample();
    }

    /**
     * Finds the position vector to the point (x, y) in the 
     * specified coordinate frame.
     * @param origin the specified origin of the coordinate
     *               frame.
     * @param x x-coordinate.
     * @param y y-coordinate.
     * @return the position vector from the origin to the point (x', y').
     */
    static Vector2D getPosition(Location origin, float x, float y)
    {
	float x0 = origin.x;
	float y0 = origin.y;
	float rotate = NBMath::subPIAngle(origin.heading);

	// Translate and rotate.
	float x_prime = (x - x0) * std::cos(rotate) + (y - y0) * std::sin(rotate);
	float y_prime = (y - y0) * std::cos(rotate) + (x - x0) * std::sin(rotate);

	return Vector2D(std::sqrt(x_prime*x_prime + y_prime*y_prime),
			std::atan2(y_prime, x_prime));
    }
}

#endif // PARTICLE_FILTER_H
