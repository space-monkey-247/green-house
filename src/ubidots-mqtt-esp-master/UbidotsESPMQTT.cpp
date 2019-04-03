/*
Copyright (c) 2016 Ubidots.

Permission is hereby granted, free of charge, to any person obtaining
a copy of this software and associated documentation files (the
"Software"), to deal in the Software without restriction, including
without limitation the rights to use, copy, modify, merge, publish,
distribute, sublicense, and/or sell copies of the Software, and to
permit persons to whom the Software is furnished to do so, subject to
the following conditions:

The above copyright notice and this permission notice shall be
included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE
LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

Original Maker: Mateo Velez - Metavix for Ubidots Inc
Modified and Maintened by: Jose Garcia - Ubidots Inc

*/

#include "UbidotsESPMQTT.h"


Ubidots::Ubidots(char* token){
    initialize(token, NULL);
}


Ubidots::Ubidots(char* token, char* clientName) {
    initialize(token, clientName);
}


void Ubidots::begin(void (*callback)(char*,uint8_t*,unsigned int)) {
    this->callback = callback;
    _client.setServer(_server, MQTT_PORT);
    _client.setCallback(callback);
}


bool Ubidots::add(char* variableLabel, float value) {
    return add(variableLabel, value, "NULL", "NULL");
}

char* Ubidots::stringToChar(String stringValue) {
  char* str = (char *) malloc(sizeof(char) * 255);
//  sprintf(str, "%s", stringValue.c_str());
//  strcpy(str, stringValue.c_str());
  stringValue.toCharArray(str, 255);
  return str;
}

bool Ubidots::addUsingStringLabel(String variableLabel, float value) {
    char* str = stringToChar(variableLabel);
    bool wasAdded = add(str, value, "NULL", "NULL");
    //free(str);
    return wasAdded;
}

bool Ubidots::add(char* variableLabel, float value, char *context) {
    return add(variableLabel, value, context, "NULL");
}

bool Ubidots::add(char* variableLabel, float value, char *context, char *timestamp) {
    // if (_debug) {
    //     Serial.print(F("adding: "));
    //     Serial.println(variableLabel);
    // }

    (val+currentValue)->_variableLabel = variableLabel;
    (val+currentValue)->_value = value;
    (val+currentValue)->_context = context;
    (val+currentValue)->_timestamp = timestamp;
    currentValue++;
    if (currentValue > MAX_VALUES) {
        Serial.println(F("You are sending more than the maximum of consecutive variables"));
        currentValue = MAX_VALUES;
    }

    // if (_debug) {
    //     Serial.println(F("added"));
    // }
    return true;
}


bool Ubidots::connected() {
    return _client.connected();
}


