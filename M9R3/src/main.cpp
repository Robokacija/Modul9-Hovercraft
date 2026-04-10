#include <Arduino>
#include <WiFi.h>
#include <WebServer.h>

const char *ssid = "";
const char *password = "12345678";

WebServer server(80);

int ledPin = ;

String page = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
  <title>ESP32 Kontrola</title>
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <style>
    body { font-family: Arial; text-align: center; margin-top: 50px; }
    button {
      padding: 15px 30px;
      font-size: 18px;
      margin: 10px;
      border: none;
      border-radius: 10px;
      cursor: pointer;
    }
    .on { background-color: green; color: white; }
    .off { background-color: red; color: white; }
  </style>
</head>
<body>
  <h1>ESP32 Kontrola LEDice</h1>
  <p>Klikni gumb:</p>

  <button class="on" onclick="location.href='/on'">UPALI</button>
  <button class="off" onclick="location.href='/off'">UGASI</button>

</body>
</html>
)rawliteral";

void handleRoot()
{
  server.send(200, "text/html", page);
}

void handleOn()
{
  digitalWrite(ledPin, HIGH);
  server.sendHeader("Location", "/");
  server.send(303);
}

void handleOff()
{
  digitalWrite(ledPin, LOW);
  server.sendHeader("Location", "/");
  server.send(303);
}

void setup()
{
  Serial.begin(115200);
  pinMode(ledPin, );

  WiFi.softAP(ssid, password);

  Serial.println("Hotspot spreman!");
  Serial.println(WiFi.softAPIP())

      server.on("/", handleRoot);
  server.on("/on", handleOn);
  server.on("/off", handleOff);

  server.begin();
}

void loop()
{
  server.handleClient();
