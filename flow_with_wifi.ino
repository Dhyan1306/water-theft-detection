#include <WiFi.h>
#include <HTTPClient.h>

// ===== WIFI & SERVER =====
const char* ssid = "YOUR_WIFI_NAME";         
const char* password = "YOUR_WIFI_PASSWORD"; 
const char* serverName = "http://192.168.1.8:5000/data"; 

// ===== PIN DEFINITIONS =====
const int flowPin1 = 27;
const int flowPin2 = 26;
const int relayPin = 25;
const int buzzerPin = 33;

// ===== VARIABLES =====
volatile int pulseCount1 = 0;
volatile int pulseCount2 = 0;

float flowRate1 = 0;
float flowRate2 = 0;

unsigned long previousMillis = 0;
const long interval = 1000;

float theftThreshold = 6.0;  

// ===== INTERRUPTS =====
void IRAM_ATTR pulseCounter1() { pulseCount1++; }
void IRAM_ATTR pulseCounter2() { pulseCount2++; }

void setup() {
  Serial.begin(115200);

  // ===== WIFI CONNECT =====
  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi Connected!");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  // ===== PIN SETUP =====
  pinMode(flowPin1, INPUT_PULLUP);
  pinMode(flowPin2, INPUT_PULLUP);
  pinMode(relayPin, OUTPUT);
  pinMode(buzzerPin, OUTPUT);

  attachInterrupt(digitalPinToInterrupt(flowPin1), pulseCounter1, FALLING);
  attachInterrupt(digitalPinToInterrupt(flowPin2), pulseCounter2, FALLING);

  digitalWrite(relayPin, LOW);   
  digitalWrite(buzzerPin, LOW);  
}

void loop() {
  unsigned long currentMillis = millis();

  if (currentMillis - previousMillis >= interval) {

    detachInterrupt(flowPin1);
    detachInterrupt(flowPin2);

    // ===== CALCULATE FLOW =====
    flowRate1 = (pulseCount1 / 7.5);
    flowRate2 = (pulseCount2 / 7.5);
    float difference = flowRate1 - flowRate2;

    Serial.println("------");
    Serial.print("Flow1: "); Serial.println(flowRate1);
    Serial.print("Flow2: "); Serial.println(flowRate2);
    Serial.print("Difference: "); Serial.println(difference);

    // ===== THEFT LOGIC =====
    if (difference > theftThreshold) {
      Serial.println("🚨 THEFT DETECTED 🚨");
      digitalWrite(relayPin, HIGH);   
      digitalWrite(buzzerPin, HIGH);  
    } else {
      Serial.println("Normal Flow");
      digitalWrite(relayPin, LOW);    
      digitalWrite(buzzerPin, LOW);   
    }

    // ===== SEND DATA TO FLASK =====
    if (WiFi.status() == WL_CONNECTED) {
      HTTPClient http;
      http.begin(serverName);
      http.addHeader("Content-Type", "application/json");

      String jsonData = "{\"inlet\":" + String(flowRate1) +
                        ",\"outlet\":" + String(flowRate2) + "}";

      int httpResponseCode = http.POST(jsonData);
      Serial.print("HTTP Response code: ");
      Serial.println(httpResponseCode);

      http.end();
    }

    // ===== RESET COUNTERS =====
    pulseCount1 = 0;
    pulseCount2 = 0;
    previousMillis = currentMillis;

    attachInterrupt(digitalPinToInterrupt(flowPin1), pulseCounter1, FALLING);
    attachInterrupt(digitalPinToInterrupt(flowPin2), pulseCounter2, FALLING);
  }
}