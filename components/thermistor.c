#include "thermistor.h"
#include <math.h>


//Funcion: Calcular la resistencia  a partir de la tension del termistor
double calculateTemperature(double voltage){


   //Calcular la resistencia del termistor usando un divisor de tension
   double resistance = RESISTANCE*voltage/(3.3-voltage);

  //Ecuacion para  calculo temperatura  (SteihHart-Hart)
   double temperature = 1/(1/(273.15+25)-(log(NOMINAL_RESISTANCE/resistance)/B));
   
// Convertir tempertura   de  K° a °C
   temperature -=273.15;

   return temperature;
}




