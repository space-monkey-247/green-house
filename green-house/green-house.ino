// First we include the libraries
#include <OneWire.h>
#include <DallasTemperature.h>
//#include <Wire.h>
//#include <LiquidCrystal_I2C.h>
#include <ESP8266WiFi.h>
#include <UbidotsESPMQTT.h>
#include <GreenHouseEnvironment.h>

bool SERIAL_COMMUNICATION_ENABLED = true;

/**** Wifi Endava ******************************************************************/
//const char* SSID_NAME = "endava-byod";
//const char* SSID_PASS = "Agile-Transformation-Innovative-Solutions";
/**** Rooter Bogdan ****************************************************************/
const char* SSID_NAME = "Telekom-rOlKBz";
const char* SSID_PASS = "36kexrah4e1s";
/**** Hotspot Tudor ****************************************************************/
// const char* SSID_NAME = "Tudor Hotspot";
// const char* SSID_PASS = "Tudor123!";
/**** Hotspot Tudor ****************************************************************/
//const char* SSID_NAME = "HUAWEI-B310-5E3F";
//const char* SSID_PASS = "0AYGBJ9E8EQ";

int ubidotsProfileIndex = 0;
int thingsProfileIndex = 0;
const char* PROFILE = "ubidots";

char* servers[2] = {
  "things.ubidots.com",
  "mqtt.thingspeak.com"
};

const char* UBIDOTS_PROFILE = "ubidots";
const char* THINGSPEAK_PROFILE = "thingspeak";
const char* ACTIVE_PROFILE = UBIDOTS_PROFILE;

// test
// const char* TOKEN = "A1E-Brpd96xLq77tUwPkpXsXvCHCzdX4dZ";
// const char* HTTPSERVER = "things.ubidots.com";
// const char* API_ENDPOINT = "/v1.6/devices/";
// const char* DEVICE_LABEL = "solar-panel-test-env";

// artan
// const char* TOKEN = "A1E-Brpd96xLq77tUwPkpXsXvCHCzdX4dZ";
// const char* HTTPSERVER = "things.ubidots.com";
// const char* API_ENDPOINT = "/v1.6/devices/";
// const char* DEVICE_LABEL = "wemos-d1-mini";

// robert
const char* TOKEN = "A1E-Brpd96xLq77tUwPkpXsXvCHCzdX4dZ";
const char* HTTPSERVER = "mqtt://things.ubidots.com";
const char* API_ENDPOINT = "/v1.6/devices/";
const char* DEVICE_LABEL = "green-house-robert";
//const char* HTTPSERVER = "mqtt.thingspeak.com";

// common
//const char* USER_AGENT = "ESP8266";
//const char* VERSION = "1.0";
//int HTTPPORT = 80;

// http client
WiFiClient clientUbi;
// mqtt cloent
Ubidots mqttClient((char*)TOKEN);

/********************************************************************/
// Data wire is plugged into pin D3 on the Arduino
#define ONE_WIRE_BUS D3

/********************************************************************/
// Setup a oneWire instance to communicate with any OneWire devices
// (not just Maxim/Dallas temperature ICs)
OneWire oneWire(ONE_WIRE_BUS);

/********************************************************************/
// Pass our oneWire reference to Dallas Temperature.
DallasTemperature sensors(&oneWire);

// LCD + I2C
// LiquidCrystal_I2C lcd(0x27, 16, 2);  // set the LCD address to 0x27 for a 16 chars and 2 line display

// solar panel environment
GreenHouseEnvironment env;

void setup(void) {
  serialSetup();
  oneWireSensorsSetup();
  wifiSetup();
  mqttSetup();
  // pumpRelaySetup();
  // lcdSetup();

  env.init(millis());
}

