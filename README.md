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
| activateSystem       | When this event is published the system activates. | `1d-2e-3e` means disable area 1 but keep areas 2 and 3 enabled. |
| deactivateSystem     | When this event is published the system deactivates. | No data. |
| triggerPanic         | When this event is published the system enters in panic mode (siren gets instantly activated). | No data. |
| testSiren            | When this event is published the system rings the siren for 3 seconds. | No data. |
| alarmSystemTriggered | When the system is breached, this event is published and a push notifications webhook gets invoked. | If panic, then `panic`. If breach, then `breach\|3` means breach through area 3. |
| systemState          | When the system detects a state change, e.g., a windows opens, this event is published | The state of the system. E.g., `0d-1e-0d-0d\|0\|1` means first area is disabled and inactive (e.g. some window from area 1 is open). Siren is off and system is active. |

### Building and flashing the firmware

Use the Particle IDE. I prefer the [desktop version](https://docs.particle.io/guide/tools-and-features/dev/).
