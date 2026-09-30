# BD-2 Balance Bot

BD-2 is my wheeled, bipedal robot: a hands-on project in mechanical design, feedback control,
electronics, and iterative prototyping. The goal is to balance and steer on two wheels, with
servo-driven legs supporting future experiments in banked turning and jumping.

This repository is both a project scrapbook and a developing build guide. Like my
[engineering portfolio](https://williamarida.github.io/projects/bipedal_robot/bipedal_robot_index.html),
it brings together the design story, supporting documents, photos, and practical work.

## Current progress

Mechanical concepts and first-iteration CAD are in development. Electrical subsystems are being
selected and tested. The PC development environment and diagnostic firmware are prepared;
balance control and full robot integration are still ahead.

**The current firmware is for bench diagnostics only.** It reports controller inputs and accepts
example settings. It does not drive motors, balance, play sounds, or apply joystick settings to motion.

See the [progress log](documents/progress.md) and [setup checklist](documents/setup-status.md).

## Explore the project

| Area | Start here |
| --- | --- |
| Mechanical design | [Legs, joints, body, and CAD](documents/mechanical-design.md) |
| Electrical system | [Power, sensing, and actuation](documents/electrical-system.md) |
| Software and controls | [Architecture and implementation status](documents/architecture.md) |
| Development setup | [VS Code, firmware builds, Git, and ROS](documents/setup.md) |
| Photos and sketches | [Image collection](images/README.md) |
| Sharing hardware files | [Hardware exports](hardware/README.md) |

## Repository layout

```text
BD-2 Balance-Bot/
  README.md               Project overview and navigation
  BD-2.code-workspace      Open this in VS Code
  Code/
    firmware/             ESP32 C++ diagnostic firmware
    ros2_ws/              ROS 2 telemetry and settings bridge
    gyro_graphing_kalman_filter/  Original Arduino experiment
    lights_and_sounds/            Original Arduino experiment
    motors_and_control/          Original Arduino experiment
  documents/              Design notes, setup guides, and progress
  images/                 Selected project photos and design sketches
  hardware/               Shareable CAD exports, wiring, and BOM files
  tools/                  Setup scripts and PC-only communication tools
  CAD/V1 CAD/                 Existing local CAD work; excluded from Git
```

This repo covers one robot, so it does not need the portfolio's extra `projects/` level.
Documents and images sit beside the code, and each design note links to its supporting images.
Original CAD files stay in their existing location. Put selected exports in `hardware/` when ready.

## Next steps

1. Restart Windows, then finish Ubuntu and ROS 2 setup.
2. Upload diagnostic firmware when the ESP32 is available.
3. Verify DualShock pairing, Wi-Fi telemetry, and settings persistence.
4. Confirm the devices and wiring before implementing balance and actuator control.

## Working locally and with GitHub

Open `BD-2.code-workspace` in VS Code. Use the firmware entry for ESP-IDF commands and the
project entry for browsing documents, images, and Git changes. [Build commands](documents/setup.md)
and [pending checks](documents/setup-status.md) are kept with the project.

Git does not upload files automatically. Changes remain local until reviewed, committed, and pushed.

```powershell
git status
git diff
# Select and stage the files you want to publish, then:
git commit -m "Describe the changes"
git fetch origin
# Review any newer GitHub changes before merging/rebasing and pushing.
git push origin HEAD
```

Bluepad32 is a pinned submodule. For a fresh clone, activate an ESP-IDF terminal and run
`tools/prepare-bluepad32.ps1`. Its required BTstack patch can make the dependency appear modified;
do not discard that patch or commit generated dependency sources.
