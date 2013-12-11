#include KEYBOARD_MODEL


typedef struct KeyDef Layer[NKEY];

#ifdef UNIT_TEST

#include "test/TestLayout.h"

Layer layers[] = KEYBOARD_LAYOUT;
#define TYPE(k)  (layers[active_layer][k].type)
#define VALUE(k) (layers[active_layer][k].value)

#else

PROGMEM Layer layers[] = KEYBOARD_LAYOUT;
#define TYPE(k)  (pgm_read_word( &(layers[active_layer][k].type) ))
#define VALUE(k) (pgm_read_word( &(layers[active_layer][k].value) ))

#endif

#define KEY(k)   (VALUE(k) & 0xff)

uint8_t usb_keyboard_send(void);

struct {uint8_t pressed; uint8_t bounce;} key[NKEY];

enum DualRoleType {
  LayerShiftOrLock,
  LayerShiftOrKeyTap,
  ModifierOrKeyTap,
};

uint8_t queue[7] = {255,255,255,255,255,255,255};
uint8_t mod_keys = 0;
uint8_t active_layer = 0;

enum DualRoleType current_dualrole_type = -1;
uint8_t current_dualrole_key = 255;
bool dualrole_tap_possible = false;
bool dualrole_modifier_possible = false;

uint32_t tick = 0;
uint32_t dualrole_tap_impossible_after_tick = 0;
uint32_t dualrole_modifier_impossible_until_tick = 0;

#define LONGEST_TAP_DURATION        500
#define SHORTEST_MODIFIER_DURATION  150

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
    if(!dualrole_modifier_possible) {
      for(i = 5; i > 0; i--)
        queue[i] = queue[i-1];
      queue[0] = current_dualrole_key;

      active_layer = key[current_dualrole_key].pressed & 0x7f;
      send();

      current_dualrole_key = 255;
      dualrole_tap_possible = false;
      dualrole_modifier_possible = false;
      current_dualrole_type = -1;
      dualrole_modifier_impossible_until_tick = 0;
      dualrole_tap_impossible_after_tick = 0;
    }
  }

  if(IS_TAPPABLE_MODIFIER(k)) {
    dualrole_tap_possible = true;
    dualrole_modifier_possible = false;
    dualrole_tap_impossible_after_tick = tick + LONGEST_TAP_DURATION;
    dualrole_modifier_impossible_until_tick = tick + SHORTEST_MODIFIER_DURATION;
    current_dualrole_type = ModifierOrKeyTap;
    current_dualrole_key = k;

    mod_keys |= GET_TAPPABLE_MODIFIER(k);
    send();
  }
  else if(IS_TAPPABLE_LAYERSHIFT(k)) {
    dualrole_tap_possible = true;
    dualrole_modifier_possible = false;
    dualrole_tap_impossible_after_tick = tick + LONGEST_TAP_DURATION;
    dualrole_modifier_impossible_until_tick = tick + SHORTEST_MODIFIER_DURATION;
    current_dualrole_type = ModifierOrKeyTap;
    current_dualrole_type = LayerShiftOrKeyTap;
    current_dualrole_key = k;

    active_layer = GET_LAYER(k);
  }
  else if(IS_LAYERLOCK(k)) {
    dualrole_tap_possible = true;
    dualrole_modifier_possible = false;
    current_dualrole_type = LayerShiftOrLock;
    current_dualrole_key = k;

    /*printf("\n%d\n",active_layer);*/
    active_layer = GET_LAYER(k);
    /*printf("\n%d\n",active_layer);*/
  }
  else if(IS_MODIFIER(k)) {
    mod_keys |= KEY(k);
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

  if(current_dualrole_key == k) {
    if(current_dualrole_type == LayerShiftOrKeyTap) {

      active_layer = key[k].pressed & 0x7f;

      if(dualrole_tap_possible) {

        for(i = 5; i > 0; i--)
          queue[i] = queue[i-1];
        queue[0] = current_dualrole_key;
        send();
        for(i = 0; i < 6; i++)
          if(queue[i]==k)
            break;
        for(i = i; i < 6; i++)
          queue[i] = queue[i+1];
        send();

      } else {

      }

    } else if(current_dualrole_type == LayerShiftOrLock) {
      if(dualrole_tap_possible) {

      } else {

        active_layer = key[k].pressed & 0x7f;

      }
    } else if(current_dualrole_type == ModifierOrKeyTap) {
      if(dualrole_tap_possible) {

        mod_keys &= ~GET_TAPPABLE_MODIFIER(k);

        for(i = 5; i > 0; i--)
          queue[i] = queue[i-1];
        queue[0] = current_dualrole_key;
        send();

        for(i = 0; i < 6; i++)
          if(queue[i]==k)
            break;
        for(i = i; i < 6; i++)
          queue[i] = queue[i+1];
        send();


      } else {

        mod_keys &= ~GET_TAPPABLE_MODIFIER(k);
        send();

      }

    }

    current_dualrole_key = 255;
    dualrole_tap_possible = false;
  }
  else
  if(IS_MODIFIER((k))) {
    mod_keys &= ~KEY(k);
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

  key[k].pressed = 0x00;
}

void send(void) {
  uint8_t i;
  uint8_t addtnl_mod_keys = 0;
  for(i = 0; i < 6; i++) {
    if(queue[i] < 255) {

      uint8_t k = queue[i];

      if(IS_MODDED(k)) {
        /*uint8_t mods = GET_ADDITIONAL_MODIFIERS(k);*/
        uint16_t v = VALUE(k) & 0xff00;
        uint8_t mods = v >> 8;
        addtnl_mod_keys |= mods;

        uint8_t kk[] = {
          KEY_0, KEY_1, KEY_2, KEY_3, KEY_4, KEY_5, KEY_6, KEY_7};
        /*keyboard_keys[i] = kk[mods & 0b00000111];*/
        /*keyboard_keys[i] = kk[(mods & 0b00111000)>>3];*/
        /*keyboard_keys[i] = kk[(mods & 0b11100000)>>5];*/
      }

      keyboard_keys[i] = KEY(k);

    } else {
      keyboard_keys[i] = 0;
    }
  }
  keyboard_modifier_keys = mod_keys | addtnl_mod_keys;
  usb_keyboard_send();
}
