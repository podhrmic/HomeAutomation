#include "chata_wifi.hpp"
#include "arduino_secrets.h"

char ssid[] = SECRET_SSID;         // your network SSID (name)
char pass[] = SECRET_PASS;         // your network password (use for WPA, or use as key for WEP)
int status = WL_IDLE_STATUS;     // the Wifi radio's status
char server[] = "io.adafruit.com"; // name address for Adafruit IOT Cloud

// Json doc
StaticJsonDocument<1024> doc;

// Initialize the client library
WiFiSSLClient client;

// Sensors
float t1,t2,t3,t4,v1,v2;
int a1,a2, a3, r1, r2, r3;

#ifdef USE_TMP102
#include <Temperature_LM75_Derived.h>
TI_TMP102 t1_sensor(0x48);
TI_TMP102 t2_sensor(0x49);
TI_TMP102 t3_sensor(0x4A);
TI_TMP102 t4_sensor(0x4B);
#endif

#ifdef USE_INA
#include <Adafruit_INA219.h>
Adafruit_INA219 v1_ina219(0x40);
Adafruit_INA219 v2_ina219(0x41);
#endif

void disconnect_wifi() {
  DEBUG_PRINTLN("Disconnecting wifi.");
  WiFi.disconnect();
  DEBUG_PRINTLN("Wifi disconnected.");
}

int setup_wifi() {
  // check for the WiFi module:
  if (WiFi.status() == WL_NO_MODULE) {
    DEBUG_PRINTLN("Communication with WiFi module failed!");
    // don't continue
    return WIFI_ERR;
  }

  String fv = WiFi.firmwareVersion();
  if (fv < WIFI_FIRMWARE_LATEST_VERSION) {
    DEBUG_PRINTLN("Please upgrade the firmware");
  }

  // attempt to connect to Wifi network:
  int attempts = 0;
  while ( (status != WL_CONNECTED) && (attempts < WIFI_MAX_ATTEMPTS)) {
    DEBUG_PRINT("Attempting to connect to WPA SSID: ");
    DEBUG_PRINTLN(ssid);
    // Connect to WPA/WPA2 network:
    status = WiFi.begin(ssid, pass);

    // wait 10 seconds for connection:
    delay(10000);
    attempts++;
  }

  if (status != WL_CONNECTED) {
    DEBUG_PRINTLN("Couldn't connect");
    return WIFI_ERR;
  }

  // you're connected now, so print out the data:
  DEBUG_PRINT("You're connected to the network");
  printCurrentNet();
  printWifiData();

  return WIFI_OK;
}

void printWifiData() {
  // print your board's IP address:
  IPAddress ip = WiFi.localIP();
  DEBUG_PRINT("IP Address: ");
  DEBUG_PRINTLN(ip);
  DEBUG_PRINTLN(ip);

  // print your MAC address:
  byte mac[6];
  WiFi.macAddress(mac);
  DEBUG_PRINT("MAC address: ");
  printMacAddress(mac);
}

void printCurrentNet() {
  // print the SSID of the network you're attached to:
  DEBUG_PRINT("SSID: ");
  DEBUG_PRINTLN(WiFi.SSID());

  // print the MAC address of the router you're attached to:
  byte bssid[6];
  WiFi.BSSID(bssid);
  DEBUG_PRINT("BSSID: ");
  printMacAddress(bssid);

  // print the received signal strength:
  long rssi = WiFi.RSSI();
  DEBUG_PRINT("signal strength (RSSI):");
  DEBUG_PRINTLN(rssi);

  // print the encryption type:
  byte encryption = WiFi.encryptionType();
  DEBUG_PRINT("Encryption Type:");
  DEBUG_PRINTLN(encryption, HEX);
  DEBUG_PRINTLN();
}

void printMacAddress(byte mac[]) {
  for (int i = 5; i >= 0; i--) {
    if (mac[i] < 16) {
      DEBUG_PRINT("0");
    }
    DEBUG_PRINT(mac[i], HEX);
    if (i > 0) {
      DEBUG_PRINT(":");
    }
  }
  DEBUG_PRINTLN();
}

