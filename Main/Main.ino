#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <Adafruit_MotorShield.h>
#include "utility/Adafruit_MS_PWMServoDriver.h"

// Replace with your network credentials
const char* ssid = "your_SSID";
const char* password = "your_PASSWORD";

// Motor
Adafruit_MotorShield AFMS = Adafruit_MotorShield(); 
Adafruit_DCMotor *myMotor = AFMS.getMotor(1);

// Pins
const int turbiditySensorPin = A0;
const int trigPin = 2;
const int echoPin = 3;

// Variables
int turbidityValue;
int distance;

ESP8266WebServer server(80);

void setup() {
  Serial.begin(115200);
  delay(10);

  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);

  // Motor setup
  AFMS.begin();
  myMotor->setSpeed(150);

  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  // Web server
  server.on("/", handleRoot);
  server.begin();
  Serial.println("HTTP server started");
}

void loop() {
  server.handleClient();

  distance = measureDistance();
  turbidityValue = analogRead(turbiditySensorPin);

  if (turbidityValue > 400) { // Adjust this threshold based on your requirements
    myMotor->run(FORWARD);
    delay(1000);
    myMotor->run(RELEASE);
  }
}

void handleRoot() {
  String message = "Turbidity: " + String(turbidityValue) + " | Distance: " + String(distance) + " cm";
  server.send(200, "text/plain", message);
}

int measureDistance() {
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);

  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  long duration = pulseIn(echoPin, HIGH);
  int distance = duration * 0.0344 / 2;

  return distance;
}
