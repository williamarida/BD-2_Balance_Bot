# Architecture and scope

## Normal operation (planned)

DualShock 4 -> Bluepad32 -> joystick shaping -> speed/turn setpoints -> local control loops -> BLDC wheel drivers.
IMU/encoders -> local estimation/control. Servo legs, lights, and sounds use non-blocking event handlers.
The ESP32 boots from stored firmware/settings; neither a PC nor ROS is required to balance or use the controller.

The PC runs ROS 2 for graphs, recordings, and configuration when connected. The proposed custom Wi-Fi
bridge is an alternative to micro-ROS, not an additional mandatory layer.

## Implemented preparation

- C++ ESP-IDF bench firmware with Bluepad32 and optional Wi-Fi.
- Controller axis/button snapshots copied under a short lock; Bluetooth callbacks avoid blocking effects.
- UDP JSON get/set/save protocol on port 4242, matched request identifiers and explicit acknowledgements.
- Example settings: gain_kp [0,1000], stick_deadzone [0,0.5], stick_scale [0,1].
- Example settings applied in RAM; explicit save uses NVS flash; settings loaded after restart.
- Python ROS bridge publishes telemetry/numeric plots and accepts set/save commands.
- PC mock and direct CLI for testing without hardware/ROS.

No actuator outputs, balance loop, sensor estimation, applied joystick shaping, sound scheduling, or OTA
update endpoint exists yet. The chosen limits are for diagnostics, not validated robot tuning limits.

## Next implementation

Once hardware is identified, replace the diagnostic example with deterministic control tasks. The joystick
requests limited speed and turn rate, not unrestricted PWM. Apply complete settings snapshots between
control iterations. Validate ranges, ramp potentially disruptive changes, and keep flash writes/OTA out
of active balancing. Log timing while Bluetooth and Wi-Fi run together.

Choose loss-of-controller behavior: ramp motion targets to zero while continuing balance when feasible;
disable drive on unrecoverable faults. PC disconnection must not prevent standalone operation.

Telemetry should add timestamped tilt, tilt rate, wheel speeds, actuator commands, battery voltage, and
loop timing. Actual sample/report rates must be measured. Button mappings and motion limits can become
live settings after implementation; currently only raw buttons/axes are reported.

OTA comes after reliable USB recovery, and requires a suitable flash partition table. The current bench
firmware uses a custom 2 MiB single-application partition on the assumed 4 MiB flash and cannot receive firmware wirelessly.
Wi-Fi IRAM speed optimizations are disabled to leave instruction memory available for Bluetooth;
actual throughput and balance-loop timing still need hardware measurements.
