//******************************************************************************************
//  File: PS_TrafficLight.cpp
//  Authors: 
//
//  Summary:  PS_TrafficLight is a class which implements a custom Level device capability.
//			  It inherits from the st::PollingSensor class.  
//
//
//			  Create an instance of this class in your sketch's global variable section
//			  For Example:  st::PS_TrafficLight sensor1("trafficlight1", 60, 0, PIN_ULTRASONIC_T, PIN_ULTRASONIC_E, "rgbSwitch");
//
//			  st::PS_TrafficLight() constructor requires the following arguments
//				- String &name - REQUIRED - the name of the object - must match the Groovy ST_Anything DeviceType tile name
//				- long interval - REQUIRED - the polling interval in seconds
//				- long offset - REQUIRED - the polling interval offset in seconds - used to prevent all polling sensors from executing at the same time
//				- byte digitalTriggerPin - REQUIRED - the Arduino Pin to be used as a digital output to trigger ultrasonic
//				- byte digitalEchoPin - REQUIRED - the Arduino Pin to be used as a digital input to read the echo
//				- String rgbSwitch - REQUIRED - determines which rgb device to send light output to
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

#include "PS_TrafficLight.h"

#include "Constants.h"
#include "Everything.h"



namespace st
{
//private
	

//public
	//constructor - called in your sketch's global variable declaration section
	PS_TrafficLight::PS_TrafficLight(const __FlashStringHelper *name, unsigned int interval, int offset, byte digitalTriggerPin, byte digitalEchoPin, String rgbSwitch):
		PollingSensor(name, interval, offset),
		m_nSensorValue(0),
		m_nRgbSwitch(rgbSwitch)
	{
		stInterval = interval;
		setPin(digitalTriggerPin,digitalEchoPin);
		//retrieve the settings from the app
		Everything::sendSmartString(getName() + " " + "get_inputs");

	}
	
	//destructor
	PS_TrafficLight::~PS_TrafficLight()
	{
		
	}

	//SmartThings Shield data handler (receives configuration data from ST - polling interval, and adjusts on the fly)
	void PS_TrafficLight::beSmart(const String &str)
	{
		String s = str.substring(str.indexOf(' ') + 1);
		String firstWord = s.substring(0, s.indexOf(' '));
		Serial.print("st string #####  ");
		Serial.println(str);
		//Serial.println(firstWord);
		
		//ST sent input values
		//use these values to make adjustments to distance values and colors
		if (firstWord == "inputs")
		{
			parseSettings(s.substring(s.indexOf(' ') + 1));
			st::PollingSensor::setInterval(localPoll.toInt());
			Serial.print(F("PS_TrafficLight::beSmart "));
			Serial.println(s);
		}
		//all other received messages from ST hub that start with a number
		//should only be the ST polling interval
		else if (s.toInt() != 0)
		{
				//changing this in the local variable instead of updating the polling sensor
				//st::PollingSensor::setInterval(s.toInt() * 1000);
				stInterval = s.toInt();
				if (st::PollingSensor::debug)
				{
					Serial.print(F("PS_TrafficLight::beSmart set polling interval to "));
					Serial.println(s.toInt());
				}
		}
		else
		{
			if (st::PollingSensor::debug)
			{
				Serial.print(F("PS_TrafficLight::beSmart cannot convert "));
				Serial.print(s);
				Serial.println(F(" to an Integer."));
			}
		}
	}

	//function to get data from sensor
	void PS_TrafficLight::readSensor()
	{
			//int m_nSensorValue=map(analogRead(m_nAnalogInputPin), SENSOR_LOW, SENSOR_HIGH, MAPPED_LOW, MAPPED_HIGH);
			long duration;
			// Clears the trigPin
			digitalWrite(m_nDigitalTriggerPin, LOW);
			delayMicroseconds(2);
			// Sets the trigPin on HIGH state for 10 micro seconds
			digitalWrite(m_nDigitalTriggerPin, HIGH);
			delayMicroseconds(10);
			digitalWrite(m_nDigitalTriggerPin, LOW);
			// Reads the echoPin, returns the sound wave travel time in microseconds
			duration = pulseIn(m_nDigitalEchoPin, HIGH);
			// Calculating the distance
			m_nSensorValue = duration*0.034/2;
			Serial.println("PS_TrafficLight: sensor value " + String(m_nSensorValue));
        }

	//called by ST::Everything:init as part of the main loop
	//calls the function to get data from sensor 
	//periodically sends results to the ST Cloud 
	void PS_TrafficLight::getData()
	{
		readSensor();
		stoplight();
		currentMillis = millis();
		//the frequency that this function is being called is localPoll instead of interval
		//limit the push to ST
		if (currentMillis - previousMillis >= stInterval * 1000)
		{
			previousMillis = currentMillis;
			// queue the distance to send to smartthings 
			Everything::sendSmartString(getName() + " " + String(m_nSensorValue));
		}
	}
	
	//parse settings coming in from the app
	void PS_TrafficLight::parseSettings(String settingsString)
	{
		while(settingsString.length() >= 1 && settingsString[0] != ' ')
		{
			//Serial.println("settingsString breakdown: " + settingsString);
			String s = settingsString + ' ';
			String myPair = s.substring(0, s.indexOf(' '));
			String myKey = s.substring(0, s.indexOf(':'));
			String myValue = s.substring(s.indexOf(':') + 1, s.indexOf(' '));
			if (myKey == "local_poll") localPoll = myValue;
			if (myKey == "proceed_distance") proceedDistance = myValue;
			if (myKey == "warning_distance") warningDistance = myValue;
			if (myKey == "stop_distance") stopDistance = myValue;
			if (myKey == "too_far_distance") tooFarDistance = myValue;
			settingsString = s.substring(s.indexOf(' ') + 1);
		}
	}


	//blink the RGB lights like a traffic light
	void PS_TrafficLight::stoplight()
	{
		//blinking red when too far
		if (m_nSensorValue <= tooFarDistance.toInt())
		{
			Serial.println("Distance: Too far");
			if (previousPIN == "off")
			{
				st::receiveSmartString(m_nRgbSwitch + " ff0000");
				previousPIN = "on";
			}
			else
			{
				st::receiveSmartString(m_nRgbSwitch + " 000000");
				previousPIN = "off";
			}
		}
		//red indicates vehicle should stop
		else if (m_nSensorValue <= stopDistance.toInt())
		{
			Serial.println("Distance: Stop");
			st::receiveSmartString(m_nRgbSwitch + " ff0000");
		}
		//yellow is approaching stop
		else if (m_nSensorValue <= warningDistance.toInt())
		{
			Serial.println("Distance: Warning");
			st::receiveSmartString(m_nRgbSwitch + " ffff00");
		}
		//green indicates vehicle should proceed
		else if (m_nSensorValue <= proceedDistance.toInt())
		{
			Serial.println("Distance: Proceed");
			st::receiveSmartString(m_nRgbSwitch + " 00ff00");
		}
		//no light until within proceed distance
		else
		{
			Serial.println("Distance: Waiting");
			st::receiveSmartString(m_nRgbSwitch + " 000000");
		}

	}

	void PS_TrafficLight::setPin(byte &trigPin,byte &echoPin)
	{
		m_nDigitalTriggerPin=trigPin;
		m_nDigitalEchoPin=echoPin;
		pinMode(m_nDigitalTriggerPin, OUTPUT); // Sets the trigPin as an Output
		pinMode(m_nDigitalEchoPin, INPUT); // Sets the echoPin as an Input
	}
}
