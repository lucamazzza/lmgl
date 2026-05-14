#include "lmgl/vr/leap.h"

namespace lmgl{
namespace vr {
Leap::Leap() : connection{nullptr}, curFrame{nullptr}, lastFrameId{0}{}
Leap::~Leap(){}

bool Leap::init() 
{
   if (LeapCreateConnection(nullptr, &connection) != eLeapRS_Success)
   {
      std::cout << "[ERROR] Unable to connect to the Leap Motion" << std::endl;      
      return false;
   }   
   if (LeapOpenConnection(connection) != eLeapRS_Success)
   {
      std::cout << "[ERROR] Unable to open connection to the Leap Motion" << std::endl;      
      LeapDestroyConnection(connection);
      connection = nullptr;
      return false;
   }
   const uint32_t timeout = 1000;
   uint32_t computedArraySize = 0;
   for (uint32_t retry = 3; retry > 0; retry--)
   {
      LEAP_CONNECTION_MESSAGE msg;
      LeapPollConnection(connection, timeout, &msg);
      eLeapRS ret = LeapGetDeviceList(connection, nullptr, &computedArraySize);
      if (ret == eLeapRS_NotConnected)      
         continue;
      else
         if (ret == eLeapRS_Success)
            if (computedArraySize)
               break;
   }
   std::cout << "Number of available devices: " << computedArraySize << std::endl;
   if (computedArraySize != 1)
   {
      std::cout << "[ERROR] Invalid number of devices (must be 1)" << std::endl;
      LeapCloseConnection(connection);
      LeapDestroyConnection(connection);
      connection = nullptr;
      return false;     
   }     
   if (LeapGetDeviceList(connection, &leapDevice, &computedArraySize) != eLeapRS_Success)
   {
      std::cout << "[ERROR] Unable to get ref to device" << std::endl;
      LeapCloseConnection(connection);
      LeapDestroyConnection(connection);
      connection = nullptr;
      return false;     
   }
   return true;
}

bool Leap::free() 
{   
   LeapCloseConnection(connection);
   LeapDestroyConnection(connection);
   connection = nullptr;
   return true;
}

bool Leap::update()
{  
   if (!connection)
      return false;

   const uint32_t timeout = 1000;      
   LEAP_CONNECTION_MESSAGE msg = {};
   int64_t now = LeapGetNow();
   int32_t frameSkipping = 0;
   bool gotTrackingFrame = false;

   for (int i = 0; i < 10; ++i)
   {
      if (LeapPollConnection(connection, timeout, &msg) != eLeapRS_Success)
      {
         std::cout << "[ERROR] Unable to poll connection" << std::endl;
         return false;
      }
      if (msg.type != eLeapEventType_Tracking || msg.tracking_event == nullptr)
         continue;

      if (msg.tracking_event->info.timestamp >= now - 100)
      {
         gotTrackingFrame = true;
         break;
      }
      frameSkipping++;
   }
   if (!gotTrackingFrame)
      return false;

   // We are lagging badly:
   if (frameSkipping > 5)
      std::cout << "[WARNING] Lag detected (" << frameSkipping << " frames skipped)" << std::endl;
   
   // Update only when necessary:   
   if (msg.tracking_event->tracking_frame_id > lastFrameId)
   {
      lastFrameId = msg.tracking_event->tracking_frame_id;
      curFrame = *msg.tracking_event;      
   }   
   // Done:
   return true;
}

const LEAP_TRACKING_EVENT *Leap::getCurFrame() const
{
   return &curFrame;
}
} //namespace vr
} // namspace lmgl
