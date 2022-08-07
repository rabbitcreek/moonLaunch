/*  Simple test for ebay 128x64 tft run via I2C on a 
    Arduino Pro Mini 3.3V (328P), with ebay DS3231 real time
    clock module.
    
    The sketch will calculate the current tide height for
    the site (assuming clock is set correctly) and display
    the tide height and time on a ssd1306-controller tft
    128x64 display. Time updates every second, tide updates
    as the last significant digit changes (10-20 seconds). 
*/
#include <FastLED.h>
#include <ESP_FlexyStepper.h>
#include <Wire.h> 
#include <TFT_eSPI.h> // Graphics and font library for ST7735 driver chip
#include <SPI.h> // Required for RTClib to compile properly
#include <RTClib.h> // From https://github.com/millerlp/RTClib
TFT_eSPI tft = TFT_eSPI();  // Invoke library, pins defined in User_Setup.h
// Real Time Clock setup
RTC_DS3231 RTC; // Uncomment when using this chip
int dS = 0;
DateTime futureHigh;
DateTime futureLow;
DateTime future;
const int SPEED_IN_STEPS_PER_SECOND = 300;
const int ACCELERATION_IN_STEPS_PER_SECOND = 800;
const int DECELERATION_IN_STEPS_PER_SECOND = 800;
int slope;
int i = 0;
int zag = 0;
bool gate = 1;
float tidalDifference=0;
 float pastResult;
 bool bing = 1;
 bool futureLowGate = 0;
 bool futureHighGate = 0;
const int MOTOR_STEP_PIN = 33;
const int MOTOR_DIRECTION_PIN = 25;
const int LIMIT_SWITCH_PIN = 17; 
// create the stepper motor object
ESP_FlexyStepper stepper;


// Tide calculation library setup.
// Change the library name here to predict for a different site.
//#include "TidelibSanDiegoSanDiegoBay.h"
#include "TidelibValdezPrinceWilliamSoundAlaska.h"
// Other sites available at http://github.com/millerlp/Tide_calculator
TideCalc myTideCalc; // Create TideCalc object 

// 0X3C+SA0 - 0x3C or 0x3D for tft screen on I2C bus




long oldmillis; // keep track of time
float results; // tide height
DateTime now; // define variable to hold date and time
// Enter the site name for display. 11 characters max
char siteName[11] = "SITKA";  
//------------------------------------------------------------------------------
void waterLevel(DateTime now, DateTime futureHigh, DateTime futureLow,int dS){
  bool hiLow;
  if( int(futureHigh.unixtime() - futureLow.unixtime()) < 0) hiLow = 1;
  if( int(futureHigh.unixtime() - futureLow.unixtime()) > 0) hiLow = 0;
  float resultsHigh = myTideCalc.currentTide(futureHigh); 
  float resultsLow = myTideCalc.currentTide(futureLow); 
  //now = (now.unixtime() - 3600);
  
  float resultsNow = myTideCalc.currentTide(now);
  tft.fillScreen(TFT_BLACK ); 
 tft.setCursor(0,0,2);
 tft.setTextColor(TFT_WHITE,TFT_BLACK);  tft.setTextSize(2);
  if(hiLow){
    
    tft.print("HI  ");
    tft.print(resultsHigh, 1);
    tft.println(" ft");
    tft.println("  Rising");
    tft.println();
    tft.print("NOW ");
    tft.print(resultsNow, 1);
    tft.print(" ft");
  }
    else {
    tft.print("NOW  ");
    tft.print(resultsNow, 1);
    tft.println(" ft");
    tft.println();
    tft.println(" Dropping");
   
    tft.print("LOW ");
    tft.print(resultsLow, 1);
    tft.print(" ft");
     
    }
    
  }
  
void graphTide(DateTime now, DateTime futureHigh, DateTime futureLow,int dS){
  bool hiLow;
   tft.fillScreen(TFT_BLACK ); 
      tft.setCursor(0,0,2); 
  
  int hourDifference;
   if(futureHigh.unixtime() > futureLow.unixtime() ){
      hourDifference =  ((futureLow.unixtime() - now.unixtime())/3600);
     hiLow = 0;
   }
   else {
     hourDifference =  ((futureHigh.unixtime() - now.unixtime())/3600);
    hiLow = 1;
   }
   
   
   tft.print(hourDifference);
   tft.println(" HOURS TO");
   if(hiLow)tft.println("   HIGH"); 
   else tft.println("    LOW");
   
    //tft.set1X();
    tft.println();
    hourDifference = map(hourDifference, 0,6,20,1);
    hourDifference = constrain(hourDifference,1,20);
    for(int printer = 0; printer < 20; printer++) tft.print("=");
    tft.println();
    for(int printer = 0; printer < hourDifference; printer ++) {
    tft.print("I");
    }
    tft.println();
    for(int printer = 0; printer < 20; printer++) tft.print("=");
    
}

