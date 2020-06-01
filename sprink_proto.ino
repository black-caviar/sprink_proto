#include <LiquidCrystal.h>
#include <avr/interrupt.h>
#include <Wire.h>
#include "menu.h"

#define O_STROBE 13 //0
#define O_DATA 12   
#define O_CLK 11
#define O_OE 0 //unused

#define O_RS 8
#define O_EN 7
#define O_D1 6
#define O_D2 5
#define O_D3 21
#define O_D4 20

#define SDA A4
#define SCL A5

#define B_ENTER A3
#define B_BACK A0
#define B_RIGHT A2
#define B_LEFT A1

#define ON 1
#define OFF 0

//const int rs = 8, en = 7, d4 = 6, d5 = 5, d6 = 21, d7 = 20;
//LiquidCrystal lcd(rs, en, d4, d5, d6, d7);
LiquidCrystal lcd(O_RS, O_EN, O_D1, O_D2, O_D3, O_D4);
union {
  struct {
    uint8_t right : 1;
    uint8_t left : 1;
    uint8_t enter : 1;
    uint8_t back : 1;
  };
  uint8_t raw;
} interface;

uint8_t SPRINKLER_STATE;

unsigned long last_button_interrupt;
#define INTERRUPT_DELAY 100

ISR (PCINT1_vect) {
  if (millis() - last_button_interrupt < INTERRUPT_DELAY)
    ;
  else {
    last_button_interrupt = millis();
    interface.back = (0x1 & PINC) ? 0 : 1;
    interface.left = (0x2 & PINC) ? 0 : 1;
    interface.right = (0x4 & PINC) ? 0 : 1;
    interface.enter = (0x8 & PINC) ? 0 : 1;
  }
}

void set_sprinkler(uint8_t n, uint8_t state) {
  SPRINKLER_STATE = SPRINKLER_STATE & (state << n);
  write_sprinklers(SPRINKLER_STATE);
}

void write_sprinklers(uint8_t state) {
  Serial.print("Writing to sprinkler ");
  Serial.println(SPRINKLER_STATE);
  digitalWrite(O_STROBE, LOW);
  for (int i=7; i>=0; i--) {
    digitalWrite(O_CLK, LOW);
    digitalWrite(O_DATA, 1 & (SPRINKLER_STATE >> i));
    delayMicroseconds(10);
    digitalWrite(O_CLK, HIGH);
  }
  digitalWrite(O_STROBE, HIGH);
}

void write_RTC() {
  Wire.beginTransmission(0x68);

  uint8_t temp[] = {RTC_time.seconds, RTC_time.minutes, RTC_time.hours, RTC_time.weekday, RTC_time.date, RTC_time.month, RTC_time.year};
  Wire.write(temp, 7);
  

  Wire.endTransmission();
}

struct menu *menu_current;

//struct menu menu_main;
//struct menu menu_manual;

void setup() {
  write_sprinklers(0);
  Serial.begin(57600);
  Serial.println("Serial Connection With Sprinkler Controller");
  Wire.begin();
  
  lcd.begin(16, 2);
  lcd.print("WELCOM SPRINKLER");

  pinMode(O_STROBE, OUTPUT);
  pinMode(O_DATA, OUTPUT);
  pinMode(O_CLK, OUTPUT);
  pinMode(O_OE, OUTPUT);

  pinMode(B_ENTER, INPUT_PULLUP);
  pinMode(B_BACK, INPUT_PULLUP);
  pinMode(B_RIGHT, INPUT_PULLUP);
  pinMode(B_LEFT, INPUT_PULLUP);

  cli(); //turn of interrupts
  
  PCICR = 0x00;
  PCMSK1 = 0x00;
  PCICR  |= 0b00000010; //enable PCIE1, inerrupts for analog pins
  PCMSK1 |= 0b00001111; //enables interrups for 3 lowest analog pins
  sei(); //enable interrupts

  menu_main = {.title = "main", .parent = NULL, .children = NULL, .num_children = 2, .cur_seg = 0, .handler = menu_handler_main};
  menu_manual = {.title = "Manual Control", .parent = &menu_main, .children = NULL, .num_children = 6, .cur_seg = 0, .handler = menu_handler_manual};
  menu_settime = {"Date and Time", &menu_main, NULL, 5, 0, menu_handler_settime};
  
  main_menu_children[0] = &menu_manual;
  main_menu_children[1] = &menu_settime;
  menu_main.children = main_menu_children;

  //Serial.print("Main Menu title: ");
  //Serial.println(menu_main.title);
    
  menu_current = &menu_main;
  //menu_current->handler(MENU_INIT);
  menu_current->handler(MENU_MOVE);

  //Serial.print("Main Menu title: ");
  //Serial.println(menu_current->title);
  //Serial.println(menu_current->num_children);
  //Serial.println(menu_current->cur_seg);
}

void loop() {
  // put your main code here, to run repeatedly:
  if (interface.raw) {
    //cli();
    //this is not endian safe code
    //replace constant with variable such as PCMSDK1
    Serial.print("interrupt received ");
    Serial.println(interface.raw);
    if (interface.right) {
      Serial.println(menu_current->title);
      Serial.print("cur_seg ");
      Serial.println(menu_current->cur_seg);
      if (menu_current->cur_seg < menu_current->num_children - 1) {
        Serial.println("Path A");
        (menu_current->cur_seg)++;
      }
      else {
        Serial.println("Path B");
        menu_current->cur_seg = (int) 0;  
      }
      interface.right = 0;
      Serial.print("cur_seg post update ");
      Serial.println(menu_current->cur_seg);
      Serial.println(menu_current->title);
      (menu_current->handler)(MENU_MOVE);
    }
    else if (interface.left) {
      if (menu_current->cur_seg > 0)
        menu_current->cur_seg--;
      else
        menu_current->cur_seg = (menu_current->num_children)-1; 
      interface.left = 0;  
      menu_current->handler(MENU_MOVE);
    }
    else if (interface.enter) {
      interface.enter = 0;
      menu_current->handler(MENU_SLKT);
    }
    else if (interface.back) {
      //menu_current = menu_current->parent;
      interface.back = 0;
      menu_current->handler(MENU_BACK);
    }
    //or more generally to ignore all simulateneous inputs
    //interface.raw = 0;
    //sei();
  }
}
