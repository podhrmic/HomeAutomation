#include "chata_wifi.hpp"

/**
 * Dashboard is at: https://io.adafruit.com/mySecretUsername/dashboards/chata
 */

#ifdef USE_LOW_POWER
#include <ArduinoLowPower.h>
#endif

#define DEEP_SLEEP_PERIOD_MS 60000

void setup() {
  //Initialize serial and wait for port to open:
  Serial.begin(SERIAL_BAUD);
  Serial1.begin(SERIAL_BAUD);

  DEBUG_PRINTLN("Setting up sensors..");
  setup_sensors();
  DEBUG_PRINTLN("Sensors OK...");
  delay(1000);

  pinMode(R1_SET_PIN, OUTPUT);
  pinMode(R1_RESET_PIN, OUTPUT);
  pinMode(R2_SET_PIN, OUTPUT);
  pinMode(R2_RESET_PIN, OUTPUT);
  pinMode(R3_SET_PIN, OUTPUT);
  pinMode(R3_RESET_PIN, OUTPUT);
}

void loop() {
  DEBUG_PRINTLN("Setting up wifi..");
  int wifi_status = setup_wifi();
  DEBUG_PRINTLN("Wifi setup done...");
  delay(1000);

  read_sensors();
  update_logic();
  update_json();
  if (wifi_status == WIFI_OK) {
    httpRequest(); // send data to Cloud 
  }
#ifdef WIFI_DISCONNECT_AFTER_SEND
  disconnect_wifi();
#endif

#ifdef USE_LOW_POWER
  DEBUG_PRINTLN("Going to deep sleep");
  LowPower.deepSleep(DEEP_SLEEP_PERIOD_MS);  
#else
  DEBUG_PRINTLN("Going to delay");
  delay(DEEP_SLEEP_PERIOD_MS); // Sleep for one minute
#endif
}
