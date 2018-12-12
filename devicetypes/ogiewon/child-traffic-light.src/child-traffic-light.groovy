/**
 *  Child Traffic Light
 *
 *  Copyright 2018 Seth Miller
 *
 *  Licensed under the Apache License, Version 2.0 (the "License"); you may not use this file except
 *  in compliance with the License. You may obtain a copy of the License at:
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software distributed under the License is distributed
 *  on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the License
 *  for the specific language governing permissions and limitations under the License.
 *
 *  Change History:
 *
 *    Date        Who            What
 *    ----        ---            ----
 *    2018-11-22  Seth Miller    Copied from child-ultrasonic-sensor
 *   
 *
 * 
 */
metadata {
	definition (name: "Child Traffic Light", namespace: "ogiewon", author: "Seth Miller") {
		capability "Sensor"
        capability "Refresh"
        
		attribute "lastUpdated", "String"
        attribute "trafficlight", "Number"        
    }

	tiles(scale: 2) {
		multiAttributeTile(name: "trafficlight", type: "generic", width: 6, height: 4, canChangeIcon: true) {
			tileAttribute("device.trafficlight", key: "PRIMARY_CONTROL") {
				attributeState("trafficlight", label: '${currentValue} in', unit:"inches", defaultState: true, backgroundColor:"#00a0dc"
			)}
            tileAttribute ("device.cm", key: "SECONDARY_CONTROL") {
        		attributeState "power", label:'${currentValue} cm'
            }    
        }
 		valueTile("lastUpdated", "device.lastUpdated", inactiveLabel: false, decoration: "flat", width: 6, height: 2) {
    			state "default", label:'Last Updated ${currentValue}', backgroundColor:"#ffffff"
		}
    }
    
    preferences {
        input name: "local_poll", type: "number", title: "Local Polling Interval", description: "Enter time in milliseconds to update the RGB strip", required: false, defaultValue: "1000"
        input name: "proceed_distance", type: "number", title: "Proceed Distance", description: "Enter distance in inches that will show a color indicating you can proceed", required: false, defaultValue: "20"
        input name: "proceed_color", type: "text", title: "Proceed Color", description: "Enter proceed color", required: false, defaultValue: "Blue"
        input name: "warning_distance", type: "number", title: "Warning Distance", description: "Enter distance in inches that will show a warning color", required: false, defaultValue: "15"
        input name: "warning_color", type: "text", title: "Warning Color", description: "Enter warning color", required: false, defaultValue: "Yellow"
        input name: "stop_distance", type: "number", title: "Stop Distance", description: "Enter distance in inches that will show a stop color", required: false, defaultValue: "10"
        input name: "stop_color", type: "text", title: "Stop Color", description: "Enter stop color", required: false, defaultValue: "Green"
        input name: "too_far_distance", type: "number", title: "Too Far Distance", description: "Enter distance in inches that will show a color that means you have gone too far", required: false, defaultValue: "5"
        input name: "too_far_color", type: "text", title: "Too Far Color", description: "Enter too far color", required: false, defaultValue: "Red"
    }
}


def parse(String description) {
    log.debug "parse(${description}) called"
	def parts = description.split(" ")
    def name  = parts.length>0?parts[0].trim():null
    def value = parts.length>1?parts[1].trim():null
    if (value.split()[0] == "get_inputs") {
        sendSettings()
    }
    else if (name && value) {
        double sensorValue = value as float
        def cm = sensorValue.round(1)
        def inches = (sensorValue / 2.54).round(1)
        sendEvent(name: name, value: inches)
        sendEvent(name: "cm", value: cm)
        // Update lastUpdated date and time
        def nowDay = new Date().format("MMM dd", location.timeZone)
        def nowTime = new Date().format("h:mm a", location.timeZone)
        sendEvent(name: "lastUpdated", value: nowDay + " at " + nowTime, displayed: false)
    }
    else {
    	log.debug "Missing either name or value.  Cannot parse!"
    }
}

def sendSettings() {
	def myName = device.deviceNetworkId.split("-")[1]
	parent.sendData("${myName} inputs " +
    	"local_poll:${settings.local_poll} " +
    	"proceed_distance:${settings.proceed_distance} " +
        "warning_distance:${settings.warning_distance} " +
        "stop_distance:${settings.stop_distance} " +
        "too_far_distance:${settings.too_far_distance}"
	)
}

def updated() {
	sendSettings()
}

def installed() {

}
