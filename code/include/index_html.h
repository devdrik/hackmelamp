#pragma once
#include <Arduino.h>

const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html lang="en">

<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Litho Lamp</title>
</head>

<body>
  <h1><strong>Litho-Lamp</strong></h1>
  <p>This is Litho-Lamp. You can just use it to see a nice litho picture, or you can hack me, and do some configuration.</p>
  <p><strong>&nbsp;</strong></p>
  <h2><strong>Basic usage</strong></h2>
  <ul>
  <li>Use switch to turn on/off lamp</li>
  <li>Use USB-Port to load battery
  <ul>
  <li>Red LED: loading</li>
  <li>Blue LED: loading finished</li>
  </ul>
  </li>
  <li><u>Do not use unsupervised</u></li>
  <li>Shake for some action</li>
  </ul>
  <h2>&nbsp;</h2>
  <h2><strong>Advanced usage</strong></h2>
  <ul>
  <li>You can hack me
  <ul>
  <li>If you manage to turn on WiFi
  <ul>
  <li>And find the password&hellip;</li>
  </ul>
  </li>
  <li>Then you can use the API to play with the Litho-Lamp
  <ul>
  <li>Its IP in AP mode is: 192.168.4.1</li>
  <li>Its hostname in your WiFi is: litholamp</li>
  </ul>
  </li>
  </ul>
  </li>
  </ul>
  <p>&nbsp;</p>
  <p>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;</p>
  <h3><strong>API:</strong></h3>
  <ul>
  <li>/on</li>
  <li>/off</li>
  <li>/rgball?r=0&amp;g=127&amp;b=255</li>
  <li>/rgbsingle?id=15&amp;r=100&amp;g=100&amp;b=100</li>
  <li>/changeshocklimit?limit=16000</li>
  <li>/changeblinkduration?duration=2000</li>
  <li>/setcredentials?ssid=ssid&amp;password=password</li>
  <li>/persist [persists your current settings]</li>
  <li>/startupdate [if you manage to upload new code and do not implement an update routine, then you bricked the Litho-Lamp]</li>
  </ul>
  <h3><strong>API-parameters:</strong></h3>
  <ul>
  <li>r/g/b: 0-255</li>
  <li>id: 0-15</li>
  <li>shocklimit: 0-32000</li>
  <li>blinkduration: x [ms]</li>
  <li>ssid: your wifi ssid [then you can access Litho-Lamp through your home WiFi]</li>
  <li>password: the password of your Litho-Lamp</li>
  </ul>
</body>

</html>
)rawliteral";