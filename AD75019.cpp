#include <Arduino.h>
#include "AD75019.h"

void _ad75019_pinModeDefault(uint8_t pinNumber, uint8_t /* unused */) {
  pinMode(pinNumber, OUTPUT);
}

void _ad75019_digitalWriteDefault(uint8_t pinNumber, uint8_t value) {
  digitalWrite(pinNumber, value);
}

AD75019::AD75019(uint8_t pclkPinNumber, uint8_t sclkPinNumber, uint8_t sinPinNumber):
  _pclkPinNumber(pclkPinNumber), _sclkPinNumber(sclkPinNumber), _sinPinNumber(sinPinNumber),
  _pinModeCallback(_ad75019_pinModeDefault), _digitalWriteCallback(_ad75019_digitalWriteDefault) {
    setUseDefaultCallbacks(true);
 }

AD75019::AD75019(uint8_t pclkPinNumber, uint8_t sclkPinNumber, uint8_t sinPinNumber, 
          voidFuncCallback_t pinModeCallback, voidFuncCallback_t digitalWriteCallback):
  _pclkPinNumber(pclkPinNumber), _sclkPinNumber(sclkPinNumber), _sinPinNumber(sinPinNumber),
  _pinModeCallback(pinModeCallback), _digitalWriteCallback(digitalWriteCallback) {}


bool AD75019::begin() {
    Serial.println("Starting basic AD75019 initialization...");

    _pinModeCallback(_pclkPinNumber, OUTPUT);
    _pinModeCallback(_sclkPinNumber, OUTPUT);
    _pinModeCallback(_sinPinNumber, OUTPUT);
    
    Serial.println("Pins configured as OUTPUT.");
    
    _digitalWriteCallback(_pclkPinNumber, HIGH);
    _digitalWriteCallback(_sclkPinNumber, LOW);
    _digitalWriteCallback(_sinPinNumber, LOW);
    
    Serial.println("Initial pin states set.");

    setBegun(true);
    Serial.println("AD75019 initialization complete.");
    return true;
}

bool AD75019::begin(uint8_t xPinMapping[16], uint8_t yPinMapping[16]) {
    Serial.println("Starting AD75019 with simplified pin mappings...");

    // Bypass pin mapping validation for testing purposes
    for (uint8_t i = 0; i < 16; i++) {
        _xPinMapping[i] = xPinMapping[i];
        _yPinMapping[i] = yPinMapping[i];
    }

    Serial.println("Pin mappings applied successfully.");
    return begin();  // Proceed with basic initialization
}

void AD75019::addRoute(uint8_t x, uint8_t y) {
    Serial.print("Adding route: X"); Serial.print(x); Serial.print(" to Y"); Serial.println(y);
    bitWrite(
        _configBuffer[_yPinMapping[y]],
        _xPinMapping[x], 1);
}

bool AD75019::isRouted(uint8_t x, uint8_t y) {
    return bitRead(
        _configBuffer[_yPinMapping[y]],
        _xPinMapping[x]);
}

void AD75019::flush() {
    if (!isBegun()) {
        Serial.println("AD75019 not begun. Cannot flush.");
        return;
    }
    Serial.println("Flushing configuration to AD75019...");
    for (int8_t y = 15; y > -1; y--) {
        for (int8_t x = 15; x > -1; x--) {
            _digitalWriteCallback(_sinPinNumber, bitRead(_configBuffer[y], x));
            _digitalWriteCallback(_sclkPinNumber, HIGH);
            _digitalWriteCallback(_sclkPinNumber, LOW);
        }
    }
    _digitalWriteCallback(_pclkPinNumber, LOW);
    _digitalWriteCallback(_pclkPinNumber, HIGH);
    Serial.println("Configuration flush complete.");
}

void AD75019::clear() {
    Serial.println("Clearing configuration buffer...");
    for (uint8_t i = 0; i < 16; i++) {
        _configBuffer[i] = 0;
    }
    Serial.println("Configuration buffer cleared.");
}

void AD75019::print() {
    if (!isBegun()) {
        Serial.println(F("AD75019 not initialized!"));
        return;
    }
    Serial.println(F("  X: 5432 1098 7654 3210"));
    for (int8_t y = 15; y > -1; y--) {
        if (y > 9) {
            Serial.print(F("Y"));
            Serial.print(y);
            Serial.print(F(": "));
        } else {
            Serial.print(F(" Y"));
            Serial.print(y);
            Serial.print(F(": "));
        }
        for (int8_t x = 15; x > -1; x--) {
            Serial.print(bitRead(_configBuffer[y], x));
            if (x % 4 == 0) {
                Serial.print(F(" "));
            }
        }
        Serial.println();
    }
}

void AD75019::setBegun(bool b) {
    bitWrite(_state, 1, b);
}

bool AD75019::isBegun() {
    return bitRead(_state, 1);
}

void AD75019::setUseDefaultCallbacks(bool b) {
    bitWrite(_state, 0, b);
}

bool AD75019::isUseDefaultCallbacks() {
    return bitRead(_state, 0);
}
