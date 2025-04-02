#define in1 D1 // L298N Motor Driver pins.
#define in2 D2
#define in3 D5
#define in4 D6
#define LED D7

int Speed = 204; // 0 - 255.
int Speedsec;
int buttonState = 0;
int lastButtonState = 0;
int Turnradius = 0; // Set the radius of a turn, 0 - 255 Note: the robot will malfunction if this is higher than int Speed.
int brakeTime = 45;
int brkonoff = 1; // 1 for the electronic braking system, 0 for normal.

#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>

ESP8266WebServer server(80);

const char html[] = "<!DOCTYPE html>\
<html>\
<head>\
  <title>Car Control</title>\
</head>\
<body>\
  <h1>Car Control Interface</h1>\
  <p>Click the buttons below to control the car:</p>\
  <button onclick=\"sendCommand('F')\">Forward</button>\
  <button onclick=\"sendCommand('B')\">Backward</button>\
  <button onclick=\"sendCommand('L')\">Left</button>\
  <button onclick=\"sendCommand('R')\">Right</button>\
  <br>\
  <button onclick=\"sendCommand('G')\">Forward Left</button>\
  <button onclick=\"sendCommand('I')\">Forward Right</button>\
  <button onclick=\"sendCommand('H')\">Backward Left</button>\
  <button onclick=\"sendCommand('J')\">Backward Right</button>\
  <br>\
  <label for=\"speed\">Speed: </label>\
  <input type=\"number\" id=\"speed\" value=\"204\" min=\"0\" max=\"255\">\
  <button onclick=\"setSpeed()\">Set Speed</button>\
  <br>\
  <button onclick=\"sendCommand('q')\">Full Speed</button>\
  <button onclick=\"sendCommand('S')\">Stop</button>\
  <br>\
  <p>Electronic Braking System:</p>\
  <label for=\"brakeOn\">On</label>\
  <input type=\"radio\" id=\"brakeOn\" name=\"brake\" value=\"1\" checked>\
  <label for=\"brakeOff\">Off</label>\
  <input type=\"radio\" id=\"brakeOff\" name=\"brake\" value=\"0\">\
  <button onclick=\"setBrake()\">Set Brake</button>\
  <br>\
  <script>\
    function sendCommand(command) {\
      fetch('/command?command=' + command);\
    }\
    function setSpeed() {\
      const speed = document.getElementById('speed').value;\
      fetch('/command?command=' + speed);\
    }\
    function setBrake() {\
      const brakeValue = document.querySelector('input[name=\"brake\"]:checked').value;\
      fetch('/command?command=' + brakeValue);\
    }\
  </script>\
</body>\
</html>";

void setup() {
  pinMode(in1, OUTPUT);
  pinMode(in2, OUTPUT);
  pinMode(in3, OUTPUT);
  pinMode(in4, OUTPUT);
  pinMode(LED, OUTPUT); // Set the LED pin.
  Serial.begin(115200);
  WiFi.mode(WIFI_AP);
  WiFi.softAP("CarControl", "yoyo0110"); // Create an Access Point for the NodeMCU.
  Serial.print("AP IP address: ");
  Serial.println(WiFi.softAPIP());
  server.on("/", handleRoot);
  server.on("/command", handleCommand);
  server.onNotFound(handleRoot);
  server.begin();
}

void loop() {
  server.handleClient();
}

void forward() {
  analogWrite(in1, Speed);
  analogWrite(in3, Speed);
}

void back() {
  analogWrite(in2, Speed);
  analogWrite(in4, Speed);
}

void left() {
  analogWrite(in3, Speed);
  analogWrite(in2, Speed);
}

void right() {
  analogWrite(in4, Speed);
  analogWrite(in1, Speed);
}

void forwardleft() {
  analogWrite(in1, Speedsec);
  analogWrite(in3, Speed);
}

void forwardright() {
  analogWrite(in1, Speed);
  analogWrite(in3, Speedsec);
}

void backright() {
  analogWrite(in2, Speed);
  analogWrite(in4, Speedsec);
}

void backleft() {
  analogWrite(in2, Speedsec);
  analogWrite(in4, Speed);
}

void Stop() {
  analogWrite(in1, 0);
  analogWrite(in2, 0);
  analogWrite(in3, 0);
  analogWrite(in4, 0);
}

void brakeOn() {
  // Here's the future use: an electronic braking system!
  buttonState = server.arg("command")[0];
  if (buttonState != lastButtonState) {
    if (buttonState == 'S') {
      if (lastButtonState != buttonState) {
        digitalWrite(in1, HIGH);
        digitalWrite(in2, HIGH);
        digitalWrite(in3, HIGH);
        digitalWrite(in4, HIGH);
        delay(brakeTime);
        Stop();
      }
    }
    lastButtonState = buttonState;
  }
}

void brakeOff() {
  // No action needed for brakeOff in this implementation.
}

void handleRoot() {
  server.send(200, "text/html", html);
}

void handleCommand() {
  String command = server.arg("command");
  Stop(); // Initialize with motors stopped.
  if (command == "F") forward();
  else if (command == "B") back();
  else if (command == "L") left();
  else if (command == "R") right();
  else if (command == "G") forwardleft();
  else if (command == "I") forwardright();
  else if (command == "H") backleft();
  else if (command == "J") backright();
  else if (command == "0") Speed = 100;
  else if (command == "1") Speed = 140;
  else if (command == "2") Speed = 153;
  else if (command == "3") Speed = 165;
  else if (command == "4") Speed = 178;
  else if (command == "5") Speed = 191;
  else if (command == "6") Speed = 204;
  else if (command == "7") Speed = 216;
  else if (command == "8") Speed = 229;
  else if (command == "9") Speed = 242;
  else if (command == "q") Speed = 255;
  Speedsec = Turnradius;
  if (brkonoff == 1) {
    brakeOn();
  } else {
    brakeOff();
  }
  server.send(200, "text/plain", "OK");
}
