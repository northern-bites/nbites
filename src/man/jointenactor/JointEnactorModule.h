/**
 * @brief The joint enactor module communicates synchronously with the
 *        DCM to control the angles and stiffnesses of the joint 
 *        actuators. 
 * @author Ellis Ratner <eratner@bowdoin.edu>
 * @date   January 2013
 */
#pragma once

#include <boost/shared_ptr.hpp>

#include <alcommon/albroker.h>
#include <alcommon/alproxy.h>
#include <alproxies/almemoryproxy.h>
#include <alproxies/dcmproxy.h>
#include <almemoryfastaccess/almemoryfastaccess.h>
#include <alerror/alerror.h>

#include <string>
#include <iostream>

#include "SensorTypes.h"
#include "RoboGrams.h"

namespace man
{
    namespace jointenactor
    {
	/**
	 * @class JointEnactorModule
	 */
	class JointEnactorModule : public portals::Module
	{
	public:
	    JointEnactorModule(boost::shared_ptr<AL::ALBroker> broker);

	    virtual ~JointEnactorModule();

	    void setStiffness(float stiffness = 0.0f);

	private:
	    /**
	     * @brief Gets a proxy to the DCM, does necessary initialization,
	     *        and connects synchronous callback to the DCM loop.
	     */
	    void start();

	    /**
	     * @brief Do initialization of joint and stiffness aliases
	     *        before connecting to the DCM. 
	     */
	    void initialize();

	    /**
	     * @brief Disconnects from the DCM loop. 
	     */
	    void stop();

	    void run_();

	    boost::shared_ptr<AL::ALBroker> broker_;
	    boost::shared_ptr<AL::DCMProxy> dcmProxy_;

	    AL::ALValue lastCommand_;

	};
    } // namespace jointenactor
} // namespace man
