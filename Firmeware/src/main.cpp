#include <Arduino.h>
#include "Hardware.h"
#include "ChipSelect.h"
#include "TFT_eSPI.h"
#include "time.h"
#include "Clock.h"
#include "alert.h"
#include "number.h"
#include "UpdateTime.h"
#include "ESP32Time.h"
#include "network.h"

uint64_t chipID;
ChipSelect chip_select;
uint8_t digits[NUM_DIGITS];
boolean time_to_sync = false;
//TFTs tfts;    // Display module driver
TFT_eSPI tft = TFT_eSPI();
extern ESP32Time g_rtc; // 用于时间解码

extern String time_api;
extern String ssid;
extern String password;
void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  chipID = ESP.getEfuseMac();
  Serial.printf("ESP32 Chip ID = %04X",(uint16_t)(chipID >> 32));
  Serial.printf("%08X\n",(uint32_t)chipID);
  
  chip_select.begin();
  chip_select.setAll();

  pinMode(TFT_ENABLE_PIN, OUTPUT);
  digitalWrite(TFT_ENABLE_PIN, LOW);
 

  tft.begin();
  tft.setRotation(0);	// landscape

  // Swap the colour byte order when rendering
  tft.setSwapBytes(true);
  // Draw the icons
//  UpdateDisplay(force);
// tft.pushImage(0, 0, dig_Width, dig_Height, dig_0);
//  delay(2000);
//  connect_wifi_get_time();
  for (uint8_t digit=0; digit < NUM_DIGITS; digit++) digits[digit] = 0;
  //Update_time_display();
  tft.pushImage(0, 0, dig_Width, dig_Height, dig_0_9);
  network_init(ssid, password);
  long long timestamp = getTimestamp(time_api) + (28800000); //nowapi时间API
  g_rtc.setTime(timestamp / 1000);
  uint8_t hh = g_rtc.getHour(true);
  uint8_t mm = g_rtc.getMinute();
  uint8_t ss = g_rtc.getSecond();


  SetDigit(HOURS_TENS, hh / 10, force);
  SetDigit(HOURS_ONES, hh % 10, force);
  SetDigit(MINUTES_TENS, mm / 10, force);
  SetDigit(MINUTES_ONES, mm % 10, force);
  SetDigit(SECONDS_TENS, ss / 10, force);
  SetDigit(SECONDS_ONES, ss % 10, force);


}


void loop() {
  // put your main code here, to run repeatedly:
  if (time_to_sync){
    network_reconnect();
    long long timestamp = getTimestamp(time_api) + TIMEZERO_OFFSIZE; //nowapi时间API
    g_rtc.setTime(timestamp / 1000);

    time_to_sync = false;
  }
  Update_time_display();


//  Serial.printf("system running\n");
//  Serial.printf("ESP32 Chip ID = %04X",(uint16_t)(chipID >> 32));
//  Serial.printf("%08X\n",(uint32_t)chipID);
}