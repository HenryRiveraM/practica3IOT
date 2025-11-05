#ifndef SOUND_DETECTOR_H
#define SOUND_DETECTOR_H

#include "MQTTManager.h"

class SoundDetector {
private:
    int pin;
    int threshold;
    unsigned long lastPeakTime;
    int peakCount;
    const int maxTimeBetweenPeaks;
    const int requiredPeaks;
    MQTTManager& mqttManager;

    // Nuevas variables para control de OFF automático
    bool intentoEnviado;
    unsigned long tiempoUltimoIntento;
    unsigned long tiempoParaOff; // Tiempo de espera para OFF (en ms)

public:
    SoundDetector(int pin, int threshold, int maxTimeBetweenPeaks, 
                 int requiredPeaks, MQTTManager& mqttManager);

    void monitor();

    // Opcional: para que puedas cambiar el tiempo dinámicamente si quieres
    void setTiempoParaOff(unsigned long tiempo) { tiempoParaOff = tiempo; }
};

#endif