/* 
 * This method makes a HTTP connection to the server and post deread sensor values 
 * to the Adafruit IOT Cloud
 */
void httpRequest()
{
  // Check if we are connected
  DEBUG_PRINTLN("\nChecking connection to server...");
  if (!client.connected()) {
    // If not, connect
    DEBUG_PRINTLN("\nStarting new connection to server...");
    if (!client.connect(server, 443)) {
      // if you couldn't make a connection:
      DEBUG_PRINTLN("connection failed!");
      client.stop();
      client.flush();
      return; // return from the function
    }
  }
  // all good, update:

  DEBUG_PRINTLN("connected to server");
  // Make a HTTP request:
  client.println("POST /api/v2/" IO_USERNAME "/groups/" IO_GROUP "/data HTTP/1.1"); 
  client.println("Host: io.adafruit.com");  
  client.println("Connection: close");  
  client.print("Content-Length: ");  
  client.println(measureJson(doc));  
  client.println("Content-Type: application/json");  
  client.println("X-AIO-Key: " IO_KEY); 

  // Terminate headers with a blank line
  client.println();
  // Send JSON document in body
  serializeJson(doc, client);

  DEBUG_PRINTLN("data sent!");

  DEBUG_PRINTLN("Stoping an flushing the client!");
  client.stop();
  client.flush();
}


void setup_sensors() {
#ifdef USE_INA
  v1_ina219.begin();
  delay(1000);
  v2_ina219.begin();
  delay(1000);

  /*
  v1_ina219.powerSave(false);
  delay(1000);
  v2_ina219.powerSave(false);
  delay(1000);
  */
#endif

#ifdef USE_TMP102
  /*
  t1_sensor.disableShutdownMode();
  delay(1000);
  t2_sensor.disableShutdownMode();
  delay(1000);
  t3_sensor.disableShutdownMode();
  delay(1000);
  t4_sensor.disableShutdownMode();
  delay(1000);
  */
#endif
}

void read_sensors() {
  DEBUG_PRINTLN("Read sensors");
#ifdef USE_INA
  // voltage sensor
  DEBUG_PRINT("V1: ");
  v1 = v1_ina219.getBusVoltage_V();
  DEBUG_PRINT(v1);
  DEBUG_PRINT("[V]\r\n");
  delay(1000);

  DEBUG_PRINT("V2: ");
  v2 = v2_ina219.getBusVoltage_V();
  DEBUG_PRINT(v2);
  DEBUG_PRINT("[V]\r\n");
  delay(1000);
#endif

#ifdef USE_TMP102
  DEBUG_PRINT("T1: ");
  t1 = t1_sensor.readTemperatureC();
  DEBUG_PRINT(t1);
  DEBUG_PRINT("[C]\r\n");
  delay(1000);

  DEBUG_PRINT("T2: ");
  t2 = t2_sensor.readTemperatureC();
  DEBUG_PRINT(t2);
  DEBUG_PRINT("[C]\r\n");
  delay(1000);
  
  DEBUG_PRINT("T3: ");
  t3 = t3_sensor.readTemperatureC();
  DEBUG_PRINT(t3);
  DEBUG_PRINT("[C]\r\n");
  delay(1000);

  DEBUG_PRINT("T4: ");
  t4 = t4_sensor.readTemperatureC();
  DEBUG_PRINT(t4);
  DEBUG_PRINT("[C]\r\n");
  delay(1000);
#endif

  // Light sensor, 0-1024
  DEBUG_PRINT("A1: ");
  a1 = analogRead(LIGHT_1_PIN);
  DEBUG_PRINT(a1);
  DEBUG_PRINT("\r\n");
  delay(1000);

  // R1
  DEBUG_PRINT("R1 set/reset: ");
  DEBUG_PRINT(digitalRead(R1_SET_PIN));
  DEBUG_PRINT("/");
  DEBUG_PRINT(digitalRead(R1_RESET_PIN));
  DEBUG_PRINT("\r\n");
  delay(1000);

  // R2
  DEBUG_PRINT("R2 set/reset: ");
  DEBUG_PRINT(digitalRead(R2_SET_PIN));
  DEBUG_PRINT("/");
  DEBUG_PRINT(digitalRead(R2_RESET_PIN));
  DEBUG_PRINT("\r\n");
  delay(1000);

  // R3
  DEBUG_PRINT("R3 set/reset: ");
  DEBUG_PRINT(digitalRead(R3_SET_PIN));
  DEBUG_PRINT("/");
  DEBUG_PRINT(digitalRead(R3_RESET_PIN));
  DEBUG_PRINT("\r\n");
  delay(1000);

  // Current sensor, 0-1024
  DEBUG_PRINT("A2: ");
  a2 = analogRead(CURRENT_1_PIN);
  DEBUG_PRINT(a2);
  DEBUG_PRINT("\r\n");
  delay(1000);

  // Current sensor, 0-1024
  DEBUG_PRINT("A3: ");
  a3 = analogRead(CURRENT_2_PIN);
  DEBUG_PRINT(a3);
  DEBUG_PRINT("\r\n");
  delay(1000);
}

