#ifndef SERVO_CONTROLLER_H
#define SERVO_CONTROLLER_H

#include <ESP32Servo.h>

class ServoController {
private:
    int servoPin;
    Servo motor;

public:
    ServoController(int pin);
    void initialize();
    void openLock();
    void closeLock();
};

#endif