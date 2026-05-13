#include <Arduino.h>
const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
<meta name="viewport" content="width=device-width, initial-scale=1.0">
<style>
body { text-align:center; font-family: Arial; }
#joystick {
  width:200px; height:200px;
  background:#ddd;
  margin:auto;
  border-radius:50%;
  position:relative;
  touch-action:none;
}
#stick {
  width:50px; height:50px;
  background:#333;
  border-radius:50%;
  position:absolute;
  top:75px; left:75px;
}
button {
  font-size:20px;
  padding:10px;
  background:red;
  color:white;
  border:none;
  border-radius:10px;
}
</style>
</head>
<body>

<div>
  <h2>Hovercraft Control 😎</h2>

  <div id="joystick">
    <div id="stick"></div>
  </div>

  <br><br>
  Throttle: <input type="range" min="0" max="255" value="0" id="throttle">

  <br><br>
  Lift: <input type="range" min="0" max="255" value="0" id="lift">

  <br><br>
  <button onclick="stopAll()">🛑 STOP</button>

</div>

<script>
let joy = document.getElementById("joystick");
let stick = document.getElementById("stick");

let x=0, y=0;

joy.addEventListener("touchmove", move);
joy.addEventListener("mousemove", move);

document.addEventListener("touchend", resetStick);
document.addEventListener("mouseup", resetStick);

function move(e){
  let rect = joy.getBoundingClientRect();
  let clientX = e.touches ? e.touches[0].clientX : e.clientX;
  let clientY = e.touches ? e.touches[0].clientY : e.clientY;

  let dx = clientX - rect.left - 100;
  let dy = clientY - rect.top - 100;

  let max = 80;
  dx = Math.max(-max, Math.min(max, dx));
  dy = Math.max(-max, Math.min(max, dy));

  stick.style.left = (dx+100-25) + "px";
  stick.style.top = (dy+100-25) + "px";

  x = Math.round(dx/max * 100);
  y = Math.round(-dy/max * 100);

  send();
}

function resetStick(){
  x = 0;
  y = 0;
  stick.style.left = "75px";
  stick.style.top = "75px";
  send();
  fetch('/stop');
}

function send(){
  let throttle = document.getElementById("throttle").value;
  let lift = document.getElementById("lift").value;
  fetch(`/control?x=${x}&y=${y}&t=${throttle}`);
  fetch(`/control?x=${x}&y=${y}&l=${lift}`);
}

function stopAll(){
  fetch('/stop');
}
</script>

</body>
</html>
)rawliteral";
