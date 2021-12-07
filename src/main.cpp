//------------------------------------------------------------------//
// Supported MCU:   ESP32 (M5Stack)
// File Contents:   Clock
// Date:            2021.12.07

// This program supports the following boards:
// M5Stack(Core2 version)
//------------------------------------------------------------------//

#include <M5Core2.h>
#include <ArduinoOTA.h>

// Define
//------------------------------------------------------------------//
#define TIMER_INTERRUPT 1 // Timer Interrupt Period

// Global
//------------------------------------------------------------------//
//  WiFi
char ssid1[] = "aterm-6448d3-g";
char pass1[] = "9ada6a721d4ee";
char ssid2[] = "DESKTOP";
char pass2[] = "1234567890";
char ssid3[] = "rin-android";
char pass3[] = "1234567890";

bool second_flag = false;
bool third_flag = false;
bool wifi_flag = true;
unsigned char wifi_cnt = 0;

bool OTA_flag = false;

// Time
char ntpServer[] = "ntp.nict.jp";
//const long gmtOffset_sec = 9 * 3600L;
const long gmtOffset_sec = 0;
const int daylightOffset_sec = 0;
struct tm timeinfo;
String dateUTC;
String timeUTC;

// Timer
hw_timer_t *timer = NULL;
portMUX_TYPE timerMux = portMUX_INITIALIZER_UNLOCKED;
volatile int interruptCounter;
int iTimer10;
int elapsedTimeCounter;

// Main
bool lcd_flag = false;
int lcd_pattern = 10;
unsigned long startTime = 0;
unsigned long reStartTime = 0;
unsigned long stopTime = 0;
int elapsedTime;
int elapsedSec;
int elapsedMin;
int elapsedHour;
int elapsedDay;
bool timer_flag = false;
bool stop_flag = false;

// Prototype
//------------------------------------------------------------------//
void getTimeFromNTP(void);
void getTime(void);
void IRAM_ATTR onTimer(void);
void timerInterrupt(void);
void lcdDisplay(void);
void buttonAction(void);

// Setup
//------------------------------------------------------------------//
void setup()
{
  M5.begin(true, false, false, false, kMBusModeOutput);

  M5.Lcd.setTextColor(WHITE, BLACK);
  M5.Lcd.setTextSize(2);
  M5.Lcd.setCursor(0, 0);
  M5.Lcd.printf("Connecting room ");
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid1, pass1);
  while (WiFi.status() != WL_CONNECTED)
  {
    wifi_cnt++;
    M5.Lcd.printf(".");
    if (wifi_cnt > 20)
    {
      wifi_cnt = 0;
      second_flag = true;
      while (WiFi.status() == WL_CONNECTED)
      {

        WiFi.disconnect();
        delay(10);
      }
      break;
    }
    delay(200);
  }
  if (second_flag)
  {
    M5.Lcd.setCursor(0, 30);
    M5.Lcd.printf("Connecting desktop ");
    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid2, pass2);
    while (WiFi.status() != WL_CONNECTED)
    {
      wifi_cnt++;
      M5.Lcd.printf(".");
      if (wifi_cnt > 20)
      {
        wifi_cnt = 0;
        third_flag = true;
        while (WiFi.status() == WL_CONNECTED)
        {
          WiFi.disconnect();
          delay(10);
        }
        break;
      }
      delay(200);
    }
  }
  if (third_flag)
  {
    M5.Lcd.setCursor(0, 60);
    M5.Lcd.printf("Connecting Phone ");
    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid3, pass3);
    while (WiFi.status() != WL_CONNECTED)
    {
      wifi_cnt++;
      M5.Lcd.printf(".");
      if (wifi_cnt > 20)
      {
        wifi_cnt = 0;
        wifi_flag = false;
        break;
      }
      delay(200);
    }
  }
  M5.Lcd.clear();
  M5.Lcd.setCursor(20, 10);
  M5.Lcd.printf("Hold BtnA to switch OTA");
  delay(1000);
  M5.update();
  if (!M5.BtnA.read())
  {
    if (wifi_flag)
    {
      // timeSet
      getTimeFromNTP();
      delay(1000);
      while (WiFi.status() == WL_CONNECTED)
      {
        WiFi.disconnect();
        delay(2000);
      }
    }
  }
  else
  {
    M5.Lcd.setCursor(90, 50);
    M5.Lcd.printf("OTA ENABLED");
    delay(1000);
    OTA_flag = true;
    ArduinoOTA
        .setHostname("M5Core2")
        .onStart([]() {})
        .onEnd([]() {})
        .onProgress([](unsigned int progress, unsigned int total) {})
        .onError([](ota_error_t error) {});
    ArduinoOTA.begin();
  }
  M5.Lcd.clear();

  // Initialize Timer Interrupt
  timer = timerBegin(0, 80, true);
  timerAttachInterrupt(timer, &onTimer, true);
  timerAlarmWrite(timer, TIMER_INTERRUPT * 1000, true);
  timerAlarmEnable(timer);
}

