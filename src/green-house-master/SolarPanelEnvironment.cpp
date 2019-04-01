#include "SolarPanelEnvironment.h"

SolarPanelEnvironment::SolarPanelEnvironment() {
}

float SolarPanelEnvironment::getSolarPanelTemperature() {
  return getSolarPanelVariable().getFloatValue() + alterSolarPanelTemp.getFloatValue();
}

float SolarPanelEnvironment::getBoilerTemperature() {
  return getBoilerVariable().getFloatValue() + alterBoilerTemp.getFloatValue();
}

bool SolarPanelEnvironment::isPanelSafetyON() {
  return getSolarPanelTemperature() >= solarPanelMaxTemp.getFloatValue() ||
  		getSolarPanelTemperature() <= solarPanelMinTemp.getFloatValue();
}

void SolarPanelEnvironment::checkPumpONState() {
  if ((isPanelSafetyON() == true) || 
		  (getSolarPanelTemperature() >= getBoilerTemperature() + start)) {
    // getSolarPanelTemperature() >= 45 + 22.75 = 67.75
		pumpON = true;
	}

	if (!isPanelSafetyON() && pumpON &&
		(getSolarPanelTemperature() <= getBoilerTemperature() + stop ||
     getBoilerTemperature() > targetBoilerTemp.getFloatValue())) {
    // getSolarPanelTemperature() <= 45 + 5.25 = 50.25
		pumpON = false;
	}

  exceptionHandling();
}

void SolarPanelEnvironment::exceptionHandling() {
  // exception handling
  float wireSensorErrorTemp = -127;
  if (wireSensorErrorTemp == getSolarPanelVariable().getFloatValue() ||
      wireSensorErrorTemp == getBoilerVariable().getFloatValue()) {
    messages.setStringValue(String("[Error] Some sensor wires are disconnected."));
    pumpON = true;
    return;
  }
}

void SolarPanelEnvironment::init(unsigned long currentMillis) {
  startUpMillis = currentMillis;

  // default values
  switch (systemMode.getIntValue()) {
    case 0: // OFF
      break;
    case 1: // ON
      break;
    case 2: // Vacation
      break;
    case 3: // Auto
      startPump.setIntValue(10);
      stopPump.setIntValue(5);
      break;
  }
}

Variable SolarPanelEnvironment::getSolarPanelVariable() {
  return temperatureSensors[getSolarPanelIndex()];
}

Variable SolarPanelEnvironment::getBoilerVariable() {
  int max = -999;
  Variable maxValueVar;
  for (int idx = 0; idx < temperatureSensorsSize; idx++) {
      if (idx == getSolarPanelIndex()) {
          continue;
      }
      Variable var = temperatureSensors[idx];
      
      int value = var.getIntValue();
      if (value > max) {
          maxValueVar = var;
          max = value;
      }
  }
  return maxValueVar;
}

int SolarPanelEnvironment::getSolarPanelIndex() {
  return solarPanelIndex.getIntValue() - 1;
}