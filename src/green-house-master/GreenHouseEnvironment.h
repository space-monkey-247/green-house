#include "Variable.h"

using namespace std;

class GreenHouseEnvironment {

  public:

    int DEFAULT_READ_INTERVAL = 5000;

    GreenHouseEnvironment();

    Variable temperatureSensors[4] = {
      Variable("sensor-1", "0"),
      Variable("sensor-2", "0"),
      Variable("sensor-3", "0"),
      Variable("sensor-4", "0")
    };
	  int temperatureSensorsSize = sizeof(temperatureSensors) / sizeof(temperatureSensors[0]);

    Variable* downloadVariables[0] = {
    };
	  int downloadVariablesSize = sizeof(downloadVariables) / sizeof(downloadVariables[0]);

    Variable systemRunningTime = Variable("system-running-time", "0");
    Variable cycles = Variable("cycle-number", "0");

    unsigned long startUpMillis = 0;
    unsigned long previousMillis = 0;
    unsigned long upTime = 0;
    unsigned long cycleNo = 0;

    void init(unsigned long currentMillis);

};
