/*!
 * @file leap.h
 * @brief Leap Motion connection and frame polling helper.
 */
#pragma once

#ifdef LMGL_ENABLE_LEAP_MOTION

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>   

#include <iostream>
#include <LeapC.h>

namespace lmgl{
namespace vr {

/*!
 * @brief Thin wrapper around the LeapC API used by LMGL demos.
 *
 * The class owns the Leap connection lifecycle and exposes the latest
 * tracking frame received from the device.
 */
class Leap {	
public:
	//! @brief Creates an uninitialized Leap wrapper.
	Leap();	 

	//! @brief Destroys the wrapper.
	~Leap();	 	      

	/*!
	 * @brief Initializes the Leap connection and validates device availability.
	 *
	 * @return True when the device connection is ready.
	 */
	bool init();

	/*!
	 * @brief Closes and destroys the Leap connection.
	 *
	 * @return True when cleanup completes.
	 */
	bool free();

	/*!
	 * @brief Polls Leap events and refreshes the cached tracking frame.
	 *
	 * @return True when polling succeeds.
	 */
	bool update();

	/*!
	 * @brief Returns the latest cached tracking frame.
	 *
	 * @return Pointer to the current Leap tracking event.
	 */
	const LEAP_TRACKING_EVENT *getCurFrame() const;
private:				
	//! @brief LeapC connection handle.
	LEAP_CONNECTION connection;

	//! @brief Reference to the connected Leap device.
	LEAP_DEVICE_REF leapDevice;

	//! @brief Most recent tracking frame.
	LEAP_TRACKING_EVENT curFrame;

	//! @brief Last processed Leap frame identifier.
	signed long long lastFrameId;
};

} // namespace vr
} // namespace lmgl

#endif
