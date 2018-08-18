#include "AlarmDriver.h"
#include "AlarmSystem.h"

int sensorsPins[] = { D0, D1, D2, D3, D4, D5 };
int sirenPin = D6;
int statusPin = D7;

AlarmDriver *alarmDriver;
AlarmSystem *alarmSystem;

String previousSystemState;

bool isActive;

int sensorCount = sizeof(sensorsPins) / sizeof(sensorsPins[0]);

void setup() {
  pinMode(statusPin, OUTPUT);

  alarmDriver = new AlarmDriver(sensorsPins, sensorCount, sirenPin);
  alarmSystem = new AlarmSystem(alarmDriver);

  Particle.variable("systemState", previousSystemState);

  Particle.subscribe("activateSystem", activateSystemEventHandler, MY_DEVICES);
  Particle.subscribe("deactivateSystem", deactivateSystemEventHandler, MY_DEVICES);
}

void loop() {
  alarmSystem->alertIfBreach();

  isActive = alarmSystem->getSystemIsActive();

  if (isActive) {
    digitalWrite(statusPin, HIGH);
  } else {
    digitalWrite(statusPin, LOW);
  }

  String currentSystemState = alarmSystem->getSystemState();

  if (previousSystemState != currentSystemState) {
    if (Particle.connected()) {
      Particle.publish("systemState", currentSystemState, 60, PRIVATE);

      delay(5000);
    }

    previousSystemState = currentSystemState;
  }
}

void activateSystemEventHandler(const char *event, const char *data) {
  String sensorsToDisableData = String(data);

  bool sensorsToDisable[sensorCount];

  for (int sensor = 0; sensor < sensorCount; sensor++) {
    sensorsToDisable[sensor] = false;
  }

  parseActivateSystemEvent(sensorsToDisableData, sensorsToDisable);

  alarmSystem->activate(sensorsToDisable);

  isActive = true;
}

void deactivateSystemEventHandler(const char *event, const char *data) {
  alarmSystem->deactivate();
  isActive = false;
}

void parseActivateSystemEvent(String activateSystemEvent, bool *sensorsToDisable) {
  const int len = activateSystemEvent.length();

  int sensors = 0;

  for (int c = 0; c < len; c+=1) {
    char currentChar = activateSystemEvent.charAt(c);

    if (currentChar == 'd') {
      sensorsToDisable[sensors] = true;
    }

    if (currentChar == '-') {
      sensors += 1;
    }
  }
}
