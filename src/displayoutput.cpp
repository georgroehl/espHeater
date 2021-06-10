#include "displayoutput.h"
#include <Arduino.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

int calculateWhitespaces(int line_length, int widget_left_length, int widget_right_length){
  if (widget_left_length+widget_right_length<=line_length){
    return line_length-widget_left_length-widget_right_length;
  }
    return 0;
}

String createWhitespaces(int line_length, int left_length, int right_length){
  int amount = calculateWhitespaces(line_length, left_length, right_length);
  String whitespaces = "";
  for (int i=0;i<amount;i++){
    whitespaces+=" ";
  }
  return whitespaces;
}

String createPrintableLine(int line_length, String leftString, String rightString){
  return leftString + createWhitespaces(line_length, leftString.length(), rightString.length()) + rightString;
}

void drawMainView(Adafruit_SSD1306 *display, String mainviewString){
  //Mainview is one line with 9 chars
  int ofx = 0;
  display->clearDisplay();
  display->setTextSize(3);
  display->setTextColor(SSD1306_WHITE);
  display->setCursor(ofx,20);
  String line1 = createPrintableLine(6,"",mainviewString);
  display->print(line1);
  display->display();
}

void drawProgressView(Adafruit_SSD1306 *display, int progress){
  if (progress<0){return;}
  if (progress>100){return;}
  int start_offset = 10;
  if (progress==0){start_offset=0;}
  int16_t screenwidth = 127;
  int16_t screenheight = 64;
  int16_t progressInScreenwidth = (screenwidth-start_offset)*progress/100;
  display->clearDisplay();
  display->fillRect(0,0,start_offset+progressInScreenwidth,screenheight,SSD1306_WHITE);
  display->display();
}


void drawStatsView(Adafruit_SSD1306 *display, String timerValue, String stopwatchValue, String batteryValue){
  //circa 4 lines with 18-20 chars

  String line1 = createPrintableLine(18,"Auto-Shutdown:",timerValue);
  String line2 = createPrintableLine(18,"Timer:",stopwatchValue);
  String line3 = createPrintableLine(18,"Battery:",batteryValue);

  int ofx = 13;
  display->clearDisplay();
  display->setTextSize(1);
  display->setTextColor(SSD1306_WHITE);
  display->setCursor(ofx,17);
  
  display->print(line1);
  display->setCursor(ofx,27);
  display->print(line2);
  display->setCursor(ofx,37);
  display->print(line3);

  display->display();
}

void drawMenuL1(Adafruit_SSD1306 *display, String entryName){
  int ofx = 15;
  display->clearDisplay();
  display->setTextSize(1);
  display->setTextColor(SSD1306_WHITE);
  display->setCursor(ofx,20);
  String current_item = "";
  current_item = entryName;

  display->println(current_item);
  display->setTextSize(1);
  display->setCursor(ofx,47);
  display->print("Menu:  ");
  display->setTextColor(SSD1306_BLACK, SSD1306_WHITE);
  display->write(16);
  display->setTextColor(SSD1306_WHITE);
  display->print("  ");
  display->write(16); //Long Press Sign
  display->println("Select");
  display->setCursor(ofx,56);
  display->print("Power: ");
  display->setTextColor(SSD1306_BLACK, SSD1306_WHITE);
  display->write(17);
  display->drawLine(0, 46, display->width()-1, 46, SSD1306_WHITE);
  display->display();
}


void drawChangeTimerApp(Adafruit_SSD1306 *display, int offtimer){
  int off_timer = offtimer/1000;
  int ofx = 15;
  display->clearDisplay();
  display->setTextSize(1);
  display->setTextColor(SSD1306_WHITE);
  display->setCursor(ofx,5);
  display->println("Auto-Off Timer:");
  display->setTextSize(2);
  display->setCursor(40,20);
  display->println(String(off_timer) + "s");
  display->setTextSize(1);
  display->setCursor(ofx,47);
  display->print("Menu:  ");
  display->setTextColor(SSD1306_BLACK, SSD1306_WHITE);
  display->print("+1s");
  display->setTextColor(SSD1306_WHITE);
  display->print("  ");
  display->write(16); //Long Press Sign
  display->println("Back");
  display->setCursor(ofx,56);
  display->print("Power: ");
  display->setTextColor(SSD1306_BLACK, SSD1306_WHITE);
  display->print("-1s");
  display->drawLine(0, 46, display->width()-1, 46, SSD1306_WHITE);
  display->display();
}


