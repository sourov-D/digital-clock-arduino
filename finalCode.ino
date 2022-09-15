/*
 * ==============================================================================
 * Author       : Sourov Sharma
 * Date         : 13 July 2022
 * Last Modified: 31 July 2022
 * Description  : A simple digital clock. It shows the time, while it is
 *                possible to change the time. It also has a alarm function.
 * Version      : 0.3v beta
 * Changes      : 0.1v beta ( read from rtc and show it to the serial monitor)
 *                0.2v beta ( output the value from rtc to 7 seg display)
 *                0.3v beta ( change the time with input buttons)
 * ===============================================================================
 */

#include <TimeLib.h>
#include <TM1637.h>
#include <DS1307RTC.h>

#define CLK 2
#define DIO 3

/*
 * Global variables
 */
int hourValue = 0;      /* to hold the hour value while changing the time */
int minuteValue = 0;    // to hold the minute value while changing the time
int hourAlarm = 0;      // holds the alarm hour value
int minuteAlarm = 0;    // holds the alarm minute value

/*
 * Pins name
 */
int hourButton = 6;
int minuteButton = 5;
int change = 7;
int setAll = 8;
int alarm = 9;
int buzzer = 12;

bool alarmState = false;
bool alarmTurnOff = false;

tmElements_t clockTime;
TM1637 outDisplay(CLK, DIO);

void setup(){
  /*
   * setsup all the inputs
   */
  pinMode(minuteButton, INPUT_PULLUP);
  pinMode(hourButton, INPUT_PULLUP);
  pinMode(change, INPUT_PULLUP);
  pinMode(setAll, INPUT_PULLUP);
  pinMode(alarm, INPUT_PULLUP);
  pinMode(buzzer, OUTPUT);
  Serial.begin(9600);
  outDisplay.set();
  outDisplay.init();
}


void loop(){
  RTC.read(clockTime);                                                      /* reads time from RTC */
  displayOut(clockTime.Hour, clockTime.Minute);                             /* displays the time */

  if((clockTime.Hour == hourAlarm) && (clockTime.Minute == minuteAlarm)){
    if(alarmTurnOff == false){
      alarmState = true;
    }
    while(alarmState){
      if(digitalRead(minuteButton) == LOW){
        alarmState = false;
        alarmTurnOff = true;
        tone(buzzer, 1000);
        delay(100);
        noTone(buzzer);
        delay(100);
      }
    } 
  }
  
  if(digitalRead(alarm) == LOW){
      Serial.println("setting alarm");
      setAlarm();
      Serial.println("alarm set");
  }
  
  if(digitalRead(change) == LOW){
    /*
     * change time
     */
    changeTime();
  }

  if((digitalRead(hourButton) == LOW) && (digitalRead(minuteButton) == LOW)){
    while(1){
      RTC.read(clockTime);
      displayOutSec(clockTime.Minute, clockTime.Second);
      if(digitalRead(setAll) == LOW){
        break;
      }
      //Serial.println(clockTime.Second);
      delay(120);
    }
  } 
}

/*
 * ==========================================
 * All the other functions
 * ==========================================
 */

/*
 * To blink the display
 */
void blinkDis(){
  outDisplay.display(0, 8);
  outDisplay.display(1, 8);
  outDisplay.point(1);
  outDisplay.display(2, 8);
  outDisplay.display(3, 8);
  delay(300);
}

/*
 * function to display minute and seconds
 */

void displayOutSec(int Minute, int Second){
  outDisplay.display(0, (Minute/10));
  outDisplay.display(1, (Minute%10));
  outDisplay.point(1);
  outDisplay.display(2, (Second/10));
  outDisplay.display(3, (Second%10));
}

/*
 * To print to the display
 */
void displayOut(int Hour, int Minute){
  outDisplay.display(0, (Hour/10));
  outDisplay.display(1, (Hour%10));
  outDisplay.point(1);
  outDisplay.display(2, (Minute/10));
  outDisplay.display(3, (Minute%10));
}

/*
 * To change the time with user input
 */
void changeTime(){
  blinkDis();
  displayOut(1,1);
  delay(300);
  
  displayOut(clockTime.Hour, clockTime.Minute);
  hourValue = clockTime.Hour;
  minuteValue = clockTime.Minute;
  while(digitalRead(setAll)){
    if(!digitalRead(minuteButton)){
      minuteValue++;
      if(minuteValue < 60){
        displayOut(hourValue,minuteValue);
      }else{
        minuteValue = 0;
        displayOut(hourValue,minuteValue);
      }
    }
    if(!digitalRead(hourButton)){
      hourValue++;
      if(hourValue < 24){
        displayOut(hourValue,minuteValue);
        
      }else{
        hourValue = 0;
        displayOut(hourValue,minuteValue);
      }
    }
    if(!digitalRead(change)){
      delay(200);
      return;
    }
    // delay between inputs
    delay(120);
  }
  clockTime.Hour = hourValue;
  clockTime.Minute = minuteValue;
  clockTime.Second = 0;
  RTC.write(clockTime);
}

void setAlarm(){
  blinkDis();
  displayOut(1,1);
  delay(300);
  
  displayOut(clockTime.Hour, clockTime.Minute);
  hourAlarm = clockTime.Hour;
  minuteAlarm = clockTime.Minute;
  while(digitalRead(setAll)){
    /*
     * loops untill set input is low
     */
    if(!digitalRead(minuteButton)){
      minuteAlarm++;
      if(minuteAlarm < 60){
        displayOut(hourAlarm,minuteAlarm);
      }else{
        minuteAlarm = 0;
        displayOut(hourAlarm,minuteAlarm);
      }
    }
    if(!digitalRead(hourButton)){
      hourAlarm++;
      if(hourAlarm < 24){
        displayOut(hourAlarm,minuteAlarm);
        
      }else{
        hourAlarm = 0;
        displayOut(hourAlarm,minuteAlarm);
      }
    }
    /*
     * delays between input checks
     */
    delay(120);
  }
  // sets the value to false to be activated again
  alarmTurnOff = false;
  Serial.println(alarmTurnOff);
  Serial.print(hourAlarm);
  Serial.print(minuteAlarm);
  Serial.print("");
  blinkDis();
  displayOut(1,1);
  delay(300);
}
