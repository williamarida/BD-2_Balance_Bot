# Setup status and bookmarks

Prepared on 2026-09-30. This is a checklist to resume, not a scheduled reminder.

## PC and repository

- [x] Read GitHub README/history and the existing portfolio context.
- [x] Locate original Code sketches and CAD; preserve them.
- [x] Verify existing VS Code and Git.
- [x] Install ESP-IDF, C/C++, and WSL VS Code extensions.
- [x] Install ESP-IDF 5.5.2 and compiler tools.
- [x] Create bench firmware, mock transport, ROS workspace, and setup instructions.
- [x] Compile Bluetooth-only and Bluetooth + Wi-Fi bench firmware; validate application size.
- [x] Configure repository-local Git author name/email from the user-provided identity.
- [x] Install Microsoft WSL package using administrator setup.
- [ ] Restart Windows to activate virtualization features; install Ubuntu 24.04 and create its user.
- [ ] Install ROS 2 Jazzy and validate the bridge/plots inside Ubuntu.

## Bookmark: step 4 — ESP32 USB

- [ ] Connect USB data cable; identify chip, flash capacity, and COM port.
- [ ] Check/install CP210x driver only if needed.
- [ ] Upload bench firmware; confirm startup serial output.
- [ ] Verify reset and recovery flashing.

## Bookmark: step 6 — Wi-Fi telemetry/tuning

- [ ] Enter local Wi-Fi credentials; rebuild/upload.
- [ ] Verify IP, request/reply transport, and PC/WSL network access.
- [ ] Set an example value without rebooting and read it back.
- [ ] Save, power-cycle, and verify settings persistence.
- [ ] Verify normal firmware operation with PC disconnected.

## Bookmark: step 7 — DualShock 4

- [ ] Pair SHARE + PS; verify reconnect after restart.
- [ ] Verify axis ranges and button bit mappings.
- [ ] Test simultaneous Bluetooth/Wi-Fi timing before introducing motors.
- [ ] Implement and test dead zones, response curves, motion ramps, and disconnect handling.

## Hardware integration later

- [ ] Identify exact IMU, motor drivers, motors/feedback, servos, lighting, and audio hardware.
- [ ] Verify wiring and power, then assign GPIOs.
- [ ] Implement balance/estimation and non-blocking lights/sounds.
- [ ] Build validated gain/input tuning UI.
- [ ] Implement OTA once USB recovery and flash partitions are settled.

## Verification notes

- GitHub base: `2190314547ff899dff0e050ab79fabb31cb31f16`; origin points to williamarida/BD-2_Balance_Bot.
- Original Arduino sketches and V1 CAD preserved; no remote push performed.
- VS Code extensions verified: espressif.esp-idf-extension, ms-vscode.cpptools, ms-vscode-remote.remote-wsl.
- ESP-IDF 5.5.2 installed at C:\Espressif\v5.5.2\esp-idf.
- Python UDP tests: four passing checks covering invalid values, setting/readback, offline errors, and stale replies.
- Python files compile and ROS package XML parses; ROS itself has not run yet.
- Bluetooth diagnostic firmware compiled. Bluetooth + Wi-Fi firmware compiled at 1,208,416 bytes;
  the custom 2 MiB application partition has about 42% free space. Actual flash capacity is still unverified.
- Wi-Fi speed optimization in IRAM disabled to fit Bluetooth and Wi-Fi together. Runtime timing unverified.
- WSL package installed. WSL2 currently cannot start; reboot is pending after feature enablement.
- Processor reports VirtualizationFirmwareEnabled=True. If WSL2 still fails after restart, diagnose Windows features next.
- Portfolio repo: C:\Users\wrari\Documents\Codex\2026-06-17\i-need-you-to-connect-directly\work\williamarida.github.io.