//Main
//------------------------------------------------------------------//
void loop() {
  if (OTA_flag)
  {
    while (1)
    {
      ArduinoOTA.handle();
      M5.Lcd.setTextSize(3);
      M5.Lcd.setCursor(90, 30);
      M5.Lcd.println("OTA MODE");
    }
  }
  
  //timerInterrupt();
  getTime();
  lcdDisplay();
  buttonAction();
}

// Timer Interrupt
//------------------------------------------------------------------//
void timerInterrupt(void)
{
  if (interruptCounter > 0)
  {
    portENTER_CRITICAL(&timerMux);
    interruptCounter--;
    portEXIT_CRITICAL(&timerMux);

    iTimer10++;
    switch (iTimer10)
    {
    case 1:
      if (lcd_pattern == 10)
      {
        getTime();
        lcd_flag = true;
      }
      break;
    case 2:
      break;
    case 3:
      break;
    case 4:
      break;
    case 5:
      break;
    case 6:
      break;
    case 7:
      break;
    case 8:
      break;
    case 9:
      break;
    case 10:
      iTimer10 = 0;
      break;

    }
    
  }
}

// LCD Display
//------------------------------------------------------------------//
void lcdDisplay(void) {
  if (true)
  {
    M5.Lcd.setTextSize(1);

    /*
    M5.Lcd.setCursor(0, 0);
    M5.Lcd.println(lcd_pattern);
    M5.Lcd.setCursor(20, 0);
    M5.Lcd.println(timer_flag);
    M5.Lcd.setCursor(40, 0);
    M5.Lcd.println(stop_flag);
    M5.Lcd.setCursor(60, 0);
    M5.Lcd.println(reStartTime - stopTime);
    */

    if (timer_flag == false)
    {
      M5.Lcd.setTextColor(GREENYELLOW, BLACK);
      M5.Lcd.setCursor(40, 220);
      M5.Lcd.println("START");
    }
    else
    {
      M5.Lcd.fillRect(20,220,40,250,BLACK);
    }

    M5.Lcd.setTextColor(GREENYELLOW, BLACK);
    M5.Lcd.setCursor(240, 220);
    M5.Lcd.println("NEXT PAGE");
    
    switch (lcd_pattern)
    {
    case 10:
      M5.Lcd.setTextColor(CYAN, BLACK);
      M5.Lcd.setTextSize(3);
      M5.Lcd.setCursor(130, 30);
      M5.Lcd.println("UTC");

      M5.Lcd.setTextColor(WHITE, BLACK);
      M5.Lcd.setTextSize(4);
      M5.Lcd.setCursor(40, 80);
      M5.Lcd.printf("%04d/%02d/%02d\n", (int)(timeinfo.tm_year + 1900), (int)(timeinfo.tm_mon + 1), (int)timeinfo.tm_mday);
      M5.Lcd.setCursor(60, 130);
      M5.Lcd.printf("%02d:%02d:%02d", (int)timeinfo.tm_hour, (int)timeinfo.tm_min, (int)timeinfo.tm_sec);
      break;
    
    case 20:

      M5.Lcd.setTextColor(CYAN, BLACK);
      M5.Lcd.setTextSize(3);
      M5.Lcd.setCursor(50, 30);
      M5.Lcd.println("Elapsed time");
      M5.Lcd.setTextColor(WHITE, BLACK);
      M5.Lcd.setCursor(50, 100);
      M5.Lcd.println("BtnA:  START");
      break;
    
    case 21:
      M5.Lcd.setTextColor(GREENYELLOW, BLACK);
      M5.Lcd.setTextSize(1);
      M5.Lcd.setCursor(150, 220);
      M5.Lcd.println("STOP ");

      M5.Lcd.setTextColor(CYAN, BLACK);
      M5.Lcd.setTextSize(3);
      M5.Lcd.setCursor(50, 30);
      M5.Lcd.println("Elapsed time");

      //elapsedTime = (millis() - startTime - (reStartTime - stopTime)) / 1000;
      elapsedTime = (millis() - startTime) / 1000;
      elapsedDay = elapsedTime / 3600 * 24;
      elapsedHour = elapsedTime / 3600;
      elapsedMin = elapsedTime / 60;
      elapsedSec = elapsedTime % 60;

      M5.Lcd.setTextColor(WHITE, BLACK);
      M5.Lcd.setTextSize(4);
      M5.Lcd.setCursor(30, 100);
      M5.Lcd.printf("%02d:%02d:%02d:%02d", elapsedDay, elapsedHour, elapsedMin, elapsedSec);
      //M5.Lcd.printf("%d", elapsedTime);
      break;

    case 22:
      M5.Lcd.setTextColor(GREENYELLOW, BLACK);
      M5.Lcd.setTextSize(1);
      M5.Lcd.setCursor(150, 220);
      M5.Lcd.println("RESET");

      M5.Lcd.setTextColor(CYAN, BLACK);
      M5.Lcd.setTextSize(3);
      M5.Lcd.setCursor(50, 30);
      M5.Lcd.println("Elapsed time");

      M5.Lcd.setTextColor(WHITE, BLACK);
      M5.Lcd.setTextSize(4);
      M5.Lcd.setCursor(30, 100);
      M5.Lcd.printf("%02d:%02d:%02d:%02d", elapsedDay, elapsedHour, elapsedMin, elapsedSec);
      break;
    }
    lcd_flag = false;
  }
}

