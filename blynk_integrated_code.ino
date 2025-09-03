#define BLYNK_PRINT Serial
#define BLYNK_DEBUG
#define BLYNK_TEMPLATE_ID "TMPL3L6YIdqHt"
#define BLYNK_TEMPLATE_NAME "IOT EV"
#define BLYNK_AUTH_TOKEN "h65RiY567J11t0agK66l0BSkItyKBAnU"

#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
#include <LiquidCrystal_I2C.h>

// Hardware pins
const int IR_SENSOR_PIN = D5;
const int RELAY_PIN = D6;

// LCD setup
LiquidCrystal_I2C lcd(0x27, 16, 2);

// WiFi credentials
char ssid[] = "gokul";
char pass[] = "123456789";

// State variables
bool chargingState = false;
bool lastCarState = false;
unsigned long detectionTime = 0;

// Virtual Pin Mapping (Updated)
#define V_CHARGING_STATUS V0  // "Car Charging Status"
#define V_CAR_INSIDE V1       // "Car inside the station"
#define V_CAR_OUTSIDE V2      // "car outside the stat"
#define V_IR_SENSOR V3        // "Ir status"

void setup() {
  Serial.begin(115200);
  
  pinMode(IR_SENSOR_PIN, INPUT_PULLUP);
  pinMode(RELAY_PIN, OUTPUT);
  digitalWrite(RELAY_PIN, LOW);

  lcd.begin();
  lcd.backlight();
  lcd.print("System Start");
  delay(1000);

  WiFi.begin(ssid, pass);
  Blynk.config(BLYNK_AUTH_TOKEN);
}

void loop() {
  Blynk.run();
  
  // Car detection logic
  bool currentCarState = (digitalRead(IR_SENSOR_PIN) == LOW);
  
  if (currentCarState != lastCarState) {
    lastCarState = currentCarState;
    
    if (currentCarState) {
      detectionTime = millis();
    } else {
      digitalWrite(RELAY_PIN, LOW);
      chargingState = false;
    }
    updateBlynk();
  }

  // Charging logic
  if (lastCarState && !chargingState && millis() - detectionTime >= 2000) {
    digitalWrite(RELAY_PIN, HIGH);
    chargingState = true;
    updateBlynk();
  }

  updateDisplay();
  delay(10);
}

void updateBlynk() {
  // Update all Blynk widgets
  Blynk.virtualWrite(V_CHARGING_STATUS, chargingState ? "CHARGING" : "READY");
  Blynk.virtualWrite(V_CAR_INSIDE, lastCarState ? 1 : 0);
  Blynk.virtualWrite(V_CAR_OUTSIDE, lastCarState ? 0 : 1);
  Blynk.virtualWrite(V_IR_SENSOR, digitalRead(IR_SENSOR_PIN) ? "OFF" : "ON");
}

void updateDisplay() {
  static String lastStatus = "";
  String newStatus = chargingState ? "CHARGING" : "READY";
  
  if (newStatus != lastStatus) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Status:");
    lcd.setCursor(0, 1);
    lcd.print(newStatus);
    lastStatus = newStatus;
  }
}
