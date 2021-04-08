#pragma once
#include <Arduino.h>

const char colors_html[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta http-equiv="X-UA-Compatible" content="IE=edge">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>HackMeLamp ColorPicker</title>
</head>
<body>
    <!-- <p>Clicki Gui for Non Hacker LED control</p> -->
    <div id="led-wrapper">
    </div>
    
</body>


<style>
    .led {
        background-color: grey;
        width: 150px;
        height: 150px;
        text-align: center;
        box-shadow: 0 4px 8px 0 rgba(0, 0, 0, 0.2);
        margin: 3px;
    }

    #led-wrapper {
        display: grid;
        grid-template-columns: repeat(4, 160px);
    }

    .led-color {
        opacity: 0;
        width: 150px;
        height: 150px;
    }

</style>


<script>

    window.onload = () => {
        const leds = Array.from(Array(16).keys());
        createFields(leds);
        addListeners();
    }

    createFields = (ledIds) => {
        ledIds.forEach(ledId => {
            const div = document.createElement('div');
            div.classList.add('led');
            div.id = 'led-' + ledId;
    
            const colorInput = document.createElement('input');
            colorInput.classList.add('led-color');
            colorInput.type = 'color';
            colorInput.id = 'led-br-' + ledId;
    
            div.appendChild(colorInput);
            document.getElementById('led-wrapper').appendChild(div)
        })

    }

    addListeners = () => { 
        let colorPickers = document.getElementsByClassName("led-color");
        Array.from(colorPickers).forEach(picker => {
            picker.oninput = event => {
                const ledId = event.target.id.split('-').pop();
                const selectedColor = event.target.value;
                changeColor(ledId, selectedColor);
            }
        });
    }
    
    changeColor = (id, color) => {
        document.getElementById('led-' + id).style.backgroundColor = color;
        const colorInt = parseInt(`0x${color.substring(1)}`,16);
        const request = `/singlecolor?id=${id}&color=${colorInt}`;
        sendGetRequest(request);
    }
    
    sendGetRequest = request => {
        const xmlHttp = new XMLHttpRequest();
        xmlHttp.open("GET", request, false);
        xmlHttp.send();
    }

</script>
</html>
)rawliteral";