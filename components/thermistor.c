#include "thermistor.h"
#include <math.h>



double calculateTemperature(double resistance){
   double inverseRes = 1.0 / resistance;

  //Ecuacion para  calculo temperatura
   double  logInverseRes = long(inverseRes);
double temperature = 1.0 / (A_COEFFICIENT + B_COEFFICIENT * logInverseRes + C_COEFFICIENT * logInverseRes * logInverseRes * logInverseRes);
   
// Convertir tempertura  °C
   temperature -=273.15;

   return temperature;

}




