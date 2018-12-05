/**
 *  Child Traffic Light
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
 *    2017-10-01  Allan (vseven) Original Creation (based on Dan Ogorchock's child dimmer switch)
 *    2017-10-06  Allan (vseven) Added preset color buttons and logic behind them.
 *	  2017-12-17  Allan (vseven) Modified setColor to use the newer color attributes of only hue and saturation which
 *                               it compatible with values passed in from things like Alexa or Goggle Home.
 *    2018-06-02  Dan Ogorchock  Revised/Simplified for Hubitat Composite Driver Model
 * 
 */

// for the UI
metadata {
	definition (name: "Child-Traffic-Light", namespace: "ogiewon", author: "Dan Ogorchock") {
        capability "Configuration"
        capability "Refresh"
        capability "Button"
        capability "Holdable Button"
        capability "Signal Strength"
        
        command "sendData", ["string"]
	}

    simulator {
    }

	// Tile Definitions
	tiles (scale: 2){
        valueTile("numberOfButtons", "device.numberOfButtons", inactiveLabel: false, width: 2, height: 2) {
			state "numberOfButtons", label:'${currentValue} buttons', unit:""
		}
	}
}


