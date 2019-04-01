#include <Arduino.h>
#include "VariableType.h"

using namespace std;

class Variable {

  public:

    Variable();
    Variable(String);
    Variable(String, String);

    void setLabel(String);
    String getLabel();

    void setStringValue(String);
    String getStringValue();

    void setFloatValue(float);
    float getFloatValue();

    void setIntValue(int);
    int getIntValue();

    void setType(VariableType&);
    VariableType getType();

    bool isEmpty();
    bool isDefaultValueEmpty();

  protected:

    String label;
    String value;
    String defaultValue;
    VariableType type = INT;

};
