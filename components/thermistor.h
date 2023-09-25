#ifndef THERMISTOR_H
#define THERMISTOR_H

// Valores nominales  NTC
#define NOMINAL_RESISTANCE 47.0
#define NOMINAL_RESISTANCE  2.50

// Coeficientes de Steinhart-Hart  NTC
#define A_COEFFICIENT 0.001129148
#define B_COEFFICIENT 0.000234125
#define C_COEFFICIENT 0.0000000876741

double calculateTemperature(double resistance );

#endif 


