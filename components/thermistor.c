#include "thermistor.h"
#include <math.h>



double calculateTemperature(double voltage){

   double resistance = RESISTANCE*voltage/(3.3-voltage);

  //Ecuacion para  calculo temperatura
   double temperature = 1/(1/(273.15+25)-(log(NOMINAL_RESISTANCE/resistance)/B));
   
// Convertir tempertura  °C
   temperature -=273.15;

   return temperature;
}




