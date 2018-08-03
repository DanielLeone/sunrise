/*
  Sunrise - Library for writing alarm clocks
  Created by Daniel Leone, 2018.
  Released into the public domain.
*/

#include "Arduino.h"
#include "Sunrise.h"
#include <RTClib.h>

bool isTimeBefore(DateTime time, int hour, int minute) {
    return time.hour() < hour || time.hour() == hour && time.minute() < minute;
}

bool isTimeAfterOrEqual(DateTime time, int hour, int minute) {
    return time.hour() > hour || time.hour() == hour && time.minute() >= minute;
}

AlarmStateMachine::AlarmStateMachine(DateTime time, int hour, int minute) {
    if (hour < 0 || hour > 23 || minute < 0 || minute > 59) {
        // invalid params, error mode
        mode = 4;
    } else {
        // start in dismissed mode (whether alarm time coming up soon or has passed for today)
        mode = 2;
        alarmHr = hour;
        alarmMin = minute;
    }
    setCurrentTime(time);
}

void AlarmStateMachine::onModifyMinBtn() {

}

void AlarmStateMachine::onModifyHrBtn() {

}

void AlarmStateMachine::onModeBtn() {

}

void AlarmStateMachine::onWakeBtn() {
    if (mode == 1) {
        mode = 2;
        isAlarming = false;
        currentDisplay = formatTime(currentTime);
    }
}

void AlarmStateMachine::setCurrentTime(DateTime time) {
    currentTime = time;

    switch (mode) {
        case 0: {
            // waiting
            if (isTimeAfterOrEqual(currentTime, alarmHr, alarmMin)) {
                // time to fire the alarm!
                mode = 1;
                isAlarming = true;
                currentDisplay = "WAKE UP";
            } else {
                isAlarming = false;
                currentDisplay = formatTime(currentTime);
            }
            break;
        }
        case 1: {
            // alarming
            isAlarming = true;
            currentDisplay = "WAKE UP";
            break;
        }
        case 2: {
            // dismissed
            if (isTimeBefore(currentTime, alarmHr, alarmMin)) {
                // time to go from dismissed mode back into waiting mode
                mode = 0;
            }
            currentDisplay = formatTime(currentTime);
            isAlarming = false;
            break;
        }
        case 3: {
            // setting
            currentDisplay = "SETTING";
            isAlarming = false;
            break;
        }
        case 4: {
            // error
            isAlarming = false;
            currentDisplay = "ERROR";
            break;
        }
    }
}


String padZero(int num) {
    String s = "";
    if (num < 10) {
        s += '0';
    }
    s += num;
    return s;
}

String padTo(String str, const size_t num, const char paddingChar) {
    String x = str;
    int toAdd = num - str.length();
    if (toAdd > 0) {
        String pre = "";
        while(toAdd--) {
            pre += paddingChar;
        }
        return pre + str;
    }
    return str;
}

String formatTime(DateTime time) {
    String str = "";
    str += padZero(time.hour());
    str += ":";
    str += padZero(time.minute());
    str += ":";
    str += padZero(time.second());
    return str;
}
