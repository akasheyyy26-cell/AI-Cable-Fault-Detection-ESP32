#include <Wire.h>
#include <Adafruit_INA219.h>
#include <LiquidCrystal_I2C.h>
#include <math.h>
#include "fault_model_esp32.h"

Adafruit_INA219 ina219;

// LCD Address set to 0x3F. If screen is still blank, try 0x27.
LiquidCrystal_I2C lcd(0x3F, 16, 2);

// ----------------- USER SETTINGS -----------------
#define VTAP_PIN 34

constexpr float R_HIGH = 100000.0f;
constexpr float R_LOW  = 10000.0f;

constexpr int   N_AVG = 10;
constexpr int   AVG_DELAY_MS = 5;
constexpr int   LOOP_DELAY_MS = 1000;
// -------------------------------------------------

// ----- VTAP conversion -----
static inline float adcToVtapActual(int adcValue) {
  const float vtapMeasured = (adcValue / 4095.0f) * 3.3f;
  const float vtapActual   = vtapMeasured * ((R_HIGH + R_LOW) / R_LOW);
  return vtapActual;
}

// ----- Softmax -----
static void softmax(const float *z, float *p, int n) {
  float maxz = z[0];
  for (int i = 1; i < n; i++) if (z[i] > maxz) maxz = z[i];
  float sum = 0.0f;
  for (int i = 0; i < n; i++) { p[i] = expf(z[i] - maxz); sum += p[i]; }
  for (int i = 0; i < n; i++) p[i] /= sum;
}

// ----- Prediction -----
static int predict_fault(float busV, float current_mA, float nodeV, float *confidence_out) {
  float x[N_FEATURES] = { busV, current_mA, nodeV };

  for (int i = 0; i < N_FEATURES; i++) {
    x[i] = (x[i] - SCALER_MEAN[i]) / SCALER_SCALE[i];
  }

  float z[N_CLASSES];
  for (int c = 0; c < N_CLASSES; c++) {
    float s = LR_B[c];
    for (int i = 0; i < N_FEATURES; i++) s += LR_W[c][i] * x[i];
    z[c] = s;
  }

  float p[N_CLASSES];
  softmax(z, p, N_CLASSES);

  int best = 0;
  for (int c = 1; c < N_CLASSES; c++) if (p[c] > p[best]) best = c;

  *confidence_out = p[best];
  return best;
}

// ----- Class Mapping -----
static const char* class_name(int idx) {
  switch (idx) {
    case 0: return CLASS_0;
    case 1: return CLASS_1;
    case 2: return CLASS_2;
    case 3: return CLASS_3;
    default: return "UNKNOWN";
  }
}

// ---------------- SETUP ----------------
void setup() {
  Serial.begin(115200);
  delay(500);

  Wire.begin();

  if (!ina219.begin()) {
    Serial.println("INA219 not detected!");
    while (1);
  }

  ina219.setCalibration_32V_2A();
  analogReadResolution(12);

  // LCD Initialization
  lcd.init();
  lcd.backlight();
  
  // Startup Splash Screen
  lcd.setCursor(0,0);
  lcd.print("AI CABLE FAULT");
  lcd.setCursor(0,1);
  lcd.print("SYSTEM READY...");
  delay(2000);
  lcd.clear();
}

// ---------------- LOOP ----------------
void loop() {

  // ----------- Sensor Read -----------
  float totalI = 0.0f;
  float totalV = 0.0f;

  for (int i = 0; i < N_AVG; i++) {
    totalI += ina219.getCurrent_mA();
    totalV += ina219.getBusVoltage_V();
    delay(AVG_DELAY_MS);
  }

  float current_mA = totalI / N_AVG;
  float busV       = totalV / N_AVG;

  // Noise filter for small currents
  if (current_mA > -5.0f && current_mA < 5.0f) current_mA = 0.0f;

  int adcValue = analogRead(VTAP_PIN);
  float nodeV  = adcToVtapActual(adcValue);

  // ----------- Inference -----------
  unsigned long startTime = micros();
  float conf = 0.0f;
  int cls = predict_fault(busV, current_mA, nodeV, &conf);
  unsigned long endTime = micros();
  
  float inferenceTime = (endTime - startTime) / 1000.0;
  float ram_kb = ESP.getFreeHeap() / 1024.0;
  const char* label = class_name(cls);

  // ----------- SERIAL OUTPUT (Keep all 4 for debugging) -----------
  Serial.println("------------ OUTPUT ------------");
  Serial.print("FAULT: "); Serial.println(label);
  Serial.print("CONF: "); Serial.print(conf * 100, 1); Serial.println(" %");
  Serial.print("Time: "); Serial.print(inferenceTime); Serial.println(" ms");
  Serial.print("RAM : "); Serial.print(ram_kb); Serial.println(" KB");
  Serial.println("--------------------------------\n");

  // ----------- LCD OUTPUT (Only First 2 Parameters) -----------
  lcd.setCursor(0, 0);
  lcd.print("FAULT:          "); // Spaces clear previous long words
  lcd.setCursor(7, 0);
  lcd.print(label);

  lcd.setCursor(0, 1);
  lcd.print("CONF: ");
  lcd.print(conf * 100, 1);
  lcd.print("%   "); // Extra spaces to clear old digits

  delay(LOOP_DELAY_MS);
}
