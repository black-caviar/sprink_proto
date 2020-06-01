#include "menu.h"

extern uint8_t SPRINKLER_STATE;
extern struct menu *menu_current;

void menu_handler_main(uint8_t c) {
  struct menu &m = menu_main;
  if (c == MENU_SLKT) {
    lcd.clear();
    lcd.write("HELP HELP HELP");
    menu_current = m.children[m.cur_seg];
    Serial.println("JA:LSKFJALSJFAKJ:");
    Serial.println(menu_current->title);
    (menu_current->handler)(MENU_INIT);
  }
  if (c == MENU_BACK) 
    ;
  //else if (c == MENU_MOVE) {
  else {
    lcd.clear();
    lcd.print((m.children[m.cur_seg])->title);
  }
}

void menu_handler_manual(uint8_t c) {
  struct menu &m = menu_manual;
  if (c == MENU_INIT) {
    //menu_handler_manual(MENU_MOVE);
    menu_handler_manual(MENU_MOVE);
  }
  else if (c == MENU_MOVE) {
    lcd.clear();
    lcd.print("Zone ");
    lcd.setCursor(5,0);
    lcd.print(m.cur_seg + 1);
    if ((SPRINKLER_STATE >> m.cur_seg) & 1) {
      lcd.print("  On");
    }
    else 
      lcd.print(" Off");
    lcd.setCursor(0,1);
    lcd.print("On time");
    lcd.print(" 00:00:00");
  }
  else if (c == MENU_SLKT) {
    SPRINKLER_STATE = SPRINKLER_STATE ^ (1 << m.cur_seg);
    write_sprinklers(SPRINKLER_STATE);
    m.handler(MENU_MOVE);
  }
  else if (c == MENU_BACK) {
    lcd.clear();
    menu_current = m.parent;
    menu_current->handler(MENU_INIT);
  }
}



void menu_handler_settime(uint8_t c) {
  struct menu &m = menu_settime;
  if (c == MENU_INIT) {
    lcd.clear();
    uint8_t month = RTC_time.month_1s + 10*RTC_time.month_10s;
    month = 5;
    Serial.print("month");
    Serial.println(month);
    Serial.println(MONTH_NAMES[month]);
    lcd.print(MONTH_NAMES[month]);
    lcd.setCursor(9,0);
    uint8_t date = RTC_time.date_1s + 10*RTC_time.date_10s;
    lcd.print(date);
    lcd.setCursor(12, 0);
    lcd.print(20);
    uint8_t year = RTC_time.year_1s + 10*RTC_time.year_10s;
    lcd.print(year);

    lcd.setCursor(0,1);
    lcd.print(WEEKDAY_NAMES[RTC_time.weekday]);
    lcd.setCursor(11,1);
    uint8_t hour = RTC_time.hours_1s + 10*RTC_time.hours_10s;
    lcd.print(hour);
    lcd.print(':');
    uint8_t minute = RTC_time.minutes_1s + 10*RTC_time.minutes_10s;
    lcd.print(minute);
  }
  else if (c == MENU_MOVE) {
    
  }
  else if (c == MENU_SLKT) {
    //menu_settime->handler(MENU_INIT);
    menu_handler_settime(MENU_INIT);
  }
  else {
    lcd.clear();
    menu_current = m.parent;
  }
}
