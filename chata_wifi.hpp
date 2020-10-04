#include <WiFiNINA.h>
#include <ArduinoJson.h> 

#define DEBUG_PRINT(...); Serial.print(__VA_ARGS__);Serial1.print(__VA_ARGS__);
#define DEBUG_PRINTLN(...); Serial.println(__VA_ARGS__);Serial1.println(__VA_ARGS__);
#define SERIAL_BAUD 115200


#define WIFI_ERR 0
#define WIFI_OK 1
#define WIFI_MAX_ATTEMPTS 3


#define USE_INA
#define USE_TMP102

// Pinout
// Analog vstup: povolene napeti 0-3.3V, hodnoty: 0-1024 (je treba prepocitat na skutecne jednotky) 
// A0 - unused
// A1 - měřič proudu, měří nabíjecí/vybíjecí proud baterie 1
// A2 - měřič proudu, měří nabíjecí/vybíjecí proud baterie 2 
// A3 - soumrakové čidlo, měření soumraku
#define LIGHT_1_PIN A3 
// A4 - unused
// A5 - unused
// A6 - kamera HD, K3, spouštěna M1, noc a den
//
// Digital vstupy
// D0 - pohybové čidlo	M1
// Pozor! Piny oznacene ~D jsou mozna invertovane
// ~D1 - pohybové čidlo	M2
// ~D2 - pohybové čidlo	M3 
// ~D3 - R1 set, relé	R1	ovládané M2 (podmíněno L1), spouští venkovní světlo 
#define R1_SET_PIN 3
// ~D4	R1 reset, relé	R1	ovládané M2 (podmíněno L1), spouští venkovní světlo 
#define R1_RESET_PIN 4
// ~D5	R2 set, relé	R2	ovládá lednici (podle stavu baterie)
#define R2_SET_PIN 5
// D6	R2 reset, relé	R2	ovládá lednici (podle stavu baterie)
#define R2_RESET_PIN 6
// D7	R3 set, relé	R3	ovládá topení (podle stavu baterie a teploty)
#define R3_SET_PIN 7
// D8 MOSI	R3 reset, relé	R3	ovládá topení (podle stavu baterie a teploty)
#define R3_RESET_PIN 8
// D9 SCK	K1, kamera HD	K1	spouštěna M2, noc i den 
// D10 MISO	K2, kamera HD	K2	spouštěna M3, noc i den
// D11 SDA	SDA
// D12 SCL	SCL
// D13 RX	unused
// D14 TX	unused

// Wifi helper functions
int setup_wifi();
void printWifiData();
void printCurrentNet();
void printMacAddress(byte mac[]);
void disconnect_wifi();

// Client function
void httpRequest();

// Sensor functions
void setup_sensors();
void read_sensors();
void update_logic();
void update_json();
void temperature_logic();
void battery_voltage_logic();
void update_outputs();
