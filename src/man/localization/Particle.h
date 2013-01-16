/**
 * @brief  Defines a particle for localization purposes.
 * @author Ellis Ratner <eratner@bowdoin.edu>
 * @date   January 2013
 */
#pragma once

namespace man
{
    namespace localization
    {
	/**
	 * @class Particle
	 * @brief Defines a localization particle with a weight
	 *        (not normalized), and a complete location, which
	 *        represents a single localization hypothesis.
	 */
	class Particle
	{
	public:
	    Particle(memory::proto::RobotLocation location, float weight)
		: weight_(w), location_(l) { }
	
	    Particle() : weight_(0.0f), location_() {}
	    
	    ~Particle() { }

	    memory::proto::RobotLocation getLocation() const { return location_; }
	    void setLocation(memory::proto::RobotLocation location) { location_ = location; }

	    float getWeight() const { return weight_; }
	    void setWeight(float weight) { weight_ = weight; }

	    void setX(float x) { location_.setX(x); }
	    void setY(float y) { location_.setY(y); }
	    void setH(float h) { location_.setH(h); }

	    /**
	     * @brief Used to compare two particles by weight, primarily for
	     *        use with the sorting algorithm.
	     *
	     * @param first the first particle.
	     * @param second the second particle.
	     * @return weight of first < weight of second ? true : false
	     */
	    friend bool operator <(const Particle& first,
				   const Particle& second)
	    {
		float w1 = first.getWeight();
		float w2 = second.getWeight();

		return (w1 < w2) ? true : false;
	    }

	    friend std::ostream& operator<<(std::ostream& out,
					    Particle p)
	    {
		out << "Particle with weight " << p.getWeight() << " with "
		    << p.getLocation().DebugString() << std::endl;
		return out;
	    }

	private:
	    float weight_;                           //! The particle weight.
	    memory::proto::RobotLocation location_;  //! The particle location (x, y, heading).

	};

	typedef std::vector<Particle> ParticleSet;
	typedef ParticleSet::iterator ParticleIt;
    } // namespace localization
} // namespace man
