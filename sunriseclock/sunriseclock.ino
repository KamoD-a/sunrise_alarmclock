#include <RTClib.h>
#include <FastLED.h>

RTC_DS3231 rtc;

//LEDS
#define NUM_LEDS 60
CRGB leds[NUM_LEDS];
#define DATA_PIN 7
#define BRIGHTNESS 10

// event from 13:50 to 14:10
uint8_t DAILY_EVENT_START_HH = 7; // event start time: hour
uint8_t DAILY_EVENT_START_MM = 45; // event start time: minute
uint8_t DAILY_EVENT_END_HH   = 8; // event end time: hour
uint8_t DAILY_EVENT_END_MM   = 45; // event end time: minute
uint8_t SUNRISE_LENGTH = 30;


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
  Serial.println("-------NEW ATTEMPT-------");

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
  FastLED.clear();
  FastLED.show();

  FastLED.clear();  // clear all pixel data
  FastLED.show();

  //startupLEDsTest();  // optional R,G,B test
  Serial.println("Setup done. \n");
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

void loop() {
  // put your main code here, to run repeatedly:
  DateTime now = rtc.now();
  int now_in_seconds = getSeconds(now.hour(), now.minute());
  int start = getSeconds(DAILY_EVENT_START_HH, DAILY_EVENT_START_MM);
  int end = getSeconds(DAILY_EVENT_END_HH, DAILY_EVENT_END_MM);
  Serial.println(start);
  Serial.println(end);
  Serial.println(now_in_seconds);
  if (now_in_seconds >= start &&
      now_in_seconds < end) {
    Serial.println("It is on scheduled time");
    // TODO: write your code"
    sunrise(SUNRISE_LENGTH);
    FastLED.show(); 
    blinkLed();
    fill_solid(leds, NUM_LEDS, CRGB(255,255,255));
    FastLED.show();
  } else {
    Serial.println("It is NOT on scheduled time");
    printTime(now);
    delay(2000);
    //blinkLed();
    FastLED.clear();
  }

  //printTime(now);
}


int getSeconds(int hour, int min) {
  return hour * 3600 + min * 60;
}

void sunrise(int length) {
  Serial.println("Sunrise started");
  
  // total sunrise length, in minutes
  static const uint8_t sunriseLength = length;

  // how often (in seconds) should the heat color increase?
  // for the default of 30 minutes, this should be about every 7 seconds
  // 7 seconds x 256 gradient steps = 1,792 seconds = ~30 minutes
  static const uint8_t interval = (sunriseLength * 60) / 256;

  // current gradient palette color index
  static uint8_t heatIndex = 0; // start out at 0
  Serial.println(heatIndex);

  // HeatColors_p is a gradient palette built in to FastLED
  // that fades from black to red, orange, yellow, white
  // feel free to use another palette or define your own custom one
  CRGB color = ColorFromPalette(HeatColors_p, heatIndex);

  // fill the entire strip with the current color
  fill_solid(leds, NUM_LEDS, color);

  // slowly increase the heat
  EVERY_N_SECONDS(interval) {
    // stop incrementing at 255, we don't want to overflow back to 0
    if(heatIndex < 255) {
      heatIndex++;
    }
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
