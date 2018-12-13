//******************************************************************************************
//  File: PS_TrafficLight.cpp
//  Authors: 
//
//  Summary:  PS_TrafficLight is a class which implements a custom Level device capability.
//			  It inherits from the st::PollingSensor class.  
//
//
//			  Create an instance of this class in your sketch's global variable section
//			  For Example:  st::PS_TrafficLight sensor1("trafficlight1", 60, 0, PIN_ULTRASONIC_T, PIN_ULTRASONIC_E);
//
//			  st::PS_TrafficLight() constructor requires the following arguments
//				- String &name - REQUIRED - the name of the object - must match the Groovy ST_Anything DeviceType tile name
//				- long interval - REQUIRED - the polling interval in seconds
//				- long localInterval - REQUIRED - the polling interval for local processes in milliseconds
//				- long offset - REQUIRED - the polling interval offset in seconds - used to prevent all polling sensors from executing at the same time
//				- byte digitalTriggerPin - REQUIRED - the Arduino Pin to be used as a digital output to trigger ultrasonic
//				- byte digitalEchoPin - REQUIRED - the Arduino Pin to be used as a digital input to read the echo
//
//
//  Change History:
//
//    Date        Who            What
//    ----        ---            ----
//    2018-12-12  Seth Miller    Copied from ultrasonic sensor library
//    
//
//
//******************************************************************************************

#include "PollingSensor.h"
#include "Executor.h"

namespace st
{
	class PS_TrafficLight : public PollingSensor
	{
		private:
			byte m_nDigitalTriggerPin;
			byte m_nDigitalEchoPin;
			unsigned long currentMillis;
			unsigned long previousMillis = 100000000;
			unsigned long stInterval;
			String m_nRgbSwitch;
			String previousPIN = "off";
			void parseSettings(String settingsString);
			String localPoll = "1000";
			String proceedDistance = "20";
			String warningDistance = "15";
			String stopDistance = "10";
			String tooFarDistance = "5";
			
		public:
			float m_nSensorValue;

			//constructor - called in your sketch's global variable declaration section
			PS_TrafficLight(const __FlashStringHelper *name, unsigned int interval, int offset, byte digitalTriggerPin, byte digitalEchoPin, String rgbSwitch);
			
			//destructor
			virtual ~PS_TrafficLight();
			
			//SmartThings Shield data handler (receives configuration data from ST - polling interval, and adjusts on the fly)
			virtual void beSmart(const String &str);

			virtual void readSensor();

			//function to get data from sensor and queue results for transfer to ST Cloud 
			virtual void getData();
			
			virtual void stoplight();

			//sets
			void setPin(byte &trigPin,byte &echoPin);
	};
}