void callback(char* topic, byte* payload, unsigned int length) {
  // if (ACTIVE_PROFILE == THINGSPEAK_PROFILE) {
  //   return;
  // }

  serialPrintln("");
  serialPrintln("Message arrived");


  serialPrint(" topic: [");
  serialPrint(String(topic));
  serialPrintln("] ");

  serialPrint(" length: ");
  serialPrintln(String(length));

  serialPrint(" payload: [");
  for (int i = 0; i < length; i++) {
    serialPrint(String((char)payload[i]));
  }
  serialPrintln("]");

  String label = extractLabel(topic);
  String value = extractValue(payload, length);

  if (label == "NULL") {
    serialPrintln(" Label is NULL");
    return;
  }

  //  serialPrintln(" Check all the downloadVariables");
  for (int idx = 0; idx < env.downloadVariablesSize; idx++) {
    //    serialPrintVariable(*env.downloadVariables[idx]);
    Variable& variable = *env.downloadVariables[idx];
    if (variable.getLabel().equalsIgnoreCase(label)) {
      updateVariableByRef(variable, value);
      break;
    }
  }
}

String extractValue(byte* payload, unsigned int length) {
  // char* charArrayPayload = (char*) payload;
  // Serial.println();
  // Serial.print("charArrayPayload: [");
  // Serial.print(charArrayPayload);
  // Serial.println("]");
  String value = "";
  for (int i = 0; i < length; i++) {
    value.concat(String((char)payload[i]));
  }
  serialPrintF(" value: %s\n", value);
  return value;
}

String extractLabel(char* topic) {
  String stringTopic = String(topic);
  String stringDeviceLabel = String(DEVICE_LABEL);
  uint8_t deviceLabelInitialPos = stringTopic.indexOf(stringDeviceLabel);
  uint8_t variableLabelInitialPos = deviceLabelInitialPos + stringDeviceLabel.length() + 1;
  stringTopic = stringTopic.substring(variableLabelInitialPos);
  String label = stringTopic.substring(0, stringTopic.indexOf("/"));
  serialPrintF(" label: %s\n", label);
  return label;
}

void updateVariableByRef(Variable& variable, String stringValue) {
  serialPrintCalculatedValue("existing value", variable.getStringValue());
  //  Serial.println(variable.getLabel() + " before: " + variable.getStringValue());

  if (stringValue != NULL && stringValue.length() > 0) {
    variable.setStringValue(stringValue);
    //    Serial.println(variable.getLabel() + " after1: " + variable.getStringValue());
    //    Serial.println(variable.getLabel() + " after2: " + String(variable.getFloatValue()));
  }
}

void mqttSetup() {
  mqttClient.setDebug(true); // Pass a true or false bool value to activate debug messages
  mqttClient.ubidotsSetBroker((char*)HTTPSERVER);
  mqttClient.ubidotsSetFirstPartTopic((char*)API_ENDPOINT);
  bool connected = mqttClient.wifiConnection((char *)SSID_NAME, (char *)SSID_PASS);
  if (connected) {
    Serial.println(" MQTT connected succesfully.");
    mqttClient.begin(callback);
    mqttSubscribeVariables();
  } else {
    Serial.println(" MQTT not connected!!!");
  }
}

void mqttSubscribeVariables() {
  // mqttClient.ubidotsSubscribe((char *)DEVICE_LABEL, "pump-start");
  // mqttClient.ubidotsSubscribe((char *)DEVICE_LABEL, "pump-stop");
  // mqttClient.ubidotsSubscribe((char *)DEVICE_LABEL, "solar-panel-index");
  // mqttClient.ubidotsSubscribe((char *)DEVICE_LABEL, "sensors-number");
}

