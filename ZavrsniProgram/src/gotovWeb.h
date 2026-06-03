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

  <h3>Motor Offsets</h3>

Front:
<input type="range" min="-255" max="255" value="0" id="off1" oninput="updateOffsets()">
<span id="off1val">0</span>

<br><br>

Back:
<input type="range" min="-255" max="255" value="0" id="off2" oninput="updateOffsets()">
<span id="off2val">0</span>

<br><br>

Left:
<input type="range" min="-255" max="255" value="0" id="off3" oninput="updateOffsets()">
<span id="off3val">0</span>

<br><br>

Right:
<input type="range" min="-255" max="255" value="0" id="off4" oninput="updateOffsets()">
<span id="off4val">0</span>

<br><br>

<button onclick="saveOffsets()">
💾 SAVE OFFSETS
</button>

  <br><br>
  <button onclick="stopAll()">🛑 STOP</button>

</div>

<script>
let socket;
window.onload = async function()
{
    socket = new WebSocket(
        "ws://" + location.host + "/ws"
    );

    await loadOffsets();
    updateOffsets();
}

async function loadOffsets()
{
    let response = await fetch('/getOffsets');
    let data = await response.json();

    document.getElementById("off1").value = data.o1;
    document.getElementById("off2").value = data.o2;
    document.getElementById("off3").value = data.o3;
    document.getElementById("off4").value = data.o4;
}

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

function updateOffsets()
{
    document.getElementById("off1val").innerText =
        document.getElementById("off1").value;

    document.getElementById("off2val").innerText =
        document.getElementById("off2").value;

    document.getElementById("off3val").innerText =
        document.getElementById("off3").value;

    document.getElementById("off4val").innerText =
        document.getElementById("off4").value;
}

function resetStick(){
  x = 0;
  y = 0;
  stick.style.left = "75px";
  stick.style.top = "75px";
  send();
}

let lastSend = 0;

function send()
{
    if(socket.readyState !== 1)
        return;

    let throttle =
        document.getElementById("throttle").value;

    let lift =
        document.getElementById("lift").value;

    socket.send(
        `${x},${y},${throttle},${lift}`
    );
}

  
function saveOffsets()
{
    let o1 = document.getElementById("off1").value;
    let o2 = document.getElementById("off2").value;
    let o3 = document.getElementById("off3").value;
    let o4 = document.getElementById("off4").value;

    fetch(`/offset?o1=${o1}&o2=${o2}&o3=${o3}&o4=${o4}`)
        .then(() => fetch('/save'));
}

function stopAll(){
  socket.send("STOP");
}
</script>

</body>
</html>
)rawliteral";
