# Alarmapp Firmware

Firmware for the Alarmapp project written for the Particle Photon.

### Features

- Watch for breaches and notify.
- Realtime system state updates.
- Disable areas (disable sensors).
- Panic (siren gets instantly activated).
- Siren test.
- Audio feedback via a buzzer.
- Can work with or without the network (code uses own thread).

### TODO
- Add a matrix keypad for offline activation/deactivation of the system.

### Project organization

| Entity                    | Description        |
|---------------------------|--------------------|
| AlarmDriver               | Talks with the hardware (reads sensors and siren state, activates or deactivates siren). |
| AlarmSystem               | Holds system state (is breached, is panic, is activated, is deactivated, etc.). |
| GracePeriodTimer          | This timer provides a 15 second siren activation delay. If the timer is finished, then the system is triggered and a `alarmSystemTriggered` event is published. Also, it beeps a buzzer for feedback. |
| Alarm.ino                 | Setups the system, exposes system state, functions, and variables to the Particle Cloud, handles Particle Cloud events, etc. |

### Events

| Name | Description | Data |
|------|-------------|-------|
| activateSystem       | When this event is published the system activates. | No data. |
| deactivateSystem     | When this event is published the system deactivates. | No data. |
| triggerPanic         | When this event is published the system enters in panic mode (siren gets instantly activated). | No data. |
| testSiren            | When this event is published the system rings the siren for 3 seconds. | No data. |
| alarmSystemTriggered | When the system is breached, this event is published and a push notifications webhook gets invoked. | If panic then `panic` if breach then `breach\|3` sensor 3 activated (e.g. breach through window 3). |
| systemState          | When the system detects a state change, e.g., a windows opens, this event is published | The state of the system. E.g., `0d-1e-0d-0d\|0\|1` means first sensor is inactive (e.g. window is open) and disabled. Siren is off and system is active. |

### Building and flashing the firmware

Use the Particle IDE. I prefer the [desktop version](https://docs.particle.io/guide/tools-and-features/dev/).
