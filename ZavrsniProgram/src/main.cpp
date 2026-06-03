#include <Arduino.h>
#include <WiFi.h>
#include <ESPAsyncWebServer.h>
#include <gotovWeb.h>
#include <motors.h>
#include <Preferences.h>
#include <AsyncTCP.h>

AsyncWebSocket ws("/ws");

Preferences preferences;

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
int offsetM1 = 0;
int offsetM2 = 0;
int offsetM3 = 0;
int offsetM4 = 0;

void onWsEvent(
    AsyncWebSocket *server,
    AsyncWebSocketClient *client,
    AwsEventType type,
    void *arg,
    uint8_t *data,
    size_t len)
{
  if (type == WS_EVT_DATA)
  {
    String msg;

    for (size_t i = 0; i < len; i++)
      msg += (char)data[i];

    int x, y, t, l;

    if (msg == "STOP")
    {
      emergencyStop = true;
      stop();
      return;
    }

    sscanf(
        msg.c_str(),
        "%d,%d,%d,%d",
        &x,
        &y,
        &t,
        &l);

    joyX = x;
    joyY = y;
    throttleVal = t;
    liftVal = l;

    lastCommandTime = millis();
    emergencyStop = false;
  }
}
// ---------------- SETUP ----------------
void setup()
{
  Serial.begin(115200);
  preferences.begin("snagaMotora", false);
  offsetM1 = preferences.getInt("off1", 0);
  offsetM2 = preferences.getInt("off2", 0);
  offsetM3 = preferences.getInt("off3", 0);
  offsetM4 = preferences.getInt("off4", 0);

  setupMotors();

  WiFi.softAP(ssid, password);
  Serial.println(WiFi.softAPIP());

  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request)
            { request->send_P(200, "text/html", index_html); });

  // server.on("/control", HTTP_GET, [](AsyncWebServerRequest *request)
  //           {
  //   lastCommandTime = millis();
  //   if (request->hasParam("x")) joyX = request->getParam("x")->value().toInt();
  //   if (request->hasParam("y")) joyY = request->getParam("y")->value().toInt();
  //   if (request->hasParam("t")) throttleVal = request->getParam("t")->value().toInt();
  //   if (request->hasParam("l"))
  //     liftVal = request->getParam("l")->value().toInt();

  //   emergencyStop = false;

  //   request->send(200, "text/plain", "OK"); });

  server.on("/save", HTTP_GET, [](AsyncWebServerRequest *request)
            {
    preferences.putInt("off1", offsetM1);
    preferences.putInt("off2", offsetM2);
    preferences.putInt("off3", offsetM3);
    preferences.putInt("off4", offsetM4);

    request->send(200, "text/plain", "SAVED"); });

  server.on("/offset", HTTP_GET, [](AsyncWebServerRequest *request)
            {
    if(request->hasParam("o1"))
        offsetM1 = request->getParam("o1")->value().toInt();

    if(request->hasParam("o2"))
        offsetM2 = request->getParam("o2")->value().toInt();

    if(request->hasParam("o3"))
        offsetM3 = request->getParam("o3")->value().toInt();

    if(request->hasParam("o4"))
        offsetM4 = request->getParam("o4")->value().toInt();

    request->send(200, "text/plain", "OK"); });
  server.on("/getOffsets", HTTP_GET, [](AsyncWebServerRequest *request)
            {
    String json = "{";
    json += "\"o1\":" + String(offsetM1) + ",";
    json += "\"o2\":" + String(offsetM2) + ",";
    json += "\"o3\":" + String(offsetM3) + ",";
    json += "\"o4\":" + String(offsetM4);
    json += "}";

    request->send(200, "application/json", json); });

  server.on("/stop", HTTP_GET, [](AsyncWebServerRequest *request)
            {
    emergencyStop = true;
    request->send(200, "text/plain", "STOPPED"); });

  ws.onEvent(onWsEvent);
  server.addHandler(&ws);

  server.begin();
}

// ---------------- LOOP ----------------
void loop()
{
  ws.cleanupClients();

  // 🔴 EMERGENCY STOP
  if (emergencyStop)
  {
    stop();
    return;
  }
  if (millis() - lastCommandTime > 200)
  {
    // nema signala 0.05 sekundi → STOP
    stop();
    return;
  }

  int lift1 = constrain(liftVal + offsetM1, 0, 255);
  int lift2 = constrain(liftVal + offsetM2, 0, 255);

  int left = constrain(throttleVal + joyY + joyX + offsetM3, 0, 255);
  int right = constrain(throttleVal + joyY - joyX + offsetM4, 0, 255);

  setLift(lift1, lift2);
  drive(left, right);

  delay(0);
}