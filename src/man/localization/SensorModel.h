/**
 * @brief  Defines an abstract sensor model interface, as well
 *         as an inheriting vision model, because in our case 
 *         localization makes use of visual measurements.
 * @author Ellis Ratner <eratner@bowdoin.edu>
 * @date   January 2013
 */
#pragma once

#include "Particle.h"

namespace man
{
    namespace localization
    {
	/**
	 * @class SensorModel
	 * @brief The abstract interface for providing sensor 
	 *        (e.g., vision) updates to the particle filter.
	 */
	class SensorModel
	{
	public:
	    SensorModel() { }
	    virtual ~SensorModel() { }

	    /**
	     * @brief Update the particles with the latest measurement
	     *        taken by the robot's sensors. 
	     */
	    virtual ParticleSet update(ParticleSet& particles) = 0;

	    /*
	     * These methods allow the client to access information as
	     * to whether or not the SensorModel has performed an
	     * update on the latest iteration.
	     */

	    
	    bool hasUpdated() const { return updated_; }
	    
	    void setUpdated(bool updated) { updated_ = updated; }

	private:
	    bool updated_;    //! Flag indicates whether or not the particles have 
	                     //! been updated with the latest sensor readings. 
	};

	
    } // namespace localization
} // namespace man
