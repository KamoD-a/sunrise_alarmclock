#include <RTClib.h>
#include <FastLED.h>


constexpr uint32_t getSeconds(int hour, int min) {
  return (uint32_t)(hour * 3600) + min * 60;
}

#define NUM_LEDS 60
#define DATA_PIN 7
#define BRIGHTNESS 10

RTC_DS3231 rtc;
CRGB leds[NUM_LEDS];

constexpr uint8_t DAILY_EVENT_START_HH = 7; // event start time: hour
constexpr uint8_t DAILY_EVENT_START_MM = 15; // event start time: minute
constexpr uint8_t DAILY_EVENT_END_HH   = 9; // event end time: hour
constexpr uint8_t DAILY_EVENT_END_MM   = 30; // event end time: minute
constexpr uint8_t SUNRISE_LENGTH = 40;
bool SUNRISE_COMPLETED = false;

constexpr uint32_t start_time = getSeconds(DAILY_EVENT_START_HH, DAILY_EVENT_START_MM);
constexpr uint32_t end_time = getSeconds(DAILY_EVENT_END_HH, DAILY_EVENT_END_MM);

char daysOfTheWeek[7][12] = {
  "Sunday",
  "Monday",
  "Tuesday",
  "Wednesday",
  "Thursday",
  "Friday",
  "Saturday"
};

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  // Serial.println("-------NEW ATTEMPT-------");

  // SETUP RTC MODULE
  if (! rtc.begin()) {
    Serial.println("Couldn't find RTC");
    while (1);
  }

  rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));

  if(rtc.lostPower()) {
    // this will adjust to the date and time at compilation
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
   }

  //we don't need the 32K Pin, so disable it
  rtc.disable32K();

  FastLED.addLeds<WS2812B, DATA_PIN, GRB>(leds, NUM_LEDS);
  FastLED.clear();  // clear all pixel data
  FastLED.show();

  //startupLEDsTest();  // optional R,G,B test
  Serial.println("Setup done. \n");
}

void loop() {
  DateTime now = rtc.now();
  int now_in_seconds = getSeconds(now.hour(), now.minute());
  
  if (now_in_seconds >= start_time &&
      now_in_seconds < end_time) {
    if (SUNRISE_COMPLETED == false){
      sunrise();
    } else {
      fill_solid(leds, NUM_LEDS, CRGB(255,255,255));
    }
  } else {
    delay(2000);
    fill_solid(leds, NUM_LEDS, CRGB(0,0,0));
    FastLED.show();
    SUNRISE_COMPLETED = false;
  }
}

void sunrise() {
  Serial.println("Sunrise started");

  const uint8_t interval = (SUNRISE_LENGTH * 60) / 256;

  static uint8_t heatIndex = 0; // start out at 0


  do 
  {
    CRGB color = ColorFromPalette(HeatColors_p, heatIndex);

    fill_solid(leds, NUM_LEDS, color);
    FastLED.show();

    heatIndex++;

    delay(interval * 1000uL);

  } while(heatIndex < 255);

  if (heatIndex == 255) {
    SUNRISE_COMPLETED = true;
  }
}

void printTime(DateTime time) {
  Serial.print("TIME: ");
  Serial.print(time.year(), DEC);
  Serial.print('/');
  Serial.print(time.month(), DEC);
  Serial.print('/');
  Serial.print(time.day(), DEC);
  Serial.print(" (");
  Serial.print(daysOfTheWeek[time.dayOfTheWeek()]);
  Serial.print(") ");
  Serial.print(time.hour(), DEC);
  Serial.print(':');
  Serial.print(time.minute(), DEC);
  Serial.print(':');
  Serial.println(time.second(), DEC);
}

void blinkLed(){
  digitalWrite(12, HIGH); // LED on the output 8
  delay(500); // pause in program execution by half a second
  digitalWrite(12, LOW); // LED off on output 8
  delay(500);
}

void startupLEDsTest() {
  // startup blink test to confirm LEDs are working.
  FastLED.setBrightness(32);
  fill_solid(leds, NUM_LEDS, CRGB(255,0,0));  // fill red
  FastLED.show();
  delay(1000);
  fill_solid(leds, NUM_LEDS, CRGB(0,255,0));  // fill green
  FastLED.show();
  delay(1000);
  fill_solid(leds, NUM_LEDS, CRGB(0,0,255));  // fill blue
  FastLED.show();
  delay(1000);
  FastLED.clear();
  FastLED.show();
  //FastLED.setBrightness(BRIGHTNESS);

} //end_startupLEDsTest
