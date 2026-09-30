#!/usr/bin/env bash
set -euo pipefail
source /etc/os-release
if [[ "${ID:-}" != ubuntu || "${VERSION_ID:-}" != 24.04 ]]; then
  echo 'This script requires Ubuntu 24.04 for ROS 2 Jazzy.' >&2
  exit 1
fi
sudo apt-get update
sudo apt-get install -y locales curl software-properties-common
sudo locale-gen en_US en_US.UTF-8
sudo update-locale LC_ALL=en_US.UTF-8 LANG=en_US.UTF-8
export LANG=en_US.UTF-8
sudo add-apt-repository -y universe
# Official ROS apt-source package from the ROS infrastructure project.
task_version=$(curl -fsSL https://api.github.com/repos/ros-infrastructure/ros-apt-source/releases/latest | python3 -c 'import json,sys; print(json.load(sys.stdin)["tag_name"])')
task_deb=$(mktemp --suffix=.deb)
trap 'rm -f "$task_deb"' EXIT
curl -fL "https://github.com/ros-infrastructure/ros-apt-source/releases/download/${task_version}/ros2-apt-source_${task_version}.noble_all.deb" -o "$task_deb"
sudo dpkg -i "$task_deb"
sudo apt-get update
sudo apt-get install -y ros-jazzy-desktop ros-dev-tools ros-jazzy-rqt-plot
echo 'ROS installed. Source /opt/ros/jazzy/setup.bash and follow documents/setup.md.'