void mqttPublish() {
  serialPrintln("");

  if (!mqttClient.connected()) {
    //    bool connected = mqttClient.wifiConnection((char *)SSID_NAME, (char *)SSID_PASS);
    //    if (!connected) {
    //      serialPrintln("MQTT Client: reconnect");
    //      mqttClient.reconnect();
    //    }
    // serialPrintln("MQTT Client: reconnect");
    // mqttClient.reconnect();
    mqttSetup();
    if (!mqttClient.connected()) {
      // force reconnecting
      WiFi.disconnect();
      mqttClient.disconnect();
    }
  }

  // skip publishing
  if (env.cycleNo % 2 == 0) {
    prepareMqttPublishValues();
  } else {
    serialPrint(" Skip publishing for cycle: ");
    serialPrintln(String(env.cycleNo));
  }

  mqttClient.loop();

  //  scheduledReconnect(60);
}

void scheduledReconnect(int reconnectMinutes) {
  // reconnect at x minutes
  long reconnectCycles = (long) reconnectMinutes * 60 * 1000 / env.DEFAULT_READ_INTERVAL;
  bool reconnect = env.cycleNo % reconnectCycles == 0;

  if (reconnect) {
    serialPrintln("MQTT Client: scheduled reconnect");
    // force reconnecting
    WiFi.disconnect();
    mqttClient.disconnect();
  }
}
void serialSetup() {
  // start serial port
  Serial.begin(115200);
  // while (!Serial) {
  //   ; // wait for serial port to connect. Needed for native USB
  // }
}

void wifiSetup() {
  wifiConnecting(3);
  printWifiStatus();
}

void printWifiStatus()
{
  // print the SSID of the network you're attached to
  serialPrintF("SSID: %s\n", String(WiFi.SSID()));

  // print your WiFi shield's IP address
  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);

  // print the received signal strength
  long rssi = WiFi.RSSI();
  Serial.print("Signal strength (RSSI):");
  Serial.print(rssi);
  Serial.println(" dBm");
  Serial.println();
}

void wifiConnecting(int retryNo) {
  Serial.println();
  if (WiFi.status() == WL_CONNECTED) {
    WiFi.setAutoReconnect(true);
    serialPrintln("WiFi Connected");
    return;
  }
  WiFi.begin(SSID_NAME, SSID_PASS);
  Serial.print("Connecting to ");
  Serial.print(SSID_NAME);
  Serial.print(" ");

  int count = 0;
  while (WiFi.status() != WL_CONNECTED && (retryNo == 0 || (count < retryNo))) {
    delay(500);
    Serial.print(".");
    count++;
  }
  Serial.println();
}

void oneWireSensorsSetup() {
  Serial.println("Dallas Temperature IC Control Library Demo");
  // Start up the library
  sensors.begin();
}

//#define BACKLIGHT_PIN     13
void lcdSetup() {
  // initialize the LCD
  //  lcd.init();

  // Turn on the backlight.
  //  lcd.backlight();
}

void lcdPrint(uint8_t col, uint8_t row, String text) {
  //  lcd.setCursor(col, row);
  //  lcd.print(text);
}

void lcdPrint(uint8_t col, uint8_t row, char aChar) {
  //  lcd.setCursor(col, row);
  //  lcd.print(aChar);
}

void loop(void) {

  // try to reconnecting for 3 times
  wifiConnecting(3);

  // read temperature of wired devices
  readTemperatures();

  // send to ubidots
  mqttPublish();

  serialPrintln("");
  serialPrintln(F("********************************************************************"));

  // sleep 5 secs
  delay(env.DEFAULT_READ_INTERVAL);
}

void serialPrintln(String strValue) {
  if (!SERIAL_COMMUNICATION_ENABLED) {
    return;
  }
  Serial.println(strValue);
}

void serialPrint(String strValue) {
  if (!SERIAL_COMMUNICATION_ENABLED) {
    return;
  }
  Serial.print(strValue);
}

void serialPrintFloat(const char* formatedMessage, float floatValue) {
  if (!SERIAL_COMMUNICATION_ENABLED) {
    return;
  }
  char* str = (char *) malloc(sizeof(char) * 255);
  char str_val[30];
  dtostrf(floatValue, 4, 2, str_val);
  sprintf(str, formatedMessage, str_val);
  Serial.printf(str);
  free(str);
}