void update_logic() {
  DEBUG_PRINTLN("Update_logic...");
  temperature_logic();
  battery_voltage_logic();
  update_outputs();
}

/* 
 * https://io.adafruit.com/api/docs/#operation/createGroupData
 * 
 * POST /{username}/groups/{group_key}/data
 * 
 * JSON:
 * 
{
  "location": {
    "lat": 0,
    "lon": 0,
    "ele": 0
  },
  "feeds": [
    {
      "key": "string",
      "value": "string"
    }
  ],
  "created_at": "string"
}
 */
void update_json() {
  // Reset
  doc.clear();

   // Add the "location" object
  JsonObject location = doc.createNestedObject("location");
  location["lat"] = 0;
  location["lon"] = 0;
  location["ele"] = 0;
  
  // Add the "feeds" array
  JsonArray feeds = doc.createNestedArray("feeds");
  JsonObject feed1 = feeds.createNestedObject();
  feed1["key"] = "t1";
  feed1["value"] = t1;
  JsonObject feed2 = feeds.createNestedObject();
  feed2["key"] = "t2";
  feed2["value"] = t2;
  JsonObject feed3 = feeds.createNestedObject();
  feed3["key"] = "t3";
  feed3["value"] = t3;
  JsonObject feed4 = feeds.createNestedObject();
  feed4["key"] = "t4";
  feed4["value"] = t4;
  JsonObject feed5 = feeds.createNestedObject();
  feed5["key"] = "v1";
  feed5["value"] = v1;
  JsonObject feed6 = feeds.createNestedObject();
  feed6["key"] = "v2";
  feed6["value"] = v2;
  JsonObject feed7 = feeds.createNestedObject();
  feed7["key"] = "light1";
  feed7["value"] = a1;
  JsonObject feed8 = feeds.createNestedObject();
  feed8["key"] = "r1";
  feed8["value"] = r1; 
  JsonObject feed9 = feeds.createNestedObject();
  feed9["key"] = "r2";
  feed9["value"] = r2;
  JsonObject feed10 = feeds.createNestedObject();
  feed10["key"] = "r3";
  feed10["value"] = r3;
}

/**
 * 
 * Měření teploty                
                
obecně: kontinuální měření v určitých intervalech (5-20 minut)              
  - prezentace - na webu -              
  - alarm v případě mezních hodnot (+ 38° v interieru, -1°v technické místnosti nebo koupelně)              
  - v případě alarmu (-1°) a v případě splnění dalších podmínek (dostatek výkonu) - zapnutí topení (relé R3)              
  - T1 technicka mistnost
  - T2 koupelna
  - T3 pokoj
  - T4 venku
 */
