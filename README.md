# 🚀Smart-IoT-Parcel-Delivery-Chamber
Receiving online deliveries when you're away from home can be risky or inconvenient. My solution? A smart delivery chamber that allows authorized delivery and secure retrieval using automation, sensors, and IoT.


## 🔍 Problem Statement
Ever missed an important parcel because you weren’t home?
Relying on neighbors or risking doorstep theft isn't a reliable solution.

## 💡 The Solution
I designed and built a Smart Parcel Chamber that securely handles deliveries even when the recipient isn’t home. It’s powered by IoT, automation, and a bit of creativity!


## ⚙️ Working Process
Our Smart Parcel Delivery Chamber automates secure parcel drop-off and pickup using sensors, motors, and a microcontroller. Here's how it works, step-by-step:
### ✅ 1. Delivery Access
    The delivery agent enters a common passcode on a 4x4 keypad.
    If correct:
      - The system unlocks the chamber using a servo motor (can be replaced with a solenoid lock for improved security).
      - A stepper motor opens the door automatically.
### 📦 2. Parcel Placement Detection
    Once the door is open, an ultrasonic sensor begins measuring the distance inside the chamber.
    When a parcel is detected (within 15 cm range):
      - The system waits a few seconds to allow proper placement.
      - Then it closes the door using the stepper motor.
      - The lock is re-engaged.
      - A random 4-digit OTP is generated.
      - This OTP is sent to the owner via Telegram.
### 🔓 3. Owner Parcel Retrieval
    The owner returns home and enters the OTP on the keypad.
    If the OTP matches:
      - The system unlocks the chamber.
      - Door opens automatically.
### 🔁 4. Auto Reset After Pickup
    After a few seconds, the ultrasonic sensor checks if the parcel has been removed.
    If the chamber is empty:
     - The door closes automatically.
     - The lock engages again.
     - The OTP is reset to default (0000).
     - System is ready for the next delivery.

## 🔐 Tech Stack
    ✅ ESP32
    ✅ Ultrasonic Sensor
    ✅ Servo + Stepper Motor
    ✅ 4x4 Keypad
    ✅ Telegram Bot for OTP
    
## 🔧 Optional Add-on Features (Future Expansion)
    These features enhance the system's functionality and security:
### 📸 Camera Integration
    - Add an ESP32-CAM or compatible USB camera to:
    - Capture images when a parcel is placed
    - Record short video clips
    - Save or upload files to the cloud or send to the owner
### ✉️ Notification Methods
    In addition to Telegram:
    - 📲 SMS Alerts using GSM modules (e.g., SIM800L)
    - 📧 Gmail Notifications using SMTP (send parcel image + OTP via email)
    This ensures the owner gets real-time updates, even without Telegram access.
### 📱 Future Interface Options
    - OLED or LCD for password feedback
    - Mobile App for remote monitoring
    - Cloud integration for delivery logs and status history

