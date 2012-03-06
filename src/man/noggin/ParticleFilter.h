/**
 * Basic implementation of a particle filter for 
 * robot localization purposes. Based on concepts
 * outlined in Probabilistic Robotics
 * [Thrun, Burgard and Fox 2005].
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
	Location(float X = 0.0f, float Y = 0.0f, float A = 0.0f)
	: x(X), y(Y), angle(A)
	{ }

	float x;
	float y;
	float angle;

	friend std::ostream& operator<<(std::ostream& out, Location l)
	{
	    out << "location (" << l.x << ", "
		<< l.y << ", " << l.angle << ")" << "\n";
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
	void setA(float a) { location.angle = a; }

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
		       MotionModel* motion, SensorModel* sensor);
	~ParticleFilter();

	void run(bool motionUpdate = true, bool sensorUpdate = true);

	ParticleSet getParticles() const { return particles; }

	LocalizationParticle getBestParticle();


    private:
	void resample();

	// Spatial dimensions
	float width;
	float height;

	int numParticles;
	ParticleSet particles;
	MotionModel* motionModel;
	SensorModel* sensorModel;
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
	float rotate = origin.angle;

	// Translate and rotate.
	float x_prime = (x - x0) * std::cos(rotate) + (y - y0) * std::sin(rotate);
	float y_prime = (y - y0) * std::cos(rotate) + (x - x0) * std::sin(rotate);

	return Vector2D(std::sqrt(x_prime*x_prime + y_prime*y_prime),
			std::atan2(y_prime, x_prime));
    }
}

#endif // PARTICLE_FILTER_H
