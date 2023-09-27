#include "thermistor.h"
#include <math.h>



double calculateTemperature(double voltage){

   double resistance = 100*voltage/(3.3-voltage);

  //Ecuacion para  calculo temperatura
   double temperature = 1/(1/(273.15+25)-(log10(NOMINAL_RESISTANCE/resistance)/B));
   
// Convertir tempertura  °C
   temperature -=273.15;

   return temperature;
}




