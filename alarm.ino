#include "AlarmDriver.h"
#include "AlarmSystem.h"

int sensorsPins[] = { D0, D1, D2, D3, D4, D5 };
int deactivateSystemButtonPin = A0;
int activateSystemButtonPin = A1;
int sirenPin = A2;
int buzzerPin = A5;
int statusPin = D7;

AlarmDriver *alarmDriver;
AlarmSystem *alarmSystem;

bool gracePeriodExpired;
bool notifyWithBuzzer;

String previousSystemState;

int sensorCount = sizeof(sensorsPins) / sizeof(sensorsPins[0]);

// Remeber to modify this timer if you modify the grace period buzzer routine.
Timer gracePeriodTimer(16000, gracePeriodExpiredCallback, true);

void setup() {
  pinMode(statusPin, OUTPUT);
  pinMode(buzzerPin, OUTPUT);
  pinMode(deactivateSystemButtonPin, INPUT_PULLUP);
  pinMode(activateSystemButtonPin, INPUT_PULLUP);

  attachInterrupt(deactivateSystemButtonPin, deactivateSystemButtonInterruptCallback, CHANGE, 0);
  attachInterrupt(activateSystemButtonPin, activateSystemButtonInterruptCallback, CHANGE, 0);

  gracePeriodExpired = false;

  notifyWithBuzzer = true;

  alarmDriver = new AlarmDriver(sensorsPins, sensorCount, sirenPin);
  alarmSystem = new AlarmSystem(alarmDriver);

  Particle.variable("systemState", previousSystemState);

  Particle.subscribe("activateSystem", activateSystemEventHandler, MY_DEVICES);
  Particle.subscribe("deactivateSystem", deactivateSystemEventHandler, MY_DEVICES);
  Particle.subscribe("triggerPanic", triggerPanicEventHandler, MY_DEVICES);
  Particle.subscribe("testSiren", testSirenEventHandler, MY_DEVICES);
}

void loop() {
  if (notifyWithBuzzer) {
    buzzerSaySomething();
    notifyWithBuzzer = false;
  }

  bool hasBreach = alarmSystem->checkIfBreached();
  bool isPanic = alarmSystem->getIsPanic();

  if (hasBreach) {
    digitalWrite(statusPin, HIGH);
  } else {
    digitalWrite(statusPin, LOW);
  }

  if (!gracePeriodExpired && hasBreach && !isPanic) {
    gracePeriodTimer.start();
    buzzerGracePeriod();
  }

  String currentSystemState = alarmSystem->getSystemState();

  if (previousSystemState != currentSystemState) {
    if (Particle.connected()) {
      Particle.publish("systemState", currentSystemState, 60, PRIVATE);
    }

    previousSystemState = currentSystemState;
  }
}

void activateSystemEventHandler(const char *event, const char *data) {
  String sensorsToDisableData = String(data);

  bool sensorsToDisable[sensorCount];

  for (int sensor = 0; sensor < sensorCount; sensor += 1) {
    sensorsToDisable[sensor] = false;
  }

  parseActivateSystemEvent(sensorsToDisableData, sensorsToDisable);

  activateSystem(sensorsToDisable);
}

void deactivateSystemEventHandler(const char *event, const char *data) {
  deactivateSystem();
}

void triggerPanicEventHandler(const char *event, const char *data) {
  alarmSystem->triggerPanic();
}

void testSirenEventHandler(const char *event, const char *data) {
  alarmSystem->testSiren(1000);
}

void parseActivateSystemEvent(String activateSystemEvent, bool *sensorsToDisable) {
  const int len = activateSystemEvent.length();

  int sensors = 0;

  for (int c = 0; c < len; c += 1) {
    char currentChar = activateSystemEvent.charAt(c);

    if (currentChar == 'd') {
      sensorsToDisable[sensors] = true;
    }

    if (currentChar == '-') {
      sensors += 1;
    }
  }
}

void buzzerSaySomething() {
  for (int i = 0; i < 3; i += 1) {
    tone(buzzerPin, 3817, 100); // C4
    delay(200);
  }
}

// Remember to modify the grace period timer.
// TODO: Better way of computing beeps delay.
void buzzerGracePeriod() {
  int phase = 0;

  for (int tick = 1; tick <= 60; tick += 1) {
    int delayMs;

    switch (phase) {
      case 0:
        delayMs = 500; // 20 * 500 / 1000 = 10 secs
        break;
      case 1:
        delayMs = 200; // 20 * 250 / 1000 = 4 secs
        break;
      case 2:
        delayMs = 150; // 20 * 150 / 1000 = 1.5 secs
        break;
    }

    if (tick % 20 == 0) {
      phase += 1;
    }

    if (gracePeriodTimer.isActive()) {
      tone(buzzerPin, 3817, 100);
      delay(delayMs);
    } else {
      break;
    }
  }
}

void gracePeriodExpiredCallback() {
  gracePeriodExpired = true;
  alarmSystem->triggerBreach();
  tone(buzzerPin, 2024, 1000);
}

void deactivateSystemButtonInterruptCallback() {
  deactivateSystem();
}

void activateSystemButtonInterruptCallback() {
  bool sensorsToDisable[sensorCount];

  for (int sensor = 0; sensor < sensorCount; sensor += 1) {
    sensorsToDisable[sensor] = false;
  }

  activateSystem(sensorsToDisable);
}

void activateSystem(bool *sensorsToDisable) {
  if (!alarmSystem->getSystemIsActive()) {
    alarmSystem->activate(sensorsToDisable);
    notifyWithBuzzer = true;
  }
}

void deactivateSystem() {
  gracePeriodTimer.stop();
  gracePeriodExpired = false;
  alarmSystem->deactivate();
  notifyWithBuzzer = true;
}