void buttonAction(void){
  M5.update();

  if (M5.BtnC.wasPressed())
  {
    M5.Lcd.clear();
    if (lcd_pattern >= 20)
    {
      lcd_pattern = 10;
    }
    else
    {
      if (timer_flag == true && stop_flag == true)
      {
        lcd_pattern = 22;
      }
      else if (timer_flag == true && stop_flag == false)
      {
        lcd_pattern = 21;
      }
      else
      {
        lcd_pattern = 20;
      }
    }
  }

  if (M5.BtnA.wasPressed())
  {
    if (timer_flag == false && stop_flag == false)
    {
      M5.Lcd.clear();
      timer_flag = true;
      startTime = millis();
      lcd_pattern = 21;
    }
    /*
    else if (timer_flag == true && stop_flag == true)
    {
      M5.Lcd.clear();
      reStartTime = millis();
      stop_flag = false;
      lcd_pattern = 21;
    }*/
  }

  if (M5.BtnB.wasPressed())
  {
    M5.Lcd.clear();
    if (timer_flag == true && stop_flag == false)
    {
      stopTime = millis();
      stop_flag = true;
      lcd_pattern = 22;
    }
    else
    {
      M5.Lcd.fillRect(140, 220, 160, 250, BLACK);
      startTime = 0;
      reStartTime = 0;
      stopTime = 0;
      timer_flag = false;
      stop_flag = false;
      lcd_pattern = 20;
    }
  }
}

// Get Time From NTP
//------------------------------------------------------------------//
void getTimeFromNTP(void)
{
  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
}

// Get Convert Time
//------------------------------------------------------------------//
void getTime(void)
{
  getLocalTime(&timeinfo);
}

// IRAM
//------------------------------------------------------------------//
void IRAM_ATTR onTimer()
{
  portENTER_CRITICAL_ISR(&timerMux);
  interruptCounter = 1;
  portEXIT_CRITICAL_ISR(&timerMux);
}