# Smart Elevator 

## Overview
This project implements a **Smart Elevator System** using an **ATmega32 microcontroller**. The system integrates various sensors and modules to automate the control of the elevator, providing features such as temperature regulation, smoke detection, and floor navigation based on user input via a mobile app.

### Features:
- **Temperature Control**: Uses an **LM35 temperature sensor** to measure the temperature inside the elevator. If the temperature exceeds 35°C, a fan is automatically turned on to cool the environment.
- **Smoke Detection**: Equipped with a smoke sensor to detect the presence of smoke inside the elevator. In case of smoke, the system activates a fan for ventilation.
- **Floor Navigation**: The user can select the desired floor through a mobile app. **IR sensors** detect the current floor, and the elevator motor moves to the selected floor.
  
## Components Used

- Microcontroller: ATmega32
- Temperature Sensor: LM35 (for temperature monitoring inside the elevator)
- Smoke Sensor: Smoke sensor (for detecting smoke and ensuring safety)
- IR Sensors: Used to detect the current level of the elevator.
- Bluetooth Module: For communication between the elevator system and a mobile app, allowing users to send the desired floor.
- Motor: To drive the elevator to different floors.    
- Fan: Turned on in case of high temperature or smoke detection.

## How It Works

1. Temperature Control:
    - The LM35 sensor reads the temperature inside the elevator cabin.
    - If the temperature is higher than 35°C, the fan is turned on.
    - Once the temperature drops below 35°C, the fan is turned off.

2. Smoke Detection:
    - The smoke sensor constantly monitors the air quality inside the elevator.
    - If smoke is detected, the fan is activated to ventilate the cabin.

3. User Floor Selection:
    -  The user sends the desired floor via a mobile app using a Bluetooth connection.
    -  The system receives the requested floor and drives the motor to move the elevator.
    -  IR sensors placed on each floor detect the elevator’s current level, allowing the system to stop at the correct floor.

## Demonstration Video 
https://github.com/user-attachments/assets/b3ec10b1-d72b-4cb5-abd3-abf7bf0bac45
