#include <Arduino.h>
#include <WiFi.h>
#include <ESPAsyncWebServer.h>
#include <web.h>

// ---------------- PINOVI ----------------
#define MOTOR1_A 18 // LIFT
#define MOTOR1_B 19

#define MOTOR2_A 32 // LIFT
#define MOTOR2_B 33

#define MOTOR3_A 13 // LEFT
#define MOTOR3_B 27

#define MOTOR4_A 26 // RIGHT
#define MOTOR4_B 25

// ---------------- WIFI ----------------
const char *ssid = "Hovercraft";
const char *password = "12345678";

AsyncWebServer server(80);

// ---------------- CONTROL ----------------
int joyX = 0;
int joyY = 0;
int throttleVal = 0;
int liftVal = 0;
bool emergencyStop = false;
unsigned long lastCommandTime = 0;

// ---------------- SETUP ----------------
void setup()
{
  Serial.begin(115200);

  analogWriteFrequency(20000);

  // sve ugasi


  WiFi.softAP(ssid, password);
  Serial.println(WiFi.softAPIP());

  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request)
            { request->send_P(200, "text/html", index_html); });

  server.on("/control", HTTP_GET, [](AsyncWebServerRequest *request)
            {
    lastCommandTime = millis();
    if (request->hasParam("x")) joyX = request->getParam("x")->value().toInt();
    if (request->hasParam("y")) joyY = request->getParam("y")->value().toInt();
    if (request->hasParam("t")) throttleVal = request->getParam("t")->value().toInt();
    if (request->hasParam("l"))
      liftVal = request->getParam("l")->value().toInt();

    emergencyStop = false;

    request->send(200, "text/plain", "OK"); });

  server.on("/stop", HTTP_GET, [](AsyncWebServerRequest *request)
            {
    emergencyStop = true;
    request->send(200, "text/plain", "STOPPED"); });

  server.begin();
}

// ---------------- LOOP ----------------
void loop()
{

  // 🔴 EMERGENCY STOP
  if (emergencyStop)
  {

    return;
  }
  if (millis() - lastCommandTime > 50)
  {
    // nema signala 0.5 sekundi → STOP

    return;
  }

  // ---- LIFT ----
  int lift = liftVal;

  // ---- POGON ----
  int left = constrain(throttleVal + joyY + joyX, 0, 255);
  int right = constrain(throttleVal + joyY - joyX, 0, 255);



  // DEBUG
  Serial.print("Lift: ");
  Serial.print(lift);
  Serial.print(" Left: ");
  Serial.print(left);
  Serial.print(" Right: ");
  Serial.println(right);

  delay(20);
}