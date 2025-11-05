#include "ServoController.h"

ServoController::ServoController(int pin) : servoPin(pin) {}

void ServoController::initialize() {
    motor.attach(servoPin);
    closeLock(); // Inicia cerrado
}

void ServoController::openLock() {
    motor.write(180);
    Serial.println("🔓 Cerradura abierta");
    delay(1000);
}

void ServoController::closeLock() {
    motor.write(0);
    Serial.println("🔒 Cerradura cerrada");
    delay(1000);
}