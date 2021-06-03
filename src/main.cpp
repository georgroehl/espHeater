#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <OneButton.h>
#include "DHTesp.h"
#include <EEPROM.h>
#include "displayoutput.h"


#define NOT_PRESSED HIGH
#define PRESSED LOW

#define PIN_MOSFET D8
#define PIN_BATTVOLTAGE A0


//Screen
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

//Buttons
#define PIN_BUTTONMAIN D0
#define PIN_BUTTONMENU D6
OneButton btnmain = OneButton(PIN_BUTTONMAIN,true,true);
OneButton btnmenu = OneButton(PIN_BUTTONMENU,true,true);

enum MenuItem{
  NONE, MAIN, STATS, CONFIG
};
enum ConfigMenuItem{
  OFFTIMER, MAINVIEW, STATSVIEW, BACK
};

//=========================================================================
//  CURRENT CONFIGURATION
//=========================================================================
MenuItem currentL0 = MAIN;
ConfigMenuItem currentL1 = OFFTIMER;
ConfigMenuItem presentConfig = BACK;
bool userCurrentlyConfiguresPreferences = false;

//offtimer in milliseconds
int l2_offtimer [1] = {12000};
//mainvieitems: 0:Status, 1:Timer, 2:Countdown, 3:Voltage
int l2_mainviewitems [2];// = {0,1};
//statusviewitems: 0:Status, 1:Timer, 2:Countdown, 3:Voltage, 4:Temp
int l2_statusviewitems [4];// = {0,2,3,4};


unsigned long powerTimestamp;
unsigned long programTimestamp;

//=========================================================================
//  ADDITIONAL VARIABLES
//=========================================================================

int main_power = 0;
DHTesp dht;

int millisec;
int tseconds;
int seconds;
int times;
int screen_last_refresh;
String last_stopwatch_duration = "0:00";


int addresses [7];
#define RESET_EEPROM false


enum Widget{
  MAIN_POWER, TIMER, BATTERY_VOLTAGE, BATTERY_PERCENTAGE, CASE_TEMP, POWER_STATUS, AUTO_OFF_MS, EMPTY, TEXT1, TEXT2
};


//===============================================================================
//  Functions
//===============================================================================
void loadConfig() {
  EEPROM.begin(512);
  EEPROM.get(0, currentL0);
  EEPROM.get(4, currentL1);  
  EEPROM.get(8, l2_offtimer[0]);
  EEPROM.get(12, l2_mainviewitems[0]);
  EEPROM.get(16, l2_mainviewitems[1]);
  EEPROM.get(20, l2_statusviewitems[0]);
  EEPROM.get(24, l2_statusviewitems[1]);
  EEPROM.get(28, l2_statusviewitems[2]);
  EEPROM.get(32, l2_statusviewitems[3]);
  char ok[2+1];
  EEPROM.get(36, ok);
  EEPROM.end();
  if (String(ok) != String("OK")) {
    l2_offtimer[0] = 0;
    l2_mainviewitems[0] = 6;
    l2_mainviewitems[1] = 9;
    l2_statusviewitems[0] = 0;
    l2_statusviewitems[1] = 4;
    l2_statusviewitems[2] = 2;
    l2_statusviewitems[3] = 0;
  }
  Serial.println("Recovered Config:");
  Serial.println("currentL0: " + String(currentL0));
  Serial.println("currentL1: " + String(currentL1));
  Serial.println("and more.");
  Serial.println("l2_offtimer: " + String(l2_offtimer[0]));
  //Serial.print("l2_mainviewitems: right:" + String(l2_mainviewitems[0]));
  //Serial.println(", left:" + String(l2_mainviewitems[1]));
  //Serial.print("l2_statusviewitems: line1: " + String(l2_statusviewitems[0]));
  //Serial.println(", line2: " + String(l2_statusviewitems[1]));
  //Serial.print("l2_statusviewitems: line3: " + String(l2_statusviewitems[2]));
 // Serial.println(", line4: " + String(l2_statusviewitems[3]));
}

void saveConfig() {
  EEPROM.begin(512);
  EEPROM.put(0, currentL0);
  EEPROM.put(4, currentL1);  
  EEPROM.put(8, l2_offtimer[0]);
  EEPROM.put(12, l2_mainviewitems[0]);
  EEPROM.put(16, l2_mainviewitems[1]);
  EEPROM.put(20, l2_statusviewitems[0]);
  EEPROM.put(24, l2_statusviewitems[1]);
  EEPROM.put(28, l2_statusviewitems[2]);
  EEPROM.put(32, l2_statusviewitems[3]);
  char ok[2+1] = "OK";
  EEPROM.put(36, ok);
  EEPROM.commit();
  EEPROM.end();
}


static void powerOnIH(){
  digitalWrite(PIN_MOSFET, HIGH);
  main_power = 1;
  powerTimestamp = millis();
  display.invertDisplay(true);
  display.display();
}

static void powerOffIH(){
  digitalWrite(PIN_MOSFET, LOW);
  main_power = 0;
  display.invertDisplay(false);
  display.display();
}


void switchedMenuL1(){
  switch (currentL1)
  {
  case OFFTIMER:
    drawMenuL1(&display,"Auto-Off Timer");
    break;
  case MAINVIEW:
    drawMenuL1(&display,"Main View Setup");
    break;
  case STATSVIEW:
    drawMenuL1(&display,"Stats View Setup");
    break;
  case BACK:
    drawMenuL1(&display,"Back");
    break;
  }
}