void serialPrintInt(const char* formatedMessage, int intValue) {
  if (!SERIAL_COMMUNICATION_ENABLED) {
    return;
  }
  char* str = (char *) malloc(sizeof(char) * 255);
  sprintf(str, formatedMessage, intValue);
  Serial.printf(str);
  free(str);
}

void serialPrintF(const char* formatedMessage) {
  if (!SERIAL_COMMUNICATION_ENABLED) {
    return;
  }
  //  Serial.print(F(formatedMessage));
  char* str = (char *) malloc(sizeof(char) * 255);
  sprintf(str, formatedMessage);
  Serial.printf(str);
  free(str);
}

void serialPrintF(const char* formatedMessage, String strValue) {
  if (!SERIAL_COMMUNICATION_ENABLED) {
    return;
  }
  //  Serial.print(F(formatedMessage));
  char* str = (char *) malloc(sizeof(char) * 255);
  sprintf(str, formatedMessage, strValue.c_str());
  Serial.printf(str);
  free(str);
}

void serialPrintVariable(Variable variable) {
  char* str = (char *) malloc(sizeof(char) * 255);
  sprintf(str, "  %s: ", variable.getLabel().c_str());
  serialPrint(str);
  serialPrintln(variable.getStringValue());
  free(str);
}

void serialPrintCalculatedValue(const char* variableLabel, String value) {
  char* str = (char *) malloc(sizeof(char) * 255);
  sprintf(str, " %s: ", variableLabel);
  serialPrint(str);
  serialPrintln(value);
  free(str);
}

void readTemperatures() {
  serialPrintF("\n");
  serialPrintF("Requesting temperatures...\n");
  sensors.requestTemperatures();
  //Serial.println("DONE");

  for (int idx = 0; idx < env.temperatureSensorsSize; idx++) {
    float temp = sensors.getTempCByIndex(idx);
    env.temperatureSensors[idx].setFloatValue(temp);
    serialPrint("sensor[");
    serialPrint(String(idx + 1));
    serialPrint("] - Solar panel temperature is: ");
    serialPrintln(String(temp));
  }
}

char* stringToChar(String stringValue) {
  char* str = (char *) malloc(sizeof(char) * 255);
  //  sprintf(str, "%s", stringValue.c_str());
  //  strcpy(str, stringValue.c_str());
  stringValue.toCharArray(str, 255);
  return str;
}

void mqttPublishValues() {
  // publish
  bool published = mqttClient.ubidotsPublishOnlyValues((char *)DEVICE_LABEL, true);
  if (!published) {
    serialPrintln("");
    serialPrintln("[Error] MQTT client unable to publish.");
    // force reconnecting
    WiFi.disconnect();
    delay(500);
    mqttClient.disconnect();
  }
}

void prepareMqttPublishValues() {
  serialPrintln("Prepare MQTT publishing values:");
  /*
    //  serialPrintVariable(env.boilerTemp);
    mqttClient.add(stringToChar(env.getBoilerVariable().getLabel()), env.getBoilerTemperature());
    //  serialPrintVariable(env.solarPanelTemp);
    mqttClient.add(stringToChar(env.getSolarPanelVariable().getLabel()), env.getSolarPanelTemperature());
  */

  for (int idx = 0; idx < env.temperatureSensorsSize; idx++) {
    Variable var = env.temperatureSensors[idx];
    mqttClient.add(stringToChar(var.getLabel()), var.getFloatValue());
  }
  mqttPublishValues();

  //  serialPrintVariable(env.systemRunningTime);
  mqttClient.add(stringToChar(env.systemRunningTime.getLabel()), env.systemRunningTime.getFloatValue());
  //  serialPrintVariable(env.cycles);
  mqttClient.add(stringToChar(env.cycles.getLabel()), env.cycles.getFloatValue());

  mqttPublishValues();
}

