#include "SoundDetector.h"
#include <Arduino.h>

SoundDetector::SoundDetector(int pin, int threshold, int maxTimeBetweenPeaks, 
                             int requiredPeaks, MQTTManager& mqttManager)
    : pin(pin), threshold(threshold), maxTimeBetweenPeaks(maxTimeBetweenPeaks),
      requiredPeaks(requiredPeaks), mqttManager(mqttManager),
      lastPeakTime(0), peakCount(0), 
      intentoEnviado(false), tiempoUltimoIntento(0), tiempoParaOff(10000) {} // 10s

void SoundDetector::monitor() {
  int value = analogRead(pin);
  unsigned long now = millis();

  // DEBUG temporal para calibrar
  // Serial.println(value);

  if (value > threshold) {
    if (now - lastPeakTime < maxTimeBetweenPeaks) {
      peakCount++;
    } else {
      peakCount = 1;
    }
    lastPeakTime = now;

    if (peakCount >= requiredPeaks && !intentoEnviado) {
      Serial.println("Patrón válido detectado, enviando intento...");
      mqttManager.publish("INTENTO_DE_APERTURA");
      tiempoUltimoIntento = now;
      intentoEnviado = true;
      peakCount = 0;
    }
  }

  if (intentoEnviado && (now - tiempoUltimoIntento >= tiempoParaOff)) {
    Serial.println("Enviando OFF automáticamente después de tiempo de espera");
    mqttManager.publish("OFF");
    intentoEnviado = false;
  }

  delay(40); // antes era 100
}
