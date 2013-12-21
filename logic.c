#include KEYBOARD_MODEL


typedef struct KeyDef Layer[NKEY];

#ifdef UNIT_TEST

#include "test/TestLayout.h"

Layer layers[] = KEYBOARD_LAYOUT;
#define TYPE(k)  (layers[active_layer][k].type)
#define VALUE(k) (layers[active_layer][k].value)
#define WAS_TYPE(k) (layers[WAS_PRESSED_ON_LAYER(k)][k].type)
#define WAS_VALUE(k) (layers[WAS_PRESSED_ON_LAYER(k)][k].value)

#else

PROGMEM Layer layers[] = KEYBOARD_LAYOUT;
#define TYPE(k)  (pgm_read_word( &(layers[active_layer][k].type) ))
#define VALUE(k) (pgm_read_word( &(layers[active_layer][k].value) ))
#define WAS_TYPE(k)  (pgm_read_word( &(layers[WAS_PRESSED_ON_LAYER(k)][k].type) ))
#define WAS_VALUE(k) (pgm_read_word( &(layers[WAS_PRESSED_ON_LAYER(k)][k].value) ))
#endif

#define KEY(k)   (VALUE(k) & 0xff)
#define WAS_KEY(k)   (WAS_VALUE(k) & 0xff)
#define WAS_PRESSED_ON_LAYER(k) (key[k].pressed & 0x7f)

#define LAYERBIT_OFF(bitNum)  (active_layer & ~(1 << (bitNum-1)))
#define LAYERBIT_ON(bitNum)   (active_layer |  (1 << (bitNum-1)))

#define KEY_SETS_LAYERBIT_ON(k) (WAS_TYPE(k) & 0x80)
#define ENGAGE_LAYER(k)         (KEY_SETS_LAYERBIT_ON(k) ? LAYERBIT_ON(GET_WAS_LAYER(k)) : LAYERBIT_OFF(GET_WAS_LAYER(k)))
#define DISENGAGE_LAYER(k)      (KEY_SETS_LAYERBIT_ON(k) ? LAYERBIT_OFF(GET_WAS_LAYER(k)) : LAYERBIT_ON(GET_WAS_LAYER(k)))

uint8_t usb_keyboard_send(void);

struct {uint8_t pressed; uint8_t bounce;} key[NKEY];

uint8_t queue[7] = {255,255,255,255,255,255,255};
uint8_t mod_keys = 0;
uint8_t active_layer = 0;

uint8_t current_dualrole_key = 255;
bool dualrole_tap_possible = false;
bool dualrole_modifier_possible = false;

uint32_t tick = 0;
uint32_t dualrole_tap_impossible_after_tick = 0;
uint32_t dualrole_modifier_impossible_until_tick = 0;

#define LONGEST_TAP_DURATION        500
#define SHORTEST_MODIFIER_DURATION  50

extern uint8_t keyboard_modifier_keys;
extern uint8_t keyboard_keys[6];

void send(void);

void debug_print(void);

void make_dualrole_tap_impossible() {
  dualrole_tap_possible = false;
  dualrole_tap_impossible_after_tick = 0;
}

void make_dualrole_modifier_possible() {
  dualrole_modifier_possible = true;
  dualrole_modifier_impossible_until_tick = 0;
}

