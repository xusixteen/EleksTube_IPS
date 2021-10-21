#include "Arduino.h"
#include "UpdateTime.h"
#include "network.h"
#include "ESP32Time.h"
#include "ChipSelect.h"
#include "TFT_eSPI.h"
#include "number.h"
#include "common.h"

String ssid = "7#5802";
String password = "7#58027#5802";
String time_api = "http://api.m.taobao.com/rest/api3.do?api=mtop.common.getTimestamp";
#define WIFI_LIFE_CYCLE 60000 // wifi的生命周期（60s）

unsigned long m_preWifiReqMillis; // 保存上一回请求的时间戳
long long m_preNetTimestamp;         // 上一次的网络时间戳
long long m_errorNetTimestamp;         // 网络到显示过程中的时间误差
long long m_preLocalTimestamp;       // 上一次的本地机器时间戳
unsigned long timeUpdataInterval = 5000;    // 日期时钟更新的时间间隔(900s = 900000 / 2)
unsigned long preTimeMillis;         // 更新时间计数器

uint8_t network_connect_status = 0;
#define not_connect 1 
#define connecting 2
#define connected 3

extern ChipSelect chip_select;
extern uint8_t digits[NUM_DIGITS];
extern boolean time_to_sync;
//TFTs tfts;    // Display module driver
extern TFT_eSPI tft;

boolean m_wifi_status;            // 表示是wifi状态 true开启 false关闭
Network g_network; // 网络连接
ESP32Time g_rtc; // 用于时间解码
long long getTimestamp(String url)
{
    String time = "";
    HTTPClient http;
    http.setTimeout(1000);
    http.begin(url);

    // start connection and send HTTP headerFFF
    int httpCode = http.GET();
    Serial.printf("httpCode is %03d\n",httpCode);
    // httpCode will be negative on error
    if (httpCode > 0)
    {
        //Serial.printf("httpCode > 0\n");
        if (httpCode == HTTP_CODE_OK)
        {
            //Serial.printf("HTTP_CODE_OK\n");
            String payload = http.getString();
            Serial.println(payload);
            int time_index = (payload.indexOf("data")) + 12;
            time = payload.substring(time_index, payload.length() - 3);
            // 以网络时间戳为准
            m_preNetTimestamp = atoll(time.c_str()) + m_errorNetTimestamp;
            m_preLocalTimestamp = millis();
            Serial.printf("synchronized time from network successfully!\n");
        }
    }
    else
    {
        Serial.printf("[HTTP] GET... failed, error: %s\n", http.errorToString(httpCode).c_str());
        // 得不到网络时间戳时
        m_preNetTimestamp = m_preNetTimestamp + (millis() - m_preLocalTimestamp);
        m_preLocalTimestamp = millis();
    }
    http.end();

    return m_preNetTimestamp;
}
void network_init(String ssid, String password)
{
	Serial.println("scan start");
	int n = WiFi.scanNetworks();
	Serial.println("scan done");
	if (n == 0)
	{
		Serial.println("no networks found");
	}
	else
	{
		Serial.print(n);
		Serial.println(" networks found");
		for (int i = 0; i < n; ++i)
		{
			Serial.print(i + 1);
			Serial.print(": ");
			Serial.print(WiFi.SSID(i));
			Serial.print(" (");
			Serial.print(WiFi.RSSI(i));
			Serial.print(")");
			Serial.println((WiFi.encryptionType(i) == WIFI_AUTH_OPEN) ? " " : "*");
			delay(10);
		}
	}
	Serial.println("");
	Serial.print("Connecting: ");
	Serial.print(ssid.c_str());
	Serial.print(" @");
	Serial.println(password.c_str());

	WiFi.begin(ssid.c_str(), password.c_str());
	while (WiFi.status() != WL_CONNECTED)
	{
		delay(500);
		Serial.print(".");
	}
	Serial.println("");
	Serial.println("WiFi connected");
	Serial.println("IP address: ");
	Serial.println(WiFi.localIP());
}
void network_reconnect()
{
  switch (network_connect_status)
  {
    case not_connect:
    	if (WiFi.status() != WL_CONNECTED)//修改连接状态
    	{
        Serial.println("");
        Serial.print("Connecting: ");
        Serial.print(ssid.c_str());
        Serial.print(" @");
        Serial.println(password.c_str());
        WiFi.begin(ssid.c_str(), password.c_str());

        network_connect_status = connecting;
      }
      break;
    case connecting:
      if (WiFi.status() == WL_CONNECTED)
      {
        network_connect_status = connected;
      }      
      else{
        Serial.print(".\n");
      }
      break;
    case connected:
      Serial.println("");
      Serial.println("WiFi connected");
      Serial.println("IP address: ");
      Serial.println(WiFi.localIP());
//      String time = g_rtc.getTime(String("%H:%M:%S"));
//      Serial.printf("time is : %s\n",time); 
      network_connect_status = not_connect;

      break;
    default:
      break;
  }
}

void Update_time_display(){

    uint8_t hh = g_rtc.getHour(true);
    uint8_t mm = g_rtc.getMinute();
    uint8_t ss = g_rtc.getSecond();
    if ((hh == 8) && (mm == 8) & (ss == 8)){
      time_to_sync = true;
    }
    SetDigit(HOURS_TENS, hh / 10, force);
    SetDigit(HOURS_ONES, hh % 10, force);
    SetDigit(MINUTES_TENS, mm / 10, force);
    SetDigit(MINUTES_ONES, mm % 10, force);
    SetDigit(SECONDS_TENS, ss / 10, force);
    SetDigit(SECONDS_ONES, ss % 10, force);

    String time = g_rtc.getTime(String("%H:%M:%S"));
    //display_time( time.c_str());
    Serial.printf("time is : %s\n",time);    
}



void ShowDigit(uint8_t digit){//刷新单个数字显示
  chip_select.setDigit(digit);
  switch (digits[digit])
  {
  case 0:
    tft.pushImage(0, 0, dig_Width, dig_Height, dig_0);
    break;
  case 1:
    tft.pushImage(0, 0, dig_Width, dig_Height, dig_1);
    break;
  case 2:
    tft.pushImage(0, 0, dig_Width, dig_Height, dig_2);
    break;
  case 3:
    tft.pushImage(0, 0, dig_Width, dig_Height, dig_3);
    break;
  case 4:
    tft.pushImage(0, 0, dig_Width, dig_Height, dig_4);
    break;
  case 5:
    tft.pushImage(0, 0, dig_Width, dig_Height, dig_5);
    break;
  case 6:
    tft.pushImage(0, 0, dig_Width, dig_Height, dig_6);
    break;
  case 7:
    tft.pushImage(0, 0, dig_Width, dig_Height, dig_7);
    break;
  case 8:
    tft.pushImage(0, 0, dig_Width, dig_Height, dig_8);
    break;
  case 9:
    tft.pushImage(0, 0, dig_Width, dig_Height, dig_9);
    break;                  
  default:
    break;
  }

}
void SetDigit(uint8_t digit, uint8_t value, show_t show) {//更新数字
  uint8_t old_value = digits[digit];
  digits[digit] = value;

  if (show != no && (old_value != value || show == force)) {
    ShowDigit(digit);

  }
}