void tftScreen(DateTime now, DateTime futureHigh, DateTime futureLow,int dS){
   bool hiLow;
   if( int(futureHigh.unixtime() - futureLow.unixtime()) < 0) hiLow = 1;
   if( int(futureHigh.unixtime() - futureLow.unixtime()) > 0) hiLow = 0;
  tft.setCursor(0,0,2);
  tft.setTextColor(TFT_WHITE,TFT_BLACK);  tft.setTextSize(2);
  //tft.clear();
  //tft.home();
 // tft.set2X();
 
  if (hiLow) {
    tft.print("HI     ");
    tft.println("LOW");
    tft.print(futureHigh.hour() +2, DEC); 
    tft.print(":");
  if (futureHigh.minute() < 10) {
    tft.print("0");
    tft.print(futureHigh.minute());
   }
  else if (futureHigh.minute() >= 10) {
    tft.print(futureHigh.minute());
  }
  tft.print(" ");
    tft.print(futureLow.hour() + 2, DEC); 
  tft.print(":");
  if (futureLow.minute() < 10) {
    tft.print("0");
    tft.print(futureLow.minute());
   }
  else if (futureLow.minute() >= 10) {
    tft.print(futureLow.minute());
  }
    
  }
  else {
    tft.print("LOW     ");
    tft.println("HI");
    tft.print(futureLow.hour() + 2, DEC); 
  tft.print(":");
  if (futureLow.minute() < 10) {
    tft.print("0");
    tft.print(futureLow.minute());
   }
  else if (futureLow.minute() >= 10) {
    tft.print(futureLow.minute());
  }
  tft.print(" ");
   tft.print(futureHigh.hour() +2, DEC); 
   tft.print(":");
  if (futureHigh.minute() < 10) {
    tft.print("0");
    tft.print(futureHigh.minute());
   }
  else if (futureHigh.minute() >= 10) {
    tft.print(futureHigh.minute());
  }
  }
  tft.println();
  
  
 
results = myTideCalc.currentTide(now);
      //tft.fillScreen(TFT_BLACK ); 
      //tft.setCursor(0,0,2);
      //tft.print("this:    ");
     // tft.println(results, 3);

      //tft.home();
      //tft.set2X();  // Enable large font    
      //tft.println(); // Print site name, move to next line
     // tft.print("  ");
      //tft.println("SITKA"); // print tide ht. to 3 decimal places
      //tft.println(" ft");
      //tft.set1X(); // Enable normal font
     //tft.println("  Tide Location");
  tft.print(now.year(), DEC);
  tft.print("/");
  tft.print(now.month(), DEC); 
  tft.print("/");
  tft.print(now.day(), DEC); 
  tft.print("  ");
  tft.print(now.hour() + (2 * dS), DEC); 
  tft.print(":");
  if (now.minute() < 10) {
    tft.print("0");
    tft.print(now.minute());
   }
  else if (now.minute() >= 10) {
    tft.print(now.minute());
  }
 tft.print(" ");

}

void setup() {
  Wire.begin();  
  RTC.begin();
  Serial.begin(57600);
  //Serial.begin(115200);
  pinMode(LIMIT_SWITCH_PIN, INPUT_PULLUP);
  //RTC.adjust(DateTime(F(__DATE__), F(__TIME__)));
  
  //RTC.adjust(DateTime(2021, 8, 26, 13, 8, 0)); 
  tft.init();
  tft.setRotation(1);
  tft.fillScreen(TFT_BLACK );
  tft.setTextFont(4);
  
  // Start up the tft display
  
  stepper.connectToPins(MOTOR_STEP_PIN, MOTOR_DIRECTION_PIN);
  stepper.setSpeedInStepsPerSecond(SPEED_IN_STEPS_PER_SECOND);
  stepper.setAccelerationInStepsPerSecondPerSecond(ACCELERATION_IN_STEPS_PER_SECOND);
  stepper.setDecelerationInStepsPerSecondPerSecond(DECELERATION_IN_STEPS_PER_SECOND);
  stepper.startAsService();
  
  oldmillis = millis() - 15000;
  findEndPoint();
  
}
void findEndPoint(){
   
  stepper.setSpeedInStepsPerSecond(250);
  stepper.setAccelerationInStepsPerSecondPerSecond(800);
  while(digitalRead(LIMIT_SWITCH_PIN)){
    stepper.moveRelativeInSteps(-60);
  }
   stepper.emergencyStop();
   stepper.setCurrentPositionAsHomeAndStop();
   stepper.moveToPositionInSteps(400);
   stepper.setCurrentPositionAsHomeAndStop();
}
//------------------------------------------------------------------------------
void loop() {
  // The main statement block will run once per second
 EVERY_N_MINUTES(15){
      
      now = RTC.now(); // update time
      
       if((now.month()>3&&now.month()<11)||(now.month()==3&&now.day()>11)||(now.month()==11&&now.day()<6))dS=1;
   now = (now.unixtime() - dS*3600);
   
   float resultsStepper = myTideCalc.currentTide(now);
   resultsStepper = resultsStepper  + 3;
   resultsStepper = constrain( resultsStepper, 0, 19);
   resultsStepper = resultsStepper * 10;
   int moveStepper = map(resultsStepper,0,190,0,6000);
  stepper.setSpeedInStepsPerSecond(250);
  stepper.setAccelerationInStepsPerSecondPerSecond(800);
  stepper.moveToPositionInSteps(0);
  //delay(5000);
  Serial.print("moveStepper");
  Serial.println(moveStepper);
  tft.fillScreen(TFT_BLACK);
   tft.setTextColor(TFT_YELLOW, TFT_BLACK);
   tft.setTextFont(4);
   tft.setCursor(0, 0);
   tft.println(moveStepper);
  //moveStepper *= -1;
  stepper.moveToPositionInSteps(moveStepper);
  delay(5000);
 
   
  
  }
}
