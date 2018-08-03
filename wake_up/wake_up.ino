#include <Wire.h>
#include "RTClib.h"
#include <SPI.h>
#include <Sunrise.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Pushbutton.h>


int BUZZER_PIN = 4;
int BTN_PIN = 8;

Adafruit_SSD1306 display(4);
RTC_DS1307 rtc;
AlarmStateMachine *machine;
Pushbutton button(BTN_PIN);

void setup () {
  Serial.begin(9600);
  Wire.begin();

  if (! rtc.begin()) {
    Serial.println("Couldn't find RTC");
    while (1);
  }

  if (! rtc.isrunning()) {
    Serial.println("RTC is NOT running! adjusting to compilation time!");
    // set the RTC to the date & time this sketch was compiled
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  }

  Serial.println("Initializing display");
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  display.setTextColor(WHITE);
  display.setTextSize(2);

  pinMode(BTN_PIN, INPUT);
  digitalWrite(BTN_PIN, LOW);

  Serial.println("Initializing alarm state machine");
  machine = new AlarmStateMachine(rtc.now(), 9, 32);
}

void loop () {
  display.clearDisplay();
  display.setCursor(0, 0);

  DateTime now = rtc.now();
  machine->setCurrentTime(now);

  Serial.println(machine->currentDisplay);
  Serial.println(machine->isAlarming);

  // read the state of the pushbutton value:
  if (button.getSingleDebouncedPress()) {
    Serial.println("btn pressed");
    machine->onWakeBtn();
  }

  display.print(machine->currentDisplay);
  digitalWrite(BUZZER_PIN, machine->isAlarming ? HIGH : LOW);

  display.display();
  delay(100);
}
