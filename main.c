/* USB Keyboard Firmware code for generic Teensy Keyboards
 * Copyright (c) 2012 Fredrik Atmer, Bathroom Epiphanies Inc
 * http://www.bathroomepiphanies.com
 *
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without
 * restriction, including without limitation the rights to use, copy,
 * modify, merge, publish, distribute, sublicense, and/or sell copies
 * of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS
 * BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
 * ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#include <avr/pgmspace.h>
#include "lib/usb_keyboard_debug.h"
#include "lib/print.h"
#include "hw_interface.h"

void init(void);
void send(void);
void key_press(uint8_t k);
void key_release(uint8_t k);
void make_dualrole_tap_impossible();
void make_dualrole_modifier_possible();
void debug_print(void);


extern uint8_t active_layer;
extern struct {uint8_t pressed; uint8_t bounce;} key[NKEY];
extern uint8_t queue[7];
extern uint8_t mod_keys;

extern uint32_t tick;
extern bool dualrole_tap_possible;
extern bool dualrole_modifier_possible;
extern uint32_t dualrole_tap_impossible_after_tick;
extern uint32_t dualrole_modifier_impossible_until_tick;

ISR(SCAN_INTERRUPT_FUNCTION) {
  poll_timer_disable();
  tick ++;
  if( dualrole_tap_possible && tick == dualrole_tap_impossible_after_tick )
    make_dualrole_tap_impossible();

  if( dualrole_tap_possible && !dualrole_modifier_possible && tick == dualrole_modifier_impossible_until_tick )
    make_dualrole_modifier_possible();

  for(uint8_t r = 0, k = 0; r < NROW; r++) {
    pull_row(r);
    for(uint8_t c = 0; c < NCOL; c++, k++) {
      key[k].bounce |= probe_column(c);
      if(key[k].bounce == 0b01111111 && !key[k].pressed)
        key_press(k);
      if(key[k].bounce == 0b10000000 &&  key[k].pressed)
        key_release(k);

      key[k].bounce <<= 1;
    }
  }
  release_rows();
  // if(mod_keys == (uint8_t)(KC_LSFT | KC_RSFT))
  //   jump_bootloader();
  update_leds(keyboard_leds);

  /*if(active_layer == 1)*/
    /*PORTB = PORTB & 0b01111111;*/
  /*else*/
    /*PORTB = PORTB | 0b10000000;*/

  /*if(active_layer == 2)*/
    /*PORTC = PORTC & 0b11011111;*/
  /*else*/
    /*PORTC = PORTC | 0b00100000;*/

  /*if(tick % 1000 > 100)*/
    /*PORTC = PORTC | 0b01000000;*/
  /*else*/
    /*PORTC = PORTC & 0b10111111;*/

  if(active_layer == 1)
    PORTC = PORTC & 0b11011111;
  else
    PORTC = PORTC | 0b00100000;

  if(active_layer == 2)
    PORTC = PORTC & 0b10111111;
  else
    PORTC = PORTC | 0b01000000;

/*#ifdef DEBUG*/
  debug_print();
/*#endif*/
  poll_timer_enable();
}

int main(void) {
  init();
  poll_timer_enable();
  for(ever);
}

void init(void) {
  usb_init();
  while(!usb_configured());
  keyboard_init();
  mod_keys = 0;
  for(uint8_t k = 0; k < NKEY; k++)
    key[k].bounce = key[k].pressed = 0x00;
  sei();
}

uint8_t debug_counter = 0;
void debug_print(void) {
  debug_counter++;
  if(debug_counter > 100) {
    debug_counter = 0;
    for(uint8_t i = 0; i < 7; i++)
      phex(queue[i]);
    print("\n");
    for(uint8_t k = 0; k < NKEY; k++)
      phex(key[k].bounce);
    print("\n");
  }
}
