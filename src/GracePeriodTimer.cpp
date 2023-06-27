#include "GracePeriodTimer.h"

GracePeriodTimer::GracePeriodTimer(int buzzerPin) {
  pinMode(buzzerPin, OUTPUT);
  _buzzerPin = buzzerPin;
  _isActive = false;
  _isFinished = false;
  _tick = 0;
  _phase = 0;
  _previousTimeMs = 0;
}

void GracePeriodTimer::start(unsigned long currentTimeMs) {
  if (_isActive) {
    return;
  }

  _isActive = true;
  _isFinished = false;
  _tick = 0;
  _phase = 0;
  _previousTimeMs = currentTimeMs;
}

void GracePeriodTimer::tick(unsigned long currentTimeMs) {
  if (!_isActive) {
    return;
  }

  if (_tick > 60) {
    _isActive = false;
    _isFinished = true;
    return;
  }

  int delayMs = _computeDelayInMs();

  unsigned long deltaBetweenTones = currentTimeMs - _previousTimeMs;

  if (abs(deltaBetweenTones) >= delayMs) {
    tone(_buzzerPin, 3817, 100); // C4

    _tick += 1;

    if (_tick % 20 == 0) {
      _phase += 1;
    }

    _previousTimeMs = currentTimeMs;
  }
}

void GracePeriodTimer::reset() {
  _isActive = false;
  _isFinished = false;
}

bool GracePeriodTimer::isActive() {
  return _isActive;
}

bool GracePeriodTimer::isFinished() {
  return _isFinished;
}

unsigned long GracePeriodTimer::_computeDelayInMs() {
  unsigned long delay;

  switch (_phase) {
    case 0:
      delay = 500; // 20 ticks * 500 ms / 1000 ms = 10 secs
      break;
    case 1:
      delay = 200; // 20 ticks * 250 ms / 1000 ms = 4 secs
      break;
    case 2:
      delay = 150; // 20 ticks * 150 ms / 1000 ms = 3 secs
      break;
  }

  return delay;
}
