/******************************************************/
//       THIS IS A GENERATED FILE - DO NOT EDIT       //
/******************************************************/

#include "Particle.h"
#line 1 "/Users/gedu/Developer/alarmapp-firmware/src/alarm.ino"
#include "AlarmDriver.h"
#include "AlarmSystem.h"
#include "GracePeriodTimer.h"

void setup();
void loop();
void activateSystemEventHandler(const char *event, const char *data);
void deactivateSystemEventHandler(const char *event, const char *data);
void triggerPanicEventHandler(const char *event, const char *data);
void testSirenEventHandler(const char *event, const char *data);
void parseActivateSystemEvent(String activateSystemEvent, bool *sensorsToDisable);
void buzzerSayOk();
void buzzerSayNotOk();
void deactivateSystemButton();
void activateSystemButton();
void triggerGracePeriodExpired();
void activateSystem(bool *sensorsToDisable);
void deactivateSystem();
#line 5 "/Users/gedu/Developer/alarmapp-firmware/src/alarm.ino"
#define MAIN_DOOR_SENSOR_NUMBER 0

SYSTEM_THREAD(ENABLED);

int sensorsPins[] = { D0, D1, D2, D3, D4, D5 };
int deactivateSystemButtonPin = A0;
int activateSystemButtonPin = A1;
int sirenPin = A2;
int buzzerPin = A5;
int statusPin = D7;

AlarmDriver *alarmDriver;
AlarmSystem *alarmSystem;
GracePeriodTimer *gracePeriodTimer;

bool gracePeriodExpired;
bool breachNotified;

String previousSystemState;

int sensorCount = sizeof(sensorsPins) / sizeof(sensorsPins[0]);

unsigned long previousTimeMs;

void setup() {
  pinMode(statusPin, OUTPUT);
  pinMode(deactivateSystemButtonPin, INPUT_PULLUP);
  pinMode(activateSystemButtonPin, INPUT_PULLUP);

  gracePeriodExpired = false;
  breachNotified = false;

  previousSystemState = String("");

  alarmDriver = new AlarmDriver(sensorsPins, sensorCount, sirenPin);
  alarmSystem = new AlarmSystem(alarmDriver);
  gracePeriodTimer = new GracePeriodTimer(buzzerPin);

  previousTimeMs = millis();

  Particle.variable("systemState", previousSystemState);

  Particle.subscribe("activateSystem", activateSystemEventHandler, MY_DEVICES);
  Particle.subscribe("deactivateSystem", deactivateSystemEventHandler, MY_DEVICES);
  Particle.subscribe("triggerPanic", triggerPanicEventHandler, MY_DEVICES);
  Particle.subscribe("testSiren", testSirenEventHandler, MY_DEVICES);

  buzzerSayOk();
}

void loop() {
  if (digitalRead(deactivateSystemButtonPin) == LOW) {
    deactivateSystemButton();
  }

  if (digitalRead(activateSystemButtonPin) == LOW) {
    activateSystemButton();
  }

  bool hasBreach = alarmSystem->checkIfBreached();
  bool isPanic = alarmSystem->isPanic();

  if (hasBreach) {
    digitalWrite(statusPin, HIGH);
  } else {
    digitalWrite(statusPin, LOW);
  }

  if (hasBreach && !isPanic && !gracePeriodTimer->isActive() && !gracePeriodExpired) {
    int triggeredSensor = alarmSystem->getTriggeredSensor();

    if (triggeredSensor == MAIN_DOOR_SENSOR_NUMBER) {
      gracePeriodTimer->start(millis());
    } else {
      triggerGracePeriodExpired();
    }
  }

  if (gracePeriodTimer->isActive()) {
    gracePeriodTimer->tick(millis());
  }

  if (gracePeriodTimer->isFinished() && !gracePeriodExpired) {
    triggerGracePeriodExpired();
  }

  if ((gracePeriodExpired || isPanic) && !breachNotified) {
      if (Particle.connected()) {
        String message;

        if (isPanic) {
          message = String("panic");
        } else {
          message = String("breach");
          message.concat('|');
          message.concat(alarmSystem->getTriggeredSensor());
        }

        Particle.publish("alarmSystemTriggered", message, 60, PRIVATE);
        breachNotified = true;
      }
  }

  unsigned long currentTimeMs = millis();

  // Debounce magnetic sensors.
  unsigned long deltaBetweenEvents = (currentTimeMs - previousTimeMs);
  bool minimumTimeBetweenEventsElapsed = deltaBetweenEvents >= 1000;

  String currentSystemState = alarmSystem->getSystemState();

  if (previousSystemState != currentSystemState && minimumTimeBetweenEventsElapsed) {
    if (Particle.connected()) {
      Particle.publish("systemState", currentSystemState, 60, PRIVATE);
      previousTimeMs = currentTimeMs;
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
  tone(buzzerPin, 2024, 1000);
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

void buzzerSayOk() {
  for (int i = 0; i < 3; i += 1) {
    tone(buzzerPin, 3817, 100); // C4
    delay(200);
  }
}

void buzzerSayNotOk() {
  tone(buzzerPin, 956, 500);
  delay(500);
}

void deactivateSystemButton() {
  deactivateSystem();
}

void activateSystemButton() {
  bool sensorsToDisable[sensorCount];

  for (int sensor = 0; sensor < sensorCount; sensor += 1) {
    sensorsToDisable[sensor] = false;
  }

  activateSystem(sensorsToDisable);
}

void triggerGracePeriodExpired() {
  gracePeriodExpired = true;
  alarmSystem->triggerBreach();
  tone(buzzerPin, 2024, 1000);
}

void activateSystem(bool *sensorsToDisable) {
  if (alarmSystem->isReadyToActivate(sensorsToDisable)) {
    alarmSystem->activate(sensorsToDisable);
    buzzerSayOk();
  } else {
    buzzerSayNotOk();
  }
}

void deactivateSystem() {
  gracePeriodTimer->reset();
  gracePeriodExpired = false;
  breachNotified = false;
  alarmSystem->deactivate();
  buzzerSayOk();
}
