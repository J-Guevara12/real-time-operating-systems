
// Elementntos del HTML
const uartData = document.getElementById('uart-data');
const temperatureData = document.getElementById('temperature-data');
const ledCircle = document.getElementById('led-circle');
const setTimeButton = document.getElementById('set-time');
const valueInput = document.getElementById('value-input');
const sendValueButton = document.getElementById('send-value');

// Función para actualizar información UART
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

//  datos UART 
setInterval(() => {
    const simulatedTemperature = Math.floor(Math.random() * 30) + 20;
    const simulatedUARTData = 'hola hola';
    updateUARTInfo(simulatedUARTData);
    updateTemperatureAndLED(simulatedTemperature);
}, 5000);  // Actualiza cada 5 segundos (

// Boton que aumenta la  frecuencia
setTimeButton.addEventListener('click', () => {

});

// Recive el valor para modificar la intensidad de LED
sendValueButton.addEventListener('click', () => {
    const value = valueInput.value;
    
});