#define TEMPERATURE_MAX_C 38.0
#define T4_TEMPERATURE_MIN_C 3.0
#define T4_TEMPERATURE_MAX_C 4.0
#define BATTERY_HEATING_THRESHOLD_V 12.5
void temperature_logic() {
  if (t3 > TEMPERATURE_MAX_C) {
    // alarm!
    DEBUG_PRINTLN("Alarm: t3 > TEMPERATURE_MAX_C!");
  }

  if (t4 < T4_TEMPERATURE_MIN_C) {
    // alarm!
    DEBUG_PRINTLN("Alarm: t4 < T_4TEMPERATURE_MIN_C");
    if (v2 > BATTERY_HEATING_THRESHOLD_V)
      DEBUG_PRINTLN("Alarm: v2 > BATTERY_HEATING_THRESHOLD_V");
      r3 = true;
  }
  
  if (v2 <= BATTERY_HEATING_THRESHOLD_V || t4 > T4_TEMPERATURE_MAX_C) {
    // alarm!
    DEBUG_PRINTLN("Alarm: v2 <= BATTERY_HEATING_THRESHOLD_V");
    r3 = false;
  }
}

/**
 * 
 * Kontrola napětí baterii                
kontrola nabíjecího a vybíjecího proudu baterií               
přenos na web – dálkové sledování               
                
                
interval  2 minuty  ?           
                
2 baterie – dva nezávislé okruhy                
                
Baterie A provozní proud, světla, motory, čerpadla              
  kontrola nabíjecího / vybíjecího proudu             
                
  kontrola napětí baterie             
  při napětí nad X Voltů sepnout relé, které sepne relé lednice (10A)         Relé R2   
  při poklesu pod Y voltů vypnout relé lednice              
  v zimním režimu místo lednice – při poklesu teploty – spínat temperující topení (zajišťuje jiné relé)             relé R3
                (temperature_logic)
                
Baterie B (baterie, která krmí systém počítače + ostrahy )              
  kontrola stavu, dostatečného napětí             
  alarm nízkého napětí              
*/
#define BATTERY_B_LOW_VOLTAGE_V 11.0
#define BATTERY_FRIDGE_CONTROL_ON_THRESHOLD_V 12.5
#define BATTERY_FRIDGE_CONTROL_OFF_THRESHOLD_V 12.15
void battery_voltage_logic() {
  if (v2 < BATTERY_B_LOW_VOLTAGE_V){
    // alarm!
    DEBUG_PRINTLN("Alarm: v2 < BATTERY_B_LOW_VOLTAGE_V");
    r2 = false;
  }
  
  if (v2 > BATTERY_FRIDGE_CONTROL_ON_THRESHOLD_V) {
    // alarm!
    DEBUG_PRINTLN("Alarm: v2 > BATTERY_FRIDGE_CONTROL_ON_THRESHOLD_V");
    DEBUG_PRINTLN("Zapinam lednici!");
    r2 = true;
  } 
  if (v2 < BATTERY_FRIDGE_CONTROL_OFF_THRESHOLD_V) {
    // alarm!
    DEBUG_PRINTLN("Vypinam lednici!");
    DEBUG_PRINTLN("Alarm: v2 < BATTERY_FRIDGE_CONTROL_OFF_THRESHOLD_V");
    r2 = false;
  }
}

void update_outputs() {
  DEBUG_PRINTLN("Updating outputs.");
  if (r1) {
    DEBUG_PRINTLN("R1 set");
    digitalWrite(R1_SET_PIN, HIGH);
    digitalWrite(R1_RESET_PIN, LOW);
  } else {
    DEBUG_PRINTLN("R1 reset");
    digitalWrite(R1_SET_PIN, LOW);
    digitalWrite(R1_RESET_PIN, HIGH);
  }
  if (r2) {
    DEBUG_PRINTLN("R2 set");
    digitalWrite(R2_SET_PIN, HIGH);
    digitalWrite(R2_RESET_PIN, LOW);
  } else {
    DEBUG_PRINTLN("R2 reset");
    digitalWrite(R2_SET_PIN, LOW);
    digitalWrite(R2_RESET_PIN, HIGH);
  }
  if (r3) {
    DEBUG_PRINTLN("R3 set");
    digitalWrite(R3_SET_PIN, HIGH);
    digitalWrite(R3_RESET_PIN, LOW);
  } else {
    DEBUG_PRINTLN("R1 reset");
    digitalWrite(R3_SET_PIN, LOW);
    digitalWrite(R3_RESET_PIN, HIGH);
  }
}
