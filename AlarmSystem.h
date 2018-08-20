#ifndef AlarmController_h
#define AlarmController_h

#include "Particle.h"
#include "AlarmDriver.h"

class AlarmSystem {
  public:
    AlarmSystem(AlarmDriver *alarmDriver);
    ~AlarmSystem();
    bool activate(bool *sensorsToDisable);
    bool deactivate();
    bool isReadyToActivate(bool *sensorsToDisable);
    bool isActive();
    void triggerBreach();
    bool checkIfBreached();
    void triggerPanic();
    void testSiren(int durationMs);
    bool hasBreach();
    bool isPanic();
    int getTriggeredSensor();
    String getSystemState();
  private:
    AlarmDriver *_alarmDriver;
    bool *_disabledSensors;
    bool *_sensorsStateAtActivation;
    int _sensorCount;
    int _triggeredSensor;
    bool _isActive;
    bool _hasBreach;
    bool _isPanic;
};

#endif
