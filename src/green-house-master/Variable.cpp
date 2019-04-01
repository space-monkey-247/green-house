#include "Variable.h"

using namespace std;

Variable::Variable() {
}

Variable::Variable(String strLabel) {
	label = strLabel;
}

Variable::Variable(String strLabel, String strValue) {
	label = strLabel;
	value = strValue;
	defaultValue = strValue;
}

void Variable::setLabel(String str) {
	label = str;
}

String Variable::getLabel() {
	return label;
}

void Variable::setStringValue(String stringValue) {
	if (isEmpty()) {
		value = stringValue;
	} else {
		value.replace(value.c_str(), stringValue.c_str());
	}
}

String Variable::getStringValue() {
	if (isEmpty()) {
		if (!isDefaultValueEmpty()) {
			return defaultValue;
		} else {
			return String("0");
		}
	}
	return value;
}

void Variable::setFloatValue(float floatValue) {
	String stringValue = String(floatValue);
	if (isEmpty()) {
		value = stringValue;
	} else {
		value.replace(value.c_str(), stringValue.c_str());
	}
}

float Variable::getFloatValue() {
	if (isEmpty()) {
		return defaultValue.toFloat();
	}
	return value.toFloat();
}

void Variable::setIntValue(int intValue) {
	String stringValue = String(intValue);
	if (isEmpty()) {
		value = stringValue;
	} else {
		value.replace(value.c_str(), stringValue.c_str());
	}
}

int Variable::getIntValue() {
	if (isEmpty()) {
		return defaultValue.toInt();
	}
	return value.toInt();
}

void Variable::setType(VariableType& aType) {
	type =  aType;
}

VariableType Variable::getType() {
	return type;
}

bool Variable::isEmpty() {
	return value == NULL || value.length() == 0;
}

bool Variable::isDefaultValueEmpty() {
	return defaultValue == NULL || defaultValue.length() == 0;
}
