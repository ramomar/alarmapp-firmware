#ifndef AlarmController_h
#define AlarmController_h

#include "Particle.h"
#include "AlarmDriver.h"

class AlarmSystem {
  public:
    AlarmSystem(AlarmDriver *alarmDriver);
    ~AlarmSystem();
    void activate(bool *sensorsToDisable);
    void deactivate();
    bool getSystemIsActive();
    void triggerIfBreach();
    void triggerPanic();
    void testSiren(int durationMs);
    void getIsBreached();
    String getSystemState();
  private:
    AlarmDriver *_alarmDriver;
    bool *_disabledSensors;
    bool *_sensorsStateAtActivation;
    int _sensorCount;
    bool _systemIsActive;
    bool _isBreached;
    bool _isPanic;
    bool _checkIfBreach();
};

#endif
