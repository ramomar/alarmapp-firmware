#include "AlarmSystem.h"

AlarmSystem::AlarmSystem(AlarmDriver *alarmDriver) {
  _alarmDriver = alarmDriver;
  _systemIsActive = false;
  _isBreached = false;
  _isPanic = false;
  _sensorCount = _alarmDriver->getSensorCount();
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

void AlarmSystem::activate(bool *sensorsToDisable) {
  if (_systemIsActive) {
    return ;
  }

  bool *sensorsState = _alarmDriver->getSensorsState();

  for (int sensor = 0; sensor < _sensorCount; sensor += 1) {
    bool sensorIsActive = sensorsState[sensor];
    bool sensorIsDisabled = sensorsToDisable[sensor];

    _sensorsStateAtActivation[sensor] = sensorIsActive;
    _disabledSensors[sensor] = sensorIsDisabled;
  }

  _systemIsActive = true;
}

void AlarmSystem::deactivate() {
  _systemIsActive = false;
  _isBreached = false;
  _isPanic = false;
  _alarmDriver->deactivateSiren();
}

bool AlarmSystem::getSystemIsActive() {
  return _systemIsActive;
}

void AlarmSystem::triggerBreach() {
  _alarmDriver->activateSiren();
}

bool AlarmSystem::checkIfBreached() {
  if (!_systemIsActive) {
    return false;
  }

  if (_isBreached) {
    return true;
  }

  bool *currentState = _alarmDriver->getSensorsState();

  for (int sensor = 0; sensor < _sensorCount; sensor += 1) {
    if (!_disabledSensors[sensor] && currentState[sensor] != _sensorsStateAtActivation[sensor]) {
      _isBreached = true;
      return true;
    }
  }

  return false;
}

void AlarmSystem::triggerPanic() {
  _isPanic = true;
  _systemIsActive = true;
  _isBreached = true;
  _alarmDriver->activateSiren();
}

void AlarmSystem::testSiren(int durationMs) {
  _alarmDriver->activateSiren();
  delay(durationMs);
  _alarmDriver->deactivateSiren();
}

bool AlarmSystem::getIsBreached() {
  return _isBreached;
}

bool AlarmSystem::getIsPanic() {
  return _isPanic;
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
  state.concat(_systemIsActive ? 1 : 0);

  return state;
}