static void switchedMenuL0(){
  switch (currentL0)
  {
  case MAIN:
    drawMainView(&display);
    break;
  case STATS:
    drawStatsView(&display);
    break;
  case CONFIG:
    switchedMenuL1();
    break;
  case NONE:
    display.clearDisplay();
    display.display();
    break;
  }  
}




static void offtimerbuttonup(){
  l2_offtimer[0]+=1000;
  drawChangeTimerApp(&display, l2_offtimer[0]);
}

static void offtimerbuttondown(){
  l2_offtimer[0]-=1000;
  drawChangeTimerApp(&display, l2_offtimer[0]);
}



static void handleMainClick() {
  Serial.println(userCurrentlyConfiguresPreferences);
  if (!userCurrentlyConfiguresPreferences){
    Serial.print("Main Power: ");
    Serial.println(main_power);
    if (main_power==1){
      powerOffIH();
    }
    else if (main_power==0){
      powerOnIH();
    }
  }
  else{ //userCurrentlyConfiguresPreferences==true
    switch (currentL1)
    {
      case OFFTIMER:
        offtimerbuttondown();
        break;
      default:
        Serial.println("Main Button Clicked in App");
        break;
    }
    saveConfig();
  }
}


static void handleMenuLongPress() {
  if (currentL0!=CONFIG){
    currentL0 = CONFIG;
    currentL1 = OFFTIMER;
    switchedMenuL1();
  }
  else{
    if (userCurrentlyConfiguresPreferences==false){
      userCurrentlyConfiguresPreferences=true;
      switch (currentL1)
      {
      case OFFTIMER:
        drawChangeTimerApp(&display, l2_offtimer[0]);
        break;
      case MAINVIEW:
        Serial.println("currentlyConfigures MAINVIEW");
        //config main view function
        break;
      case STATSVIEW:
        Serial.println("currentlyConfigures STATSVIEW");
        //config statsview function
        break;
      case BACK:
        Serial.println("back to MainView");
        userCurrentlyConfiguresPreferences = false;
        currentL1 = OFFTIMER;
        currentL0 = MAIN;
        switchedMenuL0();
        break;
      }
    }
    else{//userCurrentlyConfiguresPreferences 
      Serial.println("Exiting Configure Menu");
      userCurrentlyConfiguresPreferences = false;
      switchedMenuL1();
    }
  }
  
  saveConfig();
}

static void handleMenuClick() {
  if (userCurrentlyConfiguresPreferences==false){
    switch (currentL0)
    {
    case MAIN:
      currentL0 = STATS;
      Serial.println("L0:STATS");
      switchedMenuL0();
      break;
    case STATS:
      currentL0 = NONE;
      Serial.println("L0:NONE");
      switchedMenuL0();
      break;
    case NONE: 
      currentL0 = MAIN;
      Serial.println("L0:MAIN");
      switchedMenuL0();
      break;
    case CONFIG:
      Serial.println("L0:CONFIG");
      switch (currentL1)
      {
      case OFFTIMER:
        currentL1 = MAINVIEW;
        Serial.println("L1:MAINVIEW");
        break;
      case MAINVIEW:
        currentL1 = STATSVIEW;
        Serial.println("L1:STATSVIEW");
        break;
      case STATSVIEW:
        currentL1 = BACK;
        Serial.println("L1:BACK");
        break;
      case BACK:
        currentL1 = OFFTIMER;
        Serial.println("L1:OFFTIMER");
        break;
      }
      switchedMenuL1();
    }
  }
  else{
    switch (currentL1)
    {
    case OFFTIMER:
      offtimerbuttonup();
      break;
    default:
      Serial.println("Menu Button in App");
    }
  }
  saveConfig();
}



//===============================================================================
//  Initialization
//===============================================================================
void setup() {
  Serial.begin(9600);
  delay(2000);
  //Load Preferences from Flash
  EEPROM.begin(512);
  if ( RESET_EEPROM ) {
    for (int i = 0; i < 512; i++) {
      EEPROM.write(i, 0);
    }
    EEPROM.commit();
    delay(500);
  }
  loadConfig();

  //Buttons:
  pinMode(PIN_BUTTONMAIN, INPUT_PULLUP);
  pinMode(PIN_BUTTONMENU, INPUT_PULLUP);
  btnmain.attachClick(handleMainClick);
  btnmenu.attachClick(handleMenuClick);
  btnmenu.attachLongPressStart(handleMenuLongPress);

  //MOSFET init
  pinMode(PIN_MOSFET, OUTPUT);
  //MOSFET configute because otherwise pin is floating when powering on the IH
  digitalWrite(PIN_MOSFET, LOW); 

  //Screen
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { // I2C Address 0x3C for 128x32
    // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
    Serial.println(F("SSD1306 allocation failed")); 
    for(;;); // Don't proceed, loop forever
  }

  // Clear the buffer
  display.cp437(true);
  display.clearDisplay();
  display.setRotation(2);

  //display.setTextColor(SSD1306_BLACK, SSD1306_WHITE); // Draw 'inverse' text
  //display.invertDisplay(true);
}

//===============================================================================
//  Main
//===============================================================================
char rxByte = 0;

void loop() {
  unsigned int auto_off_ms = l2_offtimer[0];
  if ((millis()-powerTimestamp) >= auto_off_ms){
    if (main_power){
      //power is on for longer than variable auto_off_ms (default: 10s)
      powerOffIH();
    }
  }
  btnmain.tick();
  btnmenu.tick();
}
    
 