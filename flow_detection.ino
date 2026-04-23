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
void IRAM_ATTR pulseCounter1() {
  pulseCount1++;
}

void IRAM_ATTR pulseCounter2() {
  pulseCount2++;
}

void setup() {
  Serial.begin(115200);

  pinMode(flowPin1, INPUT_PULLUP);
  pinMode(flowPin2, INPUT_PULLUP);
  pinMode(relayPin, OUTPUT);
  pinMode(buzzerPin, OUTPUT);

  attachInterrupt(digitalPinToInterrupt(flowPin1), pulseCounter1, FALLING);
  attachInterrupt(digitalPinToInterrupt(flowPin2), pulseCounter2, FALLING);

  digitalWrite(relayPin, LOW);   // Motor ON (Active LOW relay)
  digitalWrite(buzzerPin, LOW);  // Buzzer OFF
}

void loop() {

  unsigned long currentMillis = millis();

  if (currentMillis - previousMillis >= interval) {

    detachInterrupt(flowPin1);
    detachInterrupt(flowPin2);

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

      digitalWrite(relayPin, HIGH);   // MOTOR OFF
      digitalWrite(buzzerPin, HIGH);  // BUZZER ON
    }
    else {
      Serial.println("Normal Flow");

      digitalWrite(relayPin, LOW);    // MOTOR ON
      digitalWrite(buzzerPin, LOW);   // BUZZER OFF
    }

    pulseCount1 = 0;
    pulseCount2 = 0;

    previousMillis = currentMillis;

    attachInterrupt(digitalPinToInterrupt(flowPin1), pulseCounter1, FALLING);
    attachInterrupt(digitalPinToInterrupt(flowPin2), pulseCounter2, FALLING);
  }
}