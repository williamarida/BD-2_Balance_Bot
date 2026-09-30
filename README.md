Intro:  
This is BD-2, my wheeled, bipedal robot! 
Built with the balance of cost, modularity, and straightforward design in mind.
This repository is built as both a story and an instruction manual to help anyone build one and learn
as much as I did in the process of deigning and building this system.

Electrical System:  
ESP32, C++ based system running on a 12V battery for motor power, and a 5V buck converter for small
electronics. This project does not currently include a specialty PCB to maintain modularity and allow
users to build their own robot with this system as a foundation without the need to disect and
understand the PCB schematic.

Mechanical System:  
2 legs with 2 digital servo hip motors and 2 BLDC gimbal motors. 3D printed body and components,
mounted together using M3 screws and heat set threated inserts. The leg mechanism uses a pulley system
instead of a 4 bar linkage to allow for perfectly linear vertical foot movement without the use of a
second set of BLDC motors for the knees. This cuts cost as well as provides challenge in design.

Software System:  
C++ running ROS 2 over Wi-Fi to a paired computer to allow for telemetry graphing, and PID and
Bluetooth controller tuning.
