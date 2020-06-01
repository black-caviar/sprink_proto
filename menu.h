#pragma once
//using namespace std;

#define MENU_INIT 0
#define MENU_MOVE 1
#define MENU_BACK 2
#define MENU_SLKT 3

#define NUM_ZONES 6

struct menu {
  const char *title;
  struct menu* parent;
  struct menu** children;
  int num_children;
  int cur_seg;
  void (*handler)(uint8_t);
};

struct datetime{
  union {
    struct {
      uint8_t seconds_1s : 4;
      uint8_t seconds_10s : 4;
    };
    uint8_t seconds;
  };
  union {
    struct {
      uint8_t minutes_1s : 4;
      uint8_t minutes_10s : 4;
    };
    uint8_t minutes;
  };
  union {
    struct {
      uint8_t hours_1s : 4;
      uint8_t hours_10s : 2;
      uint8_t hours_mode : 2;
    };
    uint8_t hours;
  };
  uint8_t weekday;
  union {
    struct {
      uint8_t date_1s : 4;
      uint8_t date_10s : 4;
    };
    uint8_t date;
  };
  union {
    struct {
      uint8_t month_1s : 4;
      uint8_t month_10s : 3;
      uint8_t century : 1;
    };
    uint8_t month;
  };
  union {
    struct {
      uint8_t year_1s : 4;
      uint8_t year_10s : 5;
    };
    uint8_t year;
  };
};

struct datetime RTC_time;

struct menu menu_main;
struct menu menu_manual;
struct menu menu_settime;
struct menu *main_menu_children[2];

const char *MONTH_NAMES[] = {"January", "February", "March", "April", "May", "June", "July", "August", "September", "October", 
  "November", "December"};

const char *WEEKDAY_NAMES[] = {"Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday", "Sunday"}; 
