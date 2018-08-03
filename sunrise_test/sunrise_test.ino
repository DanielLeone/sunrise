#include <AUnit.h>
#include <Sunrise.h>
#include <RTClib.h>

void setup() {
  Serial.begin(9600);
}

void loop() {
  aunit::TestRunner::run();
}

test(string_padding) {
  assertEqual(padTo("hi", 3), " hi");
  assertEqual(padTo("hi", 5), "   hi");
  assertEqual(padTo("hi", 1), "hi");
}

test(format_time) {
  assertEqual(formatTime(DateTime(2016, 1, 1, 3, 40, 1)), "03:40:01");
  assertEqual(formatTime(DateTime(0, 0, 0, 23, 59, 59)), "23:59:59");
}


test(happy_path) {
  // current time 3:40am, alarm at 5am
  AlarmStateMachine s (DateTime(2016, 1, 1, 3, 40, 1), 5, 0);
  assertEqual(s.isAlarming, false);
  assertEqual(s.currentDisplay, "03:40:01");

  // move the time a little
  s.setCurrentTime(DateTime(2016, 1, 1, 3, 40, 2));

  // should change the display, but be in the same alarming mode
  assertEqual(s.isAlarming, false);
  assertEqual(s.currentDisplay, "03:40:02");

  // change the time to start the alarm
  s.setCurrentTime(DateTime(2016, 1, 1, 5, 0, 1));
  assertEqual(s.isAlarming, true);
  assertEqual(s.currentDisplay, "WAKE UP");

  // a few seconds pass, we're still alarming
  s.setCurrentTime(DateTime(2016, 1, 1, 5, 0, 2));
  s.setCurrentTime(DateTime(2016, 1, 1, 5, 0, 3));
  s.setCurrentTime(DateTime(2016, 1, 1, 5, 0, 4));
  assertEqual(s.isAlarming, true);
  assertEqual(s.currentDisplay, "WAKE UP");

  // we finally hit the wake button, show the current time again
  s.onWakeBtn();
  assertEqual(s.isAlarming, false);
  assertEqual(s.currentDisplay, "05:00:04");

  // time goes on successfully
  s.setCurrentTime(DateTime(2016, 1, 1, 5, 0, 5));
  assertEqual(s.isAlarming, false);
  assertEqual(s.currentDisplay, "05:00:05");

  // and we continue to the next morning
  s.setCurrentTime(DateTime(2016, 1, 1, 6, 0, 0));
  s.setCurrentTime(DateTime(2016, 1, 1, 7, 0, 0));
  s.setCurrentTime(DateTime(2016, 1, 1, 8, 0, 0));
  s.setCurrentTime(DateTime(2016, 1, 2, 0, 0, 0));
  assertEqual(s.isAlarming, false);
  assertEqual(s.currentDisplay, "00:00:00");

  // finally, wake up time comes again
  s.setCurrentTime(DateTime(2016, 1, 2, 5, 0, 0));
  assertEqual(s.isAlarming, true);
  assertEqual(s.currentDisplay, "WAKE UP");
  
  // again a few seconds pass
  s.setCurrentTime(DateTime(2016, 1, 2, 5, 0, 1));
  s.setCurrentTime(DateTime(2016, 1, 2, 5, 0, 2));
  s.setCurrentTime(DateTime(2016, 1, 2, 5, 0, 3));
  s.setCurrentTime(DateTime(2016, 1, 2, 5, 0, 4));

  assertEqual(s.isAlarming, true);
  assertEqual(s.currentDisplay, "WAKE UP");

  // until we hit the wake button and go back to normal
  s.onWakeBtn();
  assertEqual(s.isAlarming, false);
  assertEqual(s.currentDisplay, "05:00:04");
}

test(should_not_start_alarming_on_initialization_when_alarm_passed) {
  // current time 5:00am, alarm at 4:20am -- should not start alarming straight away, even though the time has passed
  AlarmStateMachine s (DateTime(2016, 1, 1, 5, 0, 0), 4, 20);
  assertEqual(s.isAlarming, false);
  assertEqual(s.currentDisplay, "05:00:00");
  // 6am
  s.setCurrentTime(DateTime(2016, 1, 1, 6, 0, 0));
  assertEqual(s.isAlarming, false);
  // 11pm
  s.setCurrentTime(DateTime(2016, 1, 1, 23, 0, 0));
  assertEqual(s.isAlarming, false);
  // next day!
  s.setCurrentTime(DateTime(2016, 1, 1, 0, 0, 0));
  assertEqual(s.isAlarming, false);
  // 4am
  s.setCurrentTime(DateTime(2016, 1, 1, 4, 0, 0));
  assertEqual(s.isAlarming, false);
  // 4:20am alarm time!
  s.setCurrentTime(DateTime(2016, 1, 1, 4, 21, 0));
  assertEqual(s.isAlarming, true);
  assertEqual(s.currentDisplay, "WAKE UP");
}

test(should_not_start_alarming_on_initialization_when_alarm_soon) {
  // current time 4:12am, alarm at 4:20am
  AlarmStateMachine s (DateTime(2016, 1, 1, 4, 12, 0), 4, 20);
  assertEqual(s.isAlarming, false);
  assertEqual(s.currentDisplay, "04:12:00");
  s.setCurrentTime(DateTime(2016, 1, 1, 4, 20, 0));
  assertEqual(s.isAlarming, true);
  assertEqual(s.currentDisplay, "WAKE UP");
}

test(should_say_error_if_invalid_alarm_hour) {
  // bogus alarm hour
  AlarmStateMachine s (DateTime(2016, 1, 1, 4, 12, 0), 453, 0);
  assertEqual(s.isAlarming, false);
  assertEqual(s.currentDisplay, "ERROR");
}

test(should_say_error_if_invalid_alarm_minute) {
  // bogus alarm minute
  AlarmStateMachine s (DateTime(2016, 1, 1, 4, 12, 0), 3, 43242);
  assertEqual(s.isAlarming, false);
  assertEqual(s.currentDisplay, "ERROR");
}
