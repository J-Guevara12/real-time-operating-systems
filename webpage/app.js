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



// datos del RGB

document.getElementById("redSlider").onchange = function() {
    const value = this.value;
    document.getElementById("red-slider-value").textContent = value;
    console.log(`Valor R: ${value}`);
    sendRGBvalues();
  };

  document.getElementById("greenSlider").onchange = function() {
    const value = this.value;
    document.getElementById("green-slider-value").textContent = value;
    console.log(`Valor G: ${value}`);
    sendRGBvalues(); 
};


  document.getElementById("blueSlider").onchange = function() {
    const value = this.value;
    document.getElementById("blue-slider-value").textContent = value;
    console.log(`Valor B: ${value}`);
    sendRGBvalues();
};


function sendRGBvalues(){
    const redValue = document.getElementById ("redSlider").value;
    const greenValue = document.getElementById("greenSlider").value;
    const blueValue = document.getElementById("blueSlider").value;
};


// envio y solitud del  servidor
async function sendRGBValues() {
    const redValue = document.getElementById("redSlider").value;
    const greenValue = document.getElementById("greenSlider").value;
    const blueValue = document.getElementById("blueSlider").value;
  
    const data = {
      red: redValue,
      green: greenValue,
      blue: blueValue
    };

    const response = await fetch("api/ledRGB",{
    method: "POST",headers:{ "Content-Type":"RGBdata/json"},
    body: JSON.stringify(data)});
    if (response.ok){
        const reslt = await response.json();
        return reslt;

    }
}