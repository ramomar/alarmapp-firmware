#ifndef gracePeriodTimer_h
#define gracePeriodTimer_h

#include "Particle.h"

class GracePeriodTimer {
  public:
    GracePeriodTimer(int buzzerPin);
    void start(unsigned long currentTimeMs);
    void tick(unsigned long currentTimeMs);
    void reset();
    bool isActive();
    bool isFinished();
  private:
    int _buzzerPin;
    bool _isActive;
    bool _isFinished;
    int _phase;
    int _tick;
    unsigned long _previousTimeMs;
    unsigned long _computeDelayInMs();
};

#endif
