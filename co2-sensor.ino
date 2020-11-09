#include "Freenove_WS2812_Lib_for_ESP32.h"
#include "SSD1306Wire.h"
#include "OLEDDisplayUi.h"
#include "Wire.h"
#include "MHZ19.h"                                        

#define NAME "CO2 Sensor"

// MHZ
// pin for uart reading
#define BAUDRATE 9600  
MHZ19 myMHZ19;  
HardwareSerial mySerial(2);      

// WS2812
//#define LEDS_COUNT  16
#define LEDS_COUNT  12
#define LEDS_PIN  4
#define CHANNEL   0
Freenove_ESP32_WS2812 strip = Freenove_ESP32_WS2812(LEDS_COUNT, LEDS_PIN, CHANNEL);

// SSD1306
const int I2C_DISPLAY_ADDRESS = 0x3c;
const int SDA_PIN = 21; 
const int SDC_PIN = 22;
SSD1306Wire display(I2C_DISPLAY_ADDRESS, SDA_PIN, SDC_PIN);

// KY-012
int buzzerPin = 12;
 
void setup() {
  Serial.begin(115200);
  randomSeed(analogRead(0));

  // initialize display
  Serial.println("SSD1306: Init...");
  display.init();
  display.clear();

  display.flipScreenVertically();
  display.setFont(ArialMT_Plain_16);
  display.setTextAlignment(TEXT_ALIGN_CENTER);
  display.setContrast(255);
  displayText(NAME, "");

  // WS2812
  displayText(NAME, "Init LEDs");
  Serial.println("WS2812: Init...");
  strip.begin();
  setLedRingOff();
 
  // buzzer
  displayText(NAME, "Init Buzzer");
  Serial.println("KY-012: Init...");
  pinMode (buzzerPin, OUTPUT);
  beep(100);

  // MHZ-189B
  displayText(NAME, "Init CO2 Sensor");
  Serial.println("MHZ-19B: Init...");
  mySerial.begin(BAUDRATE); // (ESP32 Example) device to MH-Z19 serial start   
  myMHZ19.begin(mySerial);                                // *Serial(Stream) refence must be passed to library begin(). 
  dumpMhz19Version();

  myMHZ19.autoCalibration();                              // Turn auto calibration ON (OFF autoCalibration(false))
}

void dumpMhz19Version() {
  char myVersion[4];          
  myMHZ19.getVersion(myVersion);

  Serial.print("\nFirmware Version: ");
  for(byte i = 0; i < 4; i++)
  {
    Serial.print(myVersion[i]);
    if (i == 1)
      Serial.print(".");    
  }
  Serial.println("");

  Serial.print("Range: ");
  Serial.println(myMHZ19.getRange());   
  Serial.print("Background CO2: ");
  Serial.println(myMHZ19.getBackgroundCO2());
  Serial.print("Temperature Cal: ");
  Serial.println(myMHZ19.getTempAdjustment());
}

int readCo2Level() { 
  int co2 =  myMHZ19.getCO2(); 
  Serial.print("CO2:");
  Serial.println(co2);
  return co2;
}

int readTemperature() { 
  int8_t temperature = myMHZ19.getTemperature();     
  Serial.print("Temperature:");
  Serial.println(temperature);
  return temperature;
}

void setLedRingOff() {
  for (int i = 0; i < LEDS_COUNT; i++) {
    strip.setLedColor(i, 0, 0, 0);
    delay(50);
  }
}

void setLedRingGreen() {
  for (int i = 0; i < LEDS_COUNT; i++) {
      strip.setLedColor(i, 0, 4, 0);
      delay(50);
  }
}

void setLedRingYellow() {
  for (int i = 0; i < LEDS_COUNT; i++) {
      strip.setLedColor(i, 4, 4, 0);
      delay(50);
  }
}

void setLedRingRed() {
  for (int i = 0; i < LEDS_COUNT; i++) {
    strip.setLedColor(i, 4, 0, 0);
    delay(50);
  }
}

void beep(int millis) {
  digitalWrite(buzzerPin, HIGH);
  delay(millis);
  digitalWrite(buzzerPin, LOW);
}

void beepLoop(int loop) {
  for (int i = 0; i < loop; i++) {
    digitalWrite(buzzerPin, HIGH);
    delay(500);
    digitalWrite(buzzerPin, LOW);
    delay(500);
  }
}

void displayText(char* header, char* detail) {
  display.clear();
  display.drawString(64, 10, header);
  display.drawString(64, 30, detail);
  display.display();  
}

void displayCo2LevelTemperature(int ppm, int temperature) {
  char buff[16];

  display.clear();
  display.setFont(ArialMT_Plain_24);
  sprintf_P(buff, PSTR("%d"), ppm);
  display.drawString(64, 10, buff);

  display.setFont(ArialMT_Plain_16);
  display.setTextAlignment(TEXT_ALIGN_CENTER);
  display.drawString(64, 30, "ppm");
  display.display();  
}



void loop() {
  int co2Level = readCo2Level();
  int temperature = readTemperature();
  displayCo2LevelTemperature(co2Level, temperature);

  if (co2Level < 800) {
    setLedRingOff();
    delay(10000);
  }
  else if (co2Level < 1000) {
    setLedRingYellow();
    delay(10000);
  }
  else {
    int time = 10000 / (LEDS_COUNT * 50 * 2);
    for (int i = 0; i < 10; i++) {
      setLedRingOff();
      setLedRingRed(); 
    }
  }
}
