/*
  Sunrise - Library for writing alarm clocks
  Created by Daniel Leone, 2018.
  Released into the public domain.
*/
#ifndef Sunrise_h
#define Sunrise_h

#include "Arduino.h"
#include <RTClib.h>

class Sunrise {
  public:
    Sunrise();
    void setCurrentTime(DateTime time);
    void setAlarmTime(int hour, int minute);
  private:
    int _state;
};


class AlarmStateMachine {
    public:
        AlarmStateMachine(DateTime time, int hour, int minute);
        void onModifyMinBtn();
        void onModifyHrBtn();
        void onModeBtn();
        void onWakeBtn();
        void setCurrentTime(DateTime time);
        bool isAlarming;
        String currentDisplay;
    private:
        int mode; // 0 - waiting, 1 - alarming, 2 - dismissed, 3 - setting, 4 - error
        int alarmHr; // 0-23
        int alarmMin; // 0-59
        DateTime currentTime;

};

String padTo(String str, const size_t num, const char paddingChar = ' ');

String formatTime(DateTime time);

#endif