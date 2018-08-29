#ifndef AlarmDriver_h
#define AlarmDriver_h

#include "Particle.h"

class AlarmDriver {
  public:
    AlarmDriver(int *sensorsPins, int sensorCount, int sirenPin);
    ~AlarmDriver();
    int getSensorCount();
    bool *getSensorsState();
    bool activateSiren();
    bool deactivateSiren();
    bool getSirenState();
  private:
    int _sensorCount;
    int *_sensorsPins;
    bool *_sensorsState;
    int _sirenPin;
    bool _sirenState;
    void _readSensors();
};

#endif