void key_press(uint8_t k) {
  uint8_t i;

  key[k].pressed = 0x80 | active_layer;
  if(current_dualrole_key != 255) {
    if(!dualrole_modifier_possible && IS_NORMAL(k) && WAS_TAPPABLE_LAYERSHIFT(current_dualrole_key) ) {//TODO do we need dualrole_modifier_possible ?
      for(i = 5; i > 0; i--)
        queue[i] = queue[i-1];
      queue[0] = current_dualrole_key;

      active_layer = DISENGAGE_LAYER(current_dualrole_key);

      send();

      for(i = 0; i < 6; i++)
        if(queue[i]==current_dualrole_key)
          break;
      for(i = i; i < 6; i++)
        queue[i] = queue[i+1];
      send();

      current_dualrole_key = 255;
      dualrole_tap_possible = false;
      dualrole_modifier_possible = false;
      dualrole_modifier_impossible_until_tick = 0;
      dualrole_tap_impossible_after_tick = 0;
    }
  }

  if(IS_TAPPABLE_MODIFIER(k)) {
    dualrole_tap_possible = true;
    dualrole_modifier_possible = false;
    dualrole_tap_impossible_after_tick = tick + LONGEST_TAP_DURATION;
    dualrole_modifier_impossible_until_tick = tick + SHORTEST_MODIFIER_DURATION;
    current_dualrole_key = k;

    mod_keys |= GET_TAPPABLE_MODIFIER(k);
    send();
  }
  else if(IS_TAPPABLE_LAYERSHIFT(k)) {
    dualrole_tap_possible = true;
    dualrole_modifier_possible = false;
    dualrole_tap_impossible_after_tick = tick + LONGEST_TAP_DURATION;
    dualrole_modifier_impossible_until_tick = tick + SHORTEST_MODIFIER_DURATION;
    current_dualrole_key = k;

    active_layer = ENGAGE_LAYER(k);
  }
  else if(IS_LAYERLOCK(k)) {
    dualrole_tap_possible = true;
    dualrole_modifier_possible = false;
    current_dualrole_key = k;

    active_layer = ENGAGE_LAYER(k);
  }
  else if(IS_MODIFIER(k)) {
    mod_keys |= KEY(k);

    if(IS_MODDED(k)) {
      mod_keys |= ~GET_ADDITIONAL_MODIFIERS(k);
    }

    dualrole_tap_possible = false;
    send();
  }
  else {
    dualrole_tap_possible = false;
    for(i = 5; i > 0; i--)
      queue[i] = queue[i-1];
    queue[0] = k;
    send();
  }
}

void key_release(uint8_t k) {
  uint8_t i;

  if(WAS_TAPPABLE_LAYERSHIFT(k)) {

    active_layer = DISENGAGE_LAYER(k);

    if(dualrole_tap_possible) {

      for(i = 5; i > 0; i--)
        queue[i] = queue[i-1];
      queue[0] = k;
      send();
      for(i = 0; i < 6; i++)
        if(queue[i]==k)
          break;
      for(i = i; i < 6; i++)
        queue[i] = queue[i+1];
      send();

    } else {

    }
    current_dualrole_key = 255;
  }
  else if(WAS_LAYERLOCK(k)) {
    if(dualrole_tap_possible) {

    } else {

      active_layer = DISENGAGE_LAYER(k);

    }
    current_dualrole_key = 255;
  }
  else if(WAS_TAPPABLE_MODIFIER(k)) {
    if(dualrole_tap_possible) {

      mod_keys &= ~GET_WAS_TAPPABLE_MODIFIER(k);
      send();

      for(i = 5; i > 0; i--)
        queue[i] = queue[i-1];
      queue[0] = k;
      send();

      for(i = 0; i < 6; i++)
        if(queue[i]==k)
          break;
      for(i = i; i < 6; i++)
        queue[i] = queue[i+1];
      send();

    } else {

      mod_keys &= ~GET_WAS_TAPPABLE_MODIFIER(k);
      send();

    }
    current_dualrole_key = 255;
  }
  else if(WAS_MODIFIER((k))) {
    mod_keys &= ~KEY(k);

      if(WAS_MODDED(k)) {
        mod_keys &= ~GET_WAS_ADDITIONAL_MODIFIERS(k);
      }

    send();
  }
  else {
    for(i = 0; i < 6; i++)
      if(queue[i]==k)
        break;
    for(i = i; i < 6; i++)
      queue[i] = queue[i+1];
    send();
  }

  dualrole_tap_possible = false;

  key[k].pressed = 0x00;
}

void send(void) {
  uint8_t i;
  uint8_t addtnl_mod_keys = 0;

  for(i = 0; i < 6; i++) {
    if(queue[i] < 255) {
      uint8_t k = queue[i];

      if(WAS_MODDED(k)) {
        addtnl_mod_keys |= GET_WAS_ADDITIONAL_MODIFIERS(k);
      }
    }
  }

  //sending the new modifiers first, seems to help some OSes
  if((addtnl_mod_keys & ~mod_keys) != 0) {
    keyboard_modifier_keys = mod_keys | addtnl_mod_keys;
    usb_keyboard_send();
  }

  for(i = 0; i < 6; i++) {
    if(queue[i] < 255) {
      uint8_t k = queue[i];
      keyboard_keys[i] = KEY(k);
    } else {
      keyboard_keys[i] = 0;
    }
  }
  keyboard_modifier_keys = mod_keys | addtnl_mod_keys;
  usb_keyboard_send();
}
