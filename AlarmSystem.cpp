#include "AlarmSystem.h"

AlarmSystem::AlarmSystem(AlarmDriver *alarmDriver) {
  _alarmDriver = alarmDriver;
  _isActive = false;
  _hasBreach = false;
  _isPanic = false;
  _sensorCount = _alarmDriver->getSensorCount();
  _triggeredSensor = -1;
  _sensorsStateAtActivation = new bool[_sensorCount];
  _disabledSensors = new bool[_sensorCount];

  for (int sensor = 0; sensor < _sensorCount; sensor += 1) {
    _disabledSensors[sensor] = false;
  }
}

AlarmSystem::~AlarmSystem() {
  delete[] _sensorsStateAtActivation;
  delete[] _disabledSensors;
}

bool AlarmSystem::activate(bool *sensorsToDisable) {
  if (_isActive) {
    return false;
  }

  if (!readyToActivate(sensorsToDisable)) {
    return false;
  }

  bool *sensorsState = _alarmDriver->getSensorsState();

  for (int sensor = 0; sensor < _sensorCount; sensor += 1) {
    bool isActive = sensorsState[sensor];
    bool isDisabled = sensorsToDisable[sensor];

    _sensorsStateAtActivation[sensor] = isActive;
    _disabledSensors[sensor] = isDisabled;
  }

  _isActive = true;

  return true;
}

bool AlarmSystem::deactivate() {
  _isActive = false;
  _hasBreach = false;
  _isPanic = false;
  _triggeredSensor = -1;
  _alarmDriver->deactivateSiren();

  return true;
}

bool AlarmSystem::readyToActivate(bool *sensorsToDisable) {
  if (_isActive) {
    return false;
  }

  bool *currentState = _alarmDriver->getSensorsState();

  for (int sensor = 0; sensor < _sensorCount; sensor += 1) {
    bool isActive =  currentState[sensor];
    bool isDisabled = sensorsToDisable[sensor];

    if (!isDisabled && !isActive) {
      return false;
    }
  }

  return true;
}

bool AlarmSystem::isActive() {
  return _isActive;
}

void AlarmSystem::triggerBreach() {
  _alarmDriver->activateSiren();
}

bool AlarmSystem::checkIfBreached() {
  if (!_isActive) {
    return false;
  }

  if (_hasBreach) {
    return true;
  }

  bool *currentState = _alarmDriver->getSensorsState();

  for (int sensor = 0; sensor < _sensorCount; sensor += 1) {
    if (!_disabledSensors[sensor] && currentState[sensor] != _sensorsStateAtActivation[sensor]) {
      _hasBreach = true;
      _triggeredSensor = sensor;
      return true;
    }
  }

  return false;
}

void AlarmSystem::triggerPanic() {
  _isPanic = true;
  _isActive = true;
  _hasBreach = true;
  _alarmDriver->activateSiren();
}

void AlarmSystem::testSiren(int durationMs) {
  if (!_isActive) {
    _alarmDriver->activateSiren();
    delay(durationMs);
    _alarmDriver->deactivateSiren();
  }
}

bool AlarmSystem::hasBreach() {
  return _hasBreach;
}

bool AlarmSystem::isPanic() {
  return _isPanic;
}

int AlarmSystem::getTriggeredSensor() {
  return _triggeredSensor;
}

String AlarmSystem::getSystemState() {
  bool *sensorsState = _alarmDriver->getSensorsState();
  bool sirenState = _alarmDriver->getSirenState();
  String state = String("");

  for (int sensor = 0; sensor < _sensorCount; sensor += 1) {
    bool sensorIsActive = sensorsState[sensor];
    bool sensorIsDisabled = _disabledSensors[sensor];

    state.concat(sensorIsActive ? 1 : 0);
    state.concat(sensorIsDisabled ? 'd' : 'e');

    if (sensor != _sensorCount-1) {
      state.concat('-');
    }
  }

  state.concat('|');
  state.concat(sirenState ? 1 : 0);

  state.concat('|');
  state.concat(_isActive ? 1 : 0);

  return state;
}
