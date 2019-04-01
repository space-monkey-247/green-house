#include "Variable.h"

using namespace std;

class SolarPanelEnvironment {

  public:

    int DEFAULT_READ_INTERVAL = 5000;

    SolarPanelEnvironment();

    char* deviceLabel;
    bool pumpON = false;
    float targetDelta;
    float start;
    float stop;
    bool pumpOnPreviousValue = false;
    unsigned long startUpMillis = 0;
    unsigned long previousMillis = 0;
    unsigned long upTime = 0;
    unsigned long cycleNo = 0;

    // current status
    Variable solarPanelTemp = Variable("solar-panel-temperature", "0");
    Variable boilerTemp = Variable("boiler-temperature", "0");
    Variable pumpStatus = Variable("pump-status", "0");

    Variable temperatureSensors[4] = {
      Variable("sensor-1", "0"),
      Variable("sensor-2", "0"),
      Variable("sensor-3", "0"),
      Variable("sensor-4", "0")
    };
	  int temperatureSensorsSize = sizeof(temperatureSensors) / sizeof(temperatureSensors[0]);

    Variable getSolarPanelVariable();
    Variable getBoilerVariable();
    int getSolarPanelIndex();
    
    /**
    SystemMode {
       0 OFF      #ff2500
       1 ON       #4BA651
       2 Vacation #ffec02
       3 Auto I   #169c8d
       4 Auto II  #169c8d
       5 Auto III  #169c8d
    }
    */
    Variable systemMode = Variable("system-mode", "3");
    Variable systemRunningTime = Variable("system-running-time", "0");
    Variable cycles = Variable("cycle-number", "0");
    Variable messages = Variable("messages");
    Variable solarPanelIndex = Variable("solar-panel-index", "1");
    Variable boilerTemperatureSensors = Variable("sensors-number", "2");

    // running
    Variable targetBoilerTemp = Variable("target-boiler-temperature", "65");
    Variable startPump = Variable("pump-start", "10");
    Variable stopPump = Variable("pump-stop", "5");

    // alter values
    Variable alterSolarPanelTemp = Variable("alter-solar-panel-temperature", "0");
    Variable alterBoilerTemp = Variable("alter-boiler-temperature", "0");

    // safety
    Variable solarPanelMaxTemp = Variable("solar-panel-max-temperature", "98");
    Variable solarPanelMinTemp = Variable("solar-panel-min-temperature", "-10");

    Variable* downloadVariables[10] = {
      &systemMode,
      &targetBoilerTemp,
      &startPump,
      &stopPump,
      &solarPanelIndex,
      &boilerTemperatureSensors,
      &alterBoilerTemp,
      &alterSolarPanelTemp,
      &solarPanelMaxTemp,
      &solarPanelMinTemp
    };

	  int downloadVariablesSize = sizeof(downloadVariables) / sizeof(downloadVariables[0]);
    int variableDownloadIndex = 0;

    void init(unsigned long currentMillis);
    bool isPanelSafetyON();
    void checkPumpONState();
    void exceptionHandling();

    float getSolarPanelTemperature();
    float getBoilerTemperature();

};
