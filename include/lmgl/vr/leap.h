/*!
 * @file leap.hpp
 * @brief Leap Motion integration
 */
#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>   

#include <iostream>
#include <LeapC.h>

namespace lmgl{
namespace vr {

class Leap {	
public:
	Leap();	 
	~Leap();	 	      

	bool init();
	bool free();

	bool update();
	const LEAP_TRACKING_EVENT *getCurFrame() const;
private:				
	LEAP_CONNECTION connection;
	LEAP_DEVICE_REF leapDevice;
	LEAP_TRACKING_EVENT curFrame;
	signed long long lastFrameId;
};

} // namespace vr
} // namespace lmgl


