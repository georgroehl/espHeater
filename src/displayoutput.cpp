#include "displayoutput.h"
#include <Arduino.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

void drawMainView(Adafruit_SSD1306 *display){
  int ofx = 15;
  display->clearDisplay();
  display->setTextSize(2);
  display->setTextColor(SSD1306_WHITE);
  display->setCursor(ofx,30);
  String current_item = "";
  display->print("MAINVIEW");
  display->display();
}

void drawStatsView(Adafruit_SSD1306 *display){
  int ofx = 15;
  display->clearDisplay();
  display->setTextSize(1);
  display->setTextColor(SSD1306_WHITE);
  display->setCursor(ofx,30);
  display->print("STATS STATS STATS");
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


