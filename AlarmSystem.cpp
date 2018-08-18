#include "AlarmSystem.h"

AlarmSystem::AlarmSystem(AlarmDriver *alarmDriver) {
  _alarmDriver = alarmDriver;
  _systemIsActive = false;
  _isBreached = false;
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
  _alarmDriver->deactivateSiren();
}

bool AlarmSystem::getSystemIsActive() {
  return _systemIsActive;
}

void AlarmSystem::alertIfBreach() {
  bool hasBreach = _checkIfBreach();
  if (_systemIsActive && hasBreach) {
    _isBreached = true;
    _alarmDriver->activateSiren();
  }
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

bool AlarmSystem::_checkIfBreach() {
  bool *currentState = _alarmDriver->getSensorsState();

  for (int sensor = 0; sensor < _sensorCount; sensor += 1) {
    if (!_disabledSensors[sensor] && currentState[sensor] != _sensorsStateAtActivation[sensor]) {
      return true;
    }
  }

return false;
}
