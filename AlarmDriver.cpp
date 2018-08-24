#include "AlarmDriver.h"

AlarmDriver::AlarmDriver(int *sensorsPins, int sensorCount, int sirenPin) {
  _sensorCount = sensorCount;
  _sensorsPins = sensorsPins;
  _sensorsState = new bool[_sensorCount];
  _sirenPin = sirenPin;
  _sirenState = false;

  pinMode(_sirenPin, OUTPUT);

  for (int sensor = 0; sensor < _sensorCount; sensor += 1) {
    pinMode(_sensorsPins[sensor], INPUT_PULLDOWN);
    _sensorsState[sensor] = false;
  }
}

AlarmDriver::~AlarmDriver() {
  delete[] _sensorsState;
}

bool *AlarmDriver::getSensorsState() {
  _readSensors();

  return _sensorsState;
}

bool AlarmDriver::activateSiren() {
  if (!_sirenState) {
    digitalWrite(_sirenPin, HIGH);
    _sirenState = true;
  }

  return _sirenState;
}

bool AlarmDriver::deactivateSiren() {
  if (_sirenState) {
    digitalWrite(_sirenPin, LOW);
    _sirenState = false;
  }

  return _sirenState;
}

bool AlarmDriver::getSirenState() {
  return _sirenState;
}

int AlarmDriver::getSensorCount() {
  return _sensorCount;
}

void AlarmDriver::_readSensors() {
  for (int sensor = 0; sensor < _sensorCount; sensor += 1) {
    int sensorPin = _sensorsPins[sensor];
    _sensorsState[sensor] = (digitalRead(sensorPin) == HIGH);
  }
}

void AlarmDriver::_readSiren() {
  _sirenState = (digitalRead(_sirenPin) == HIGH);
}
