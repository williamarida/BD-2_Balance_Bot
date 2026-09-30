# BD-2 development setup

## Open the project

Local project: `C:\Users\wrari\Documents\Robotics Projects\BD-2 Balance-Bot`.
Open `BD-2.code-workspace` in VS Code. Work in the **ESP32 firmware** entry for ESP-IDF commands.
The root entry is for docs, pictures, and Git.

Toolchain: ESP-IDF **5.5.2**, original **esp32** target, Bluepad32 pinned in the submodule.
VS Code: ESP-IDF, C/C++, and Remote WSL extensions. Existing Git installation is reused.

Windows compiler tools can have trouble with spaces in paths. The setup creates a directory junction
at `C:\Espressif\projects\bd2` pointing to the real project folder. It is an alternate path to the same
files, not a duplicate checkout. Build output goes under `C:\Espressif\build\bd2`.

## Firmware build and USB upload

From PowerShell, after opening the project:

```powershell
. 'C:\Espressif\tools\Microsoft.v5.5.2.PowerShell_profile.ps1'
Set-Location C:\Espressif\projects\bd2\Code\firmware
idf.py -B C:\Espressif\build\bd2 build
# Once the ESP32 is available, replace COM5 with its actual serial port:
idf.py -B C:\Espressif\build\bd2 -p COM5 flash monitor
```

Use `Ctrl+]` to exit the serial monitor. No GPIO pins are driven by this bench firmware.
Connect the board with a USB data cable. Identify its port in Device Manager. Install a CP210x driver
only if Windows has not recognized the board. Use BOOT/RESET if automatic flashing does not work.
Do not erase flash as a routine troubleshooting step; it removes saved settings and pairing keys.

## Wi-Fi preparation

Copy `Code/firmware/main/wifi_credentials.example.h` to `wifi_credentials.h` and enter a 2.4 GHz
network name/password locally. The credential file is ignored by Git. Rebuild and upload once.
Run `idf.py -B C:\Espressif\build\bd2 reconfigure` after first adding or removing the credential file.
Without this file, firmware runs Bluetooth/serial diagnostics and skips Wi-Fi.

Read the ESP32 IP address from serial output. PC requests go to UDP port 4242; replies return to
the same PC socket. This request/reply design helps with WSL NAT, but real network testing is pending.
The bench protocol has no authentication: use your private development network and no port forwarding.

```powershell
python tools\robot_cli.py --host ESP32_IP get
python tools\robot_cli.py --host ESP32_IP set gain_kp 25
python tools\robot_cli.py --host ESP32_IP save
```

If `python` is not on PATH, use the installed interpreter:
`C:\Espressif\tools\python\v5.5.2\venv\Scripts\python.exe`.
The settings are examples. They do not control motors or change the balance behavior yet.

## PS4 pairing

With diagnostic firmware running, hold **SHARE + PS** until the DualShock light flashes. Firmware
scans for a controller and retains pairing keys. This requires the original ESP32 Bluetooth Classic
hardware. Pairing and axis/button telemetry require the actual board/controller and are unverified.

## ROS 2 on Windows through WSL2

WSL has been installed on this PC, but a Windows restart is still needed before continuing. After
restarting, run `wsl --install -d Ubuntu-24.04 --no-launch`, then open Ubuntu to create your username/password.

For reinstalling or setting up another PC: Windows requires administrator privileges. Run `tools/install-wsl.ps1` from an
administrator PowerShell. It installs the already-downloaded Microsoft-signed MSI, enables the two
required Windows features without restarting, and stops if a reboot is required. After restarting
Windows yourself, run `wsl --install -d Ubuntu-24.04 --no-launch` and open Ubuntu to create your Linux
username/password. Then verify `wsl --list --verbose` shows version 2.

Inside Ubuntu 24.04:

```bash
cd '/mnt/c/Users/wrari/Documents/Robotics Projects/BD-2 Balance-Bot'
bash tools/install-ros2.sh
source /opt/ros/jazzy/setup.bash
cd Code/ros2_ws
colcon build --symlink-install --build-base "$HOME/bd2-build" --install-base "$HOME/bd2-install"
source "$HOME/bd2-install/setup.bash"
ros2 run bd2_bridge bridge --ros-args -p robot_host:=127.0.0.1
```

For the no-hardware demo, run `python3 tools/mock_robot.py` in another Ubuntu terminal first.
For the real robot, replace `127.0.0.1` with its IP. WSL ROS runtime/build verification is pending
until WSL and Ubuntu are installed. The bridge does not require micro-ROS or another onboard computer.

In additional terminals, source both ROS and the workspace setup files before these commands:

```bash
ros2 topic echo /bd2/telemetry
ros2 topic echo /bd2/command_reply
ros2 topic pub --once /bd2/command std_msgs/msg/String '{data: "{\"op\":\"set\",\"key\":\"gain_kp\",\"value\":25}"}'
ros2 run rqt_plot rqt_plot /bd2/gain_kp/data /bd2/stick_x/data /bd2/stick_y/data
ros2 bag record -o "$HOME/bd2-recordings/session-01" /bd2/telemetry /bd2/command_reply /bd2/link_connected
```

## PC-only tests without ROS

```powershell
python tools\test_protocol.py
# In one terminal:
python tools\mock_robot.py
# In another:
python tools\robot_cli.py get
python tools\robot_cli.py set gain_kp 25
python tools\robot_cli.py get
```

The mock validates communication and setting changes, not robot physics or controller timing.

## Sources

- https://docs.espressif.com/projects/vscode-esp-idf-extension/en/latest/installation.html
- https://bluepad32.readthedocs.io/en/latest/plat_esp32/
- https://bluepad32.readthedocs.io/en/latest/FAQ/
- https://learn.microsoft.com/en-us/windows/wsl/install
- https://docs.ros.org/en/jazzy/Installation/Ubuntu-Install-Debs.html
