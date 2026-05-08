# Line-Following-Robot-with-obstacle-avoidance
A line-following robot with obstacle avoidance is an autonomous mobile system designed to track a predefined path (usually a black line on a white surface) while detecting and maneuvering around obstructions in its way. It combines navigation intelligence with collision safety, making it suitable for applications like warehouse automation, smart transport, and automated cleaning.

**Core Components**

**Microcontroller:** The "brain" of the robot (e.g., Arduino Uno, STM32) processes data from sensors and controls motor actions.
Link: https://shorturl.at/NsglK

**Line Sensors:** Typically 2-3 Infrared (IR) sensors (e.g., TCRT5000) mounted at the front-bottom to detect the high-contrast difference between the line and the background surface.

Link: https://shorturl.at/TM7y4

**Obstacle Sensor:** An Ultrasonic Sensor (HC-SR04) or IR proximity sensor mounted at the front to measure the distance to objects.
Link: https://shorturl.at/GpABu

**Motor Driver:** An H-Bridge driver (e.g., L298N or L293D) acts as an interface to control the speed and direction of DC motors based on signals from the microcontroller.

Link: https://shorturl.at/OyyWW

**Chassis & Motors:** A lightweight structure with 2 or 4 DC geared motors, wheels, and a caster wheel for maneuverability
Link: https://shorturl.at/TetCy
