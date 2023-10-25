// Elemenetos del HTML
const uartData = document.getElementById('uart-data');
const temperatureData = document.getElementById('temperature-data');
const ledCircle = document.getElementById('led-circle');
const setTimeButton = document.getElementById('set-time');
const valueInput = document.getElementById('value-input');
const sendBrihtnessButton = document.getElementById('send-value-brightness');
var sendCredentialsButton = document.getElementById("send-credentials");
var ssdiInput =document.getElementById("ssdi-input");
var passwordInput = document.getElementById("value-password");

//Recibe e impirme los datos que se incresaron en los credenciales
sendCredentialsButton.addEventListener("click", function() {

    var ssdiValue = ssdiInput.value;
    var passwordValue = passwordInput.value;

    console.log("Value of the ssdi; "+ ssdiValue);
    console.log("Value of Password: "+ passwordValue);
})

//Recive e imprime las veces que se oprimio el boton
setTimeButton.addEventListener("click", function (){
    console.log("print increase");

})

async function getTemperature() {
  const response = await fetch("api/temperature");
  const res = await response.json();
  return res.temperature;
}


// Recive el valor para modificar la intensidad de LED
sendBrihtnessButton.addEventListener("click", function(){
    var brightnessValue = valueInput.value

    console.log("Value of Brigthness: "+ brightnessValue);


})



//  actualizar información en la UART
function updateUARTInfo(data) {
    uartData.textContent = data;
}

// Función para actualizar temperatura y color del LED
function updateTemperatureAndLED(temperature) {
    temperatureData.textContent = temperature + ' °C';
    
    // Cambiar el color del LED en función de la temperatura
    if (temperature < 30) {
        ledCircle.style.backgroundColor = 'blue';
    } else if (temperature < 50) {
        ledCircle.style.backgroundColor = 'green';
    } else {
        ledCircle.style.backgroundColor = 'red';
    }
}

//  datos UART  mostrados
setInterval(async () => {
    const temperature = await getTemperature();
    const simulatedUARTData = 'hola hola';
    updateUARTInfo(simulatedUARTData);
    updateTemperatureAndLED(temperature);
}, 5000);  // Actualiza cada 5 segundos (