char* Ubidots::getMac() {
    // Obtains the MAC of the device
    Serial.println("entra");
    byte mac[6];
    WiFi.macAddress(mac);
    char macAddr[18];
    sprintf(macAddr, "%2X%2X%2X%2X%2X%2X", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
    return macAddr;
}


void Ubidots::initialize(char* token, char* clientName) {
    _server = SERVER;
    _first_part_topic = FIRST_PART_TOPIC;
    _token = token;
    currentValue = 0;
    val = (Value *)malloc(MAX_VALUES*sizeof(Value));
    if(clientName!=NULL){
        _clientName = clientName;
    }
}


bool Ubidots::loop() {
    if (!_client.connected()) {
        reconnect();
    }
    return _client.loop();
}


void Ubidots::reconnect() {
    int times = 0;
    while (!_client.connected() && times < 10) {
        Serial.print("Attempting MQTT connection...");
        if (_client.connect(_clientName, _token, NULL)) {
            Serial.println("connected");
            break;
        } else {
            Serial.print("failed, rc=");
            Serial.print(_client.state());
            Serial.println(" try again in 3 seconds");
            delay(3000);
        }
        times++;
    }
}

void Ubidots::disconnect() {
    _client.disconnect();
    delay(500);
}

void Ubidots::setDebug(bool debug){
    _debug = debug;
}


void Ubidots::ubidotsSetBroker(char* broker) {
    if (_debug){
        Serial.println("Broker set for Business Account");
    }
    _server = broker;
}

void Ubidots::ubidotsSetFirstPartTopic(char* firstPartTopic) {
    _first_part_topic = firstPartTopic;
}

bool Ubidots::ubidotsSubscribe(char* deviceLabel, char* variableLabel) {
    char topic[150];
    sprintf(topic, "%s%s/%s/lv", _first_part_topic, deviceLabel, variableLabel);
    if (!_client.connected()) {
        reconnect();
    }
    if (_debug) {
        Serial.println("Subscribed to: ");
        Serial.println(topic);
    }
    return _client.subscribe(topic);
}


bool Ubidots::ubidotsPublish(char *deviceLabel) {
    char topic[150];
    char payload[500];
    String str;

    // JSON dict: {"solar-panel-temperature": {"value": 10.00}, "boiler-temperature": {"value": 20.00}}

    sprintf(topic, "%s%s", _first_part_topic, deviceLabel);
    sprintf(payload, "{");
    for (int i = 0; i <= currentValue; ) {
        str = String((val+i)->_value, 2);
        sprintf(payload, "%s\"%s\": {\"value\": %s", payload, (val+i)->_variableLabel, str.c_str());
        // free((val+i)->_variableLabel);
        if ((val+i)->_timestamp != "NULL") {
            sprintf(payload, "%s, \"timestamp\": %s", payload, (val+i)->_timestamp);
            // free((val+i)->_timestamp);
        }
        if ((val+i)->_context != "NULL") {
            sprintf(payload, "%s, \"context\": {%s}", payload, (val+i)->_context);
            // free((val+i)->_context);
        }
    
        i++;
        if (i >= currentValue) {
            sprintf(payload, "%s}}", payload);
            break;
        } else {
            sprintf(payload, "%s}, ", payload);
        }
    }
    if (_debug){
        Serial.println(" publishing ");
        Serial.println("  TOPIC: ");
        Serial.println(topic);
        Serial.print("  JSON: ");
        Serial.println(payload);
    }
    currentValue = 0;
    return _client.publish(topic, payload, 500);
}

bool Ubidots::ubidotsPublishOnlyValues(char *deviceLabel, bool freeMemory) {
    char topic[500];
    char payload[2000];
    String str;

    // JSON dict: {"solar-panel-temperature": 10.00, "boiler-temperature": 20.00}

    sprintf(topic, "%s%s", _first_part_topic, deviceLabel);
    sprintf(payload, "{");
    for (int i = 0; i <= currentValue; ) {
        str = String((val+i)->_value, 2);
        sprintf(payload, "%s\"%s\": %s", payload, (val+i)->_variableLabel, str.c_str());
        if (freeMemory) {
            free((val+i)->_variableLabel);
        }

        i++;
        if (i >= currentValue) {
            sprintf(payload, "%s}", payload);
            break;
        } else {
            sprintf(payload, "%s, ", payload);
        }
    }
    if (_debug) {
        Serial.println(" publishing ");
        Serial.print("  TOPIC: ");
        Serial.println(topic);
        Serial.print("  JSON: ");
        Serial.println(payload);
        // Serial.print(" strlen(payload): ");
        // Serial.println(strlen(payload));
    }
    currentValue = 0;
    return _client.publish(topic, payload, 2000);
}


bool Ubidots::wifiConnection(char* ssid, char* pass) {
    WiFi.begin(ssid, pass);
    int times = 0;
    while (WiFi.status() != WL_CONNECTED && times < 90) {
        delay(500);
        Serial.print(".");
        times++;
    }
    if (WiFi.status() != WL_CONNECTED) {
        return false;
    }
    Serial.println(F("WiFi connected"));
    Serial.println(F("IP address: "));
    Serial.println(WiFi.localIP());
    if(_clientName==NULL){
        _clientName = getMac();
    }
    return true;
}
