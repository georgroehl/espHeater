//Everything that draws on Screen
#include <Adafruit_SSD1306.h>

void drawMainView(Adafruit_SSD1306 *display, String mainviewString);
void drawMainView(Adafruit_SSD1306 *display, String mainviewString, String batteryValue, String tempString);
void drawProgressView(Adafruit_SSD1306 *display, int progress);
void drawStatsView(Adafruit_SSD1306 *display, String timerValue, String stopwatchValue, String batteryValue, String tempString);

void drawMenuL1(Adafruit_SSD1306 *display, String entryName);
void drawChangeTimerApp(Adafruit_SSD1306 *display, int offtimer);
