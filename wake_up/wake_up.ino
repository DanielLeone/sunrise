#include <Wire.h>
#include "RTClib.h"
#include <SPI.h>
#include <Sunrise.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Pushbutton.h>


int BUZZER_PIN = 2;
int BTN_PIN = 4;

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
  pinMode(BUZZER_PIN, OUTPUT);

  Serial.println("Initializing alarm state machine");
  machine = new AlarmStateMachine(rtc.now(), 6, 0);
}

void loop () {
  display.clearDisplay();
  display.setCursor(0, 0);

  digitalWrite(BUZZER_PIN, HIGH);

  DateTime now = rtc.now();
  machine->setCurrentTime(now);
  bool isBtnPressed = button.getSingleDebouncedPress();
  bool isAlarming = machine->isAlarming;

  String state = "";
  String alarmState = isAlarming ? "yes" : "no";
  String btnState = isBtnPressed ? "yes" : "no";
  state += machine->currentDisplay;
  state += ", is alarming : ";
  state += alarmState;
  state += ", btn pressed: ";
  state += btnState;
  Serial.println(state);

  // read the state of the pushbutton value:
  if (isBtnPressed) {
    machine->onWakeBtn();
  }
  if (isAlarming) {
    digitalWrite(BUZZER_PIN, HIGH);
  } else {
    digitalWrite(BUZZER_PIN, LOW);
  }

  display.print(machine->currentDisplay);

  display.display();
  delay(250);
}
