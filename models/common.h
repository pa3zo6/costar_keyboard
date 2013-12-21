/* Common stuff. */

#ifndef common_h__
#define common_h__

#ifndef UNIT_TEST
#include <avr/pgmspace.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#endif

#include "../lib/keycode.h"
#include "../lib/avr_extra.h"

#define STR_MANUFACTURER L"Bathroom Epiphanies, Costar Keyboard -"
#define VENDOR_ID        0x16C0

#define SCAN_INTERRUPT_FUNCTION TIMER0_COMPA_vect
#define SETTLE_TIME_US 1

#define NROW  18
#define NCOL   8
#define NKEY 144

/* Convert physical keyboard layout to matrix array. This is a macro
   to define keymap easily in keyboard layout form. All ANSI ISO JIS
   Layouts are on the same PCB */
#define KEYMAP( \
    KG6,      KH4, KI4, KI2, KI6, KP5, KL6, KM2, KM4, KO4, KO5, KO6, KO0,   KN5, KN7, KP7,                       \
    KG4, KG5, KH5, KI5, KJ5, KJ4, KK4, KK5, KL5, KM5, KF5, KF4, KL4, KO2,   KR4, KC4, KE4,   KQ1, KR1, KE1, KE0, \
    KG2, KG7, KH7, KI7, KJ7, KJ2, KK2, KK7, KL7, KM7, KF7, KF2, KL2, KO3,   KQ4, KC5, KE5,   KQ7, KR7, KE7, KC7, \
    KH2, KG3, KH3, KI3, KJ3, KJ6, KK6, KK3, KL3, KM3, KF3, KF6,      KO1,                    KQ2, KR2, KE2,      \
    KB2, KH6, KG1, KH1, KI1, KJ1, KJ0, KK0, KK1, KL1, KM1, KF0,      KB3,        KC6,        KQ3, KR3, KE3, KC3, \
    KP4, KD2, KN6,                KQ6,                KN0, KA3, KM0, KP1,   KC0, KQ0, KR0,   KR6,      KE6     ) \
{                                                                                       \
/*      0         1         2         3         4         5         6         7   */    \
/* A */ KC_NO,    KC_NO,    KC_NO,    KC_##KA3, KC_NO,    KC_NO,    KC_NO,    KC_NO,    \
/* B */ KC_NO,    KC_NO,    KC_##KB2, KC_##KB3, KC_NO,    KC_NO,    KC_NO,    KC_NO,    \
/* C */ KC_##KC0, KC_NO,    KC_NO,    KC_##KC3, KC_##KC4, KC_##KC5, KC_##KC6, KC_##KC7, \
/* D */ KC_NO,    KC_NO,    KC_##KD2, KC_NO,    KC_NO,    KC_NO,    KC_NO,    KC_NO,    \
/* E */ KC_##KE0, KC_##KE1, KC_##KE2, KC_##KE3, KC_##KE4, KC_##KE5, KC_##KE6, KC_##KE7, \
/* F */ KC_##KF0, KC_NO,    KC_##KF2, KC_##KF3, KC_##KF4, KC_##KF5, KC_##KF6, KC_##KF7, \
/* G */ KC_NO,    KC_##KG1, KC_##KG2, KC_##KG3, KC_##KG4, KC_##KG5, KC_##KG6, KC_##KG7, \
/* H */ KC_NO,    KC_##KH1, KC_##KH2, KC_##KH3, KC_##KH4, KC_##KH5, KC_##KH6, KC_##KH7, \
/* I */ KC_NO,    KC_##KI1, KC_##KI2, KC_##KI3, KC_##KI4, KC_##KI5, KC_##KI6, KC_##KI7, \
/* J */ KC_##KJ0, KC_##KJ1, KC_##KJ2, KC_##KJ3, KC_##KJ4, KC_##KJ5, KC_##KJ6, KC_##KJ7, \
/* K */ KC_##KK0, KC_##KK1, KC_##KK2, KC_##KK3, KC_##KK4, KC_##KK5, KC_##KK6, KC_##KK7, \
/* L */ KC_NO,    KC_##KL1, KC_##KL2, KC_##KL3, KC_##KL4, KC_##KL5, KC_##KL6, KC_##KL7, \
/* M */ KC_##KM0, KC_##KM1, KC_##KM2, KC_##KM3, KC_##KM4, KC_##KM5, KC_NO,    KC_##KM7, \
/* N */ KC_##KN0, KC_NO,    KC_NO,    KC_NO,    KC_NO,    KC_##KN5, KC_##KN6, KC_##KN7, \
/* O */ KC_##KO0, KC_##KO1, KC_##KO2, KC_##KO3, KC_##KO4, KC_##KO5, KC_##KO6, KC_NO,    \
/* P */ KC_NO,    KC_##KP1, KC_NO,    KC_NO,    KC_##KP4, KC_##KP5, KC_NO,    KC_##KP7, \
/* Q */ KC_##KQ0, KC_##KQ1, KC_##KQ2, KC_##KQ3, KC_##KQ4, KC_NO,    KC_##KQ6, KC_##KQ7, \
/* R */ KC_##KR0, KC_##KR1, KC_##KR2, KC_##KR3, KC_##KR4, KC_NO,    KC_##KR6, KC_##KR7, \
}

/* ,---.   ,---------------. ,---------------. ,---------------. ,-----------. ,---------------.
   |Esc|   |F1 |F2 |F3 |F4 | |F5 |F6 |F7 |F8 | |F9 |F10|F11|F12| |PrS|ScL|Pau| |PrS|ScL|ScL|Pau|
   `---'   `---------------' `---------------' `---------------' `-----------' `---------------'
   ,-----------------------------------------------------------. ,-----------. ,---------------.
   | ~ | 1 | 2 | 3 | 4 | 5 | 6 | 7 | 8 | 9 | 0 | - | = | Backsp| |Ins|Hom|PgU| |NuL| / | * | - |
   |-----------------------------------------------------------| |-----------| |---------------|
   | Tab | Q | W | E | R | T | Y | U | I | O | P | [ | ] |  \  | |Del|End|PgD| | 7 | 8 | 9 | + |
   |-----------------------------------------------------------| `-----------' |-----------|   |
   |CapsL | A | S | D | F | G | H | J | K | L | ; | ' | Return |               | 5 | 6 | 6 |   |
   |-----------------------------------------------------------|     ,---.     |---------------|
   |Shft|ISO| Z | X | C | V | B | N | M | , | . | / |    Shift |     |Up |     | 1 | 2 | 3 | E |
   |-----------------------------------------------------------| ,-----------. |-----------| n |
   |Ctl|Gui|Alt|          Space                |Alt|Gui|App|Ctl| |Lef|Dow|Rig| | 0     | . | t |
   `-----------------------------------------------------------' `-----------' `---------------'  */

// ==== ANSI ISO JIS ===================================================================================================
#define ANSI_ISO_JIS {\
  KEYMAP(ESC,        F1,  F2,  F3,  F4,  F5,  F6,  F7,  F8,  F9, F10, F11, F12,  PSCR,SLCK,PAUS,                       \
         GRV,    1,   2,   3,   4,   5,   6,   7,   8,   9,   0,MINS, EQL,BSPC,   INS,HOME,PGUP,  NLCK,PSLS,PAST,PMNS, \
         TAB,    Q,   W,   E,   R,   T,   Y,   U,   I,   O,   P,LBRC,RBRC,BSLS,   DEL, END,PGDN,    P7,  P8,  P9,PPLS, \
         CAPS,   A,   S,   D,   F,   G,   H,   J,   K,   L,SCLN,QUOT,      ENT,                     P4,  P5,  P6,      \
         LSFT,NUBS,   Z,   X,   C,   V,   B,   N,   M,COMM, DOT,SLSH,     RSFT,          UP,        P1,  P2,  P3,PENT, \
         LCTL,LGUI,LALT,                SPC,               RALT,RGUI, APP,RCTL,   LEFT,DOWN,RGHT,   P0,     PDOT       )}


// ==== DVORAK =========================================================================================================
#define DVORAK {\
  KEYMAP(ESC,        F1,  F2,  F3,  F4,  F5,  F6,  F7,  F8,  F9, F10, F11, F12,  PSCR,SLCK,PAUS,                       \
         GRV,    1,   2,   3,   4,   5,   6,   7,   8,   9,   0,LBRC,RBRC,BSPC,   INS,HOME,PGUP,  NLCK,PSLS,PAST,PMNS, \
         TAB, QUOT,COMM, DOT,   P,   Y,   F,   G,   C,   R,   L,SLSH, EQL,BSLS,   DEL, END,PGDN,    P7,  P8,  P9,PPLS, \
         CAPS,   A,   O,   E,   U,   I,   D,   H,   T,   N,   S,MINS,      ENT,                     P4,  P5,  P6,      \
         LSFT,NUBS,SCLN,   Q,   J,   K,   X,   B,   M,   W,   V,   Z,     RSFT,         UP,         P1,  P2,  P3,PENT, \
         LCTL,LGUI,LALT,                SPC,               RALT,RGUI, APP,RCTL,  LEFT,DOWN,RGHT)    P0,     PDOT       )}


// ==== Space FN =======================================================================================================
#define KC_SPCFN1   TLAYERON(1, KEY_SPACE)                                      /*Tap to space, hold switch to Layer 1*/
#define KC_APP1     TLAYERON(1, KEY_APPLICATION)                          /*Tap to Application, hold switch to Layer 1*/
// ---------------------------------------------------------------------------------------------------------------------
#define SPACE_FN {\
  /*--- Layer 0 ----------------------------------------------------------------------------------------------------*/ \
  KEYMAP(ESC,        F1,  F2,  F3,  F4,  F5,  F6,  F7,  F8,  F9, F10, F11, F12,  PSCR,SLCK,PAUS,                       \
         ESC,    1,   2,   3,   4,   5,   6,   7,   8,   9,   0,MINS, EQL,BSPC,   INS,HOME,PGUP,  NLCK,PSLS,PAST,PMNS, \
         TAB,    Q,   W,   E,   R,   T,   Y,   U,   I,   O,   P,LBRC,RBRC,BSLS,   DEL, END,PGDN,    P7,  P8,  P9,PPLS, \
         CAPS,   A,   S,   D,   F,   G,   H,   J,   K,   L,SCLN,QUOT,      ENT,                     P4,  P5,  P6,      \
         LSFT,NUBS,   Z,   X,   C,   V,   B,   N,   M,COMM, DOT,SLSH,     RSFT,         UP,         P1,  P2,  P3,PENT, \
         LCTL,LGUI,LALT,             SPCFN1,               RALT,RGUI,APP1,RCTL,  LEFT,DOWN,RGHT,    P0,     PDOT     ),\
  /*--- Layer 1 ----------------------------------------------------------------------------------------------------*/ \
  KEYMAP(ESC,        F1,  F2,  F3,  F4,  F5,  F6,  F7,  F8,  F9, F10, F11, F12,  PSCR,SLCK,PAUS,                       \
         ESC,   F1,  F2,  F3,  F4,  F5,  F6,  F7,  F8,  F9, F10, F11, F12, DEL,   INS,HOME,PGUP,  NLCK,PSLS,PAST,PMNS, \
         TAB,    Q,   W, ESC,   R,   T,   Y,HOME,  UP, END,PSCR,SLCK,PAUS, INS,   DEL, END,PGDN,    P7,  P8,  P9,PPLS, \
         CAPS,   A,   S,   D,   F,   G,PGUP,LEFT,DOWN,RGHT,SCLN,QUOT,      ENT,                     P4,  P5,  P6,      \
         LSFT,NUBS,   Z,   X,   C, SPC,PGDN, GRV,sGRV,COMM, DOT, APP,     RSFT,          UP,        P1,  P2,  P3,PENT, \
         LCTL,LGUI,LALT,                SPC,               RALT,RGUI,APP1,RCTL,   LEFT,DOWN,RGHT,   P0,     PDOT     ),\
  }

// ==== Space FN 4 mooswa ==============================================================================================
#define KC_LSB        TMODIFIER(LSFT, KEY_LBRACKET)
#define KC_sLSB       TMODIFIER(LSFT, MODDED(LBRACKET,RSFT))
#define KC_RSB        TMODIFIER(RSFT, KEY_RBRACKET)
#define KC_sRSB       TMODIFIER(RSFT, MODDED(RBRACKET,RSFT))
#define KC_CTRLESC    TMODIFIER(LCTL, KEY_ESCAPE)
#define KC_CTRLENT    TMODIFIER(RCTL, KEY_ENTER)
#define KC_FN2s9      TLAYERON(2, MODDED(9, LSFT))
#define KC_FN2s0      TLAYERON(2, MODDED(0, LSFT))
#define KC_RSs0       TMODIFIER(RSFT, MODDED(0, LSFT))

#define KC_CTLMNS     TMODIFIER(LCTL, KEY_MINUS)
#define KC_sCTLUN     TMODIFIER(LSFT|LCTL, MODDED(MINUS, LSFT))

#define KC_sALTMN     TMODIFIER(LSFT|LCTL, KEY_MINUS)
#define KC_ALTUND     TMODIFIER(LALT, MODDED(MINUS, LSFT))

#define KC_CTLPLS     TMODIFIER(RCTL, MODDED(EQUAL, LSFT))
#define KC_sCTLEQ     TMODIFIER(LSFT|RCTL, KEY_EQUAL)

#define KC_sALTPL     TMODIFIER(LSFT|RCTL, MODDED(EQUAL, LSFT))
#define KC_ALTEQL     TMODIFIER(RALT, KEY_EQUAL)

#define KC_sSPCFN1    TLAYERON(1, MODDED(SPACE,LSFT))                      /*Tap to shift+space, hold switch to Layer 1*/
// ---------------------------------------------------------------------------------------------------------------------
#define SPACE_FN_MOOSWA {\
  /*--- Layer 00 -----------------------------------------------------------------------------------------------------------*/ \
  KEYMAP(    ESC,             _,     _,     _,     _,     _,     _,     _,     _,     _,     _,     _,     _, _,_,_,           \
             GRV,      1,     2,     3,     4,     5,     6,     7,     8,     9,     0,     _,     _,  BSPC, _,_,_, _,_,_,_,  \
             TAB,      Q,     W,     E,     R,     T,     Y,     U,     I,     O,     P,  BSPC,  BSPC,  BSLS, _,_,_, _,_,_,_,  \
         CTRLESC,      A,     S,     D,     F,     G,     H,     J,     K,     L,  SCLN,  QUOT,      CTRLENT,        _,_,_,    \
           FN2s9,   NUBS,     Z,     X,     C,     V,     B,     N,     M,  COMM,   DOT,  SLSH,        FN2s0,    _,  _,_,_,_,  \
          CTLMNS,   LGUI,ALTUND,                     SPCFN1,              ALTEQL,  RGUI,CTLPLS, CTLPLS,        _,_,_, _, _   ),\
  /*--- Layer 01 -----------------------------------------------------------------------------------------------------------*/ \
  KEYMAP(      _,             _,     _,     _,     _,     _,     _,     _,     _,     _,     _,     _,     _, _,_,_,           \
             GRV,     F1,    F2,    F3,    F4,    F5,    F6,    F7,    F8,    F9,   F10,   F11,   F12,   DEL, _,_,_, _,_,_,_,  \
             TAB,      Q,    P7,    P8,    P9,     T,  HOME,  PGDN,  PGUP,   END,  PSCR,   DEL,   DEL,  BSLS, _,_,_, _,_,_,_,  \
         CTRLESC,   PDOT,    P4,    P5,    P6,    P0,  LEFT,  DOWN,    UP,  RGHT,  SCLN,  QUOT,      CTRLENT,        _,_,_,    \
             LSB,   NUBS,     Z,    P1,    P2,    P3,   SPC,   INS,   APP,  COMM,   DOT,  BSLS,          RSB,    _,  _,_,_,_,  \
            LCTL,   LGUI,  LALT,                          _,                RALT,  RGUI,  RCTL,   RCTL,        _,_,_, _, _   ),\
  /*--- Layer 10 -----------------------------------------------------------------------------------------------------------*/ \
  KEYMAP(      _,             _,     _,     _,     _,     _,     _,     _,     _,     _,     _,     _,     _, _,_,_,           \
            sGRV,     s1,    s2,    s3,    s4,    s5,    s6,    s7,    s8,    s9,    s0,     _,     _, sBSPC, _,_,_, _,_,_,_,  \
            sTAB,     sQ,    sW,    sE,    sR,    sT,    sY,    sU,    sI,    sO,    sP, sBSPC, sBSPC, sBSLS, _,_,_, _,_,_,_,  \
         CTRLESC,     sA,    sS,    sD,    sF,    sG,    sH,    sJ,    sK,    sL, sSCLN, sQUOT,      CTRLENT,        _,_,_,    \
           sLBRC,  sNUBS,    sZ,    sX,    sC,    sV,    sB,    sN,    sM, sCOMM,  sDOT, sSLSH,        sRBRC,   _,   _,_,_,_,  \
          sCTLUN,  sLGUI,sALTMN,                    sSPCFN1,              sALTPL, sRGUI,sCTLEQ,sCTLEQ,        _,_,_, _,  _   ),\
  /*--- Layer 11 -----------------------------------------------------------------------------------------------------------*/ \
  KEYMAP(      _,             _,     _,     _,     _,     _,     _,     _,     _,     _,     _,     _,     _, _,_,_,           \
            sGRV,    sF1,   sF2,   sF3,   sF4,   sF5,   sF6,   sF7,   sF8,   sF9,  sF10,  sF11,  sF12,  sDEL, _,_,_, _,_,_,_,  \
            sTAB,     sQ,    sW,    sE,    sR,    sT, sHOME, sPGDN, sPGUP,  sEND, sPSCR,  sDEL,  sDEL, sBSLS, _,_,_, _,_,_,_,  \
         CTRLESC,     sA,    sS,    sD,    sF,    sG, sLEFT, sDOWN,   sUP, sRGHT, sSCLN, sQUOT,      CTRLENT,        _,_,_,    \
            sLSB,  sNUBS,    sZ,    sX,    sC,    sV,  sSPC,  sINS,    sM, sCOMM,  sDOT, sBSLS,         sRSB,   _,   _,_,_,_,  \
           sLCTL,  sLGUI, sLALT,                          _,               sRALT, sRGUI, sRCTL, sRCTL,        _,_,_, _,  _   ),\
  }





/* Test layouts to easily find the position of keys in the underlying
   matrix. */
#define TEST_COLS \
/*  COL 0   COL 1   COL 2   COL 3   COL 4   COL 5   COL 6   COL 7 */ \
  {{ \
    KC_0,   KC_1,   KC_2,   KC_3,   KC_4,   KC_5,   KC_6,   KC_7,   /* ROW A */ \
    KC_0,   KC_1,   KC_2,   KC_3,   KC_4,   KC_5,   KC_6,   KC_7,   /* ROW B */ \
    KC_0,   KC_1,   KC_2,   KC_3,   KC_4,   KC_5,   KC_6,   KC_7,   /* ROW C */ \
    KC_0,   KC_1,   KC_2,   KC_3,   KC_4,   KC_5,   KC_6,   KC_7,   /* ROW D */ \
    KC_0,   KC_1,   KC_2,   KC_3,   KC_4,   KC_5,   KC_6,   KC_7,   /* ROW E */ \
    KC_0,   KC_1,   KC_2,   KC_3,   KC_4,   KC_5,   KC_6,   KC_7,   /* ROW F */ \
    KC_0,   KC_1,   KC_2,   KC_3,   KC_4,   KC_5,   KC_6,   KC_7,   /* ROW G */ \
    KC_0,   KC_1,   KC_2,   KC_3,   KC_4,   KC_5,   KC_6,   KC_7,   /* ROW H */ \
    KC_0,   KC_1,   KC_2,   KC_3,   KC_4,   KC_5,   KC_6,   KC_7,   /* ROW I */ \
    KC_0,   KC_1,   KC_2,   KC_3,   KC_4,   KC_5,   KC_6,   KC_7,   /* ROW J */ \
    KC_0,   KC_1,   KC_2,   KC_3,   KC_4,   KC_5,   KC_6,   KC_7,   /* ROW K */ \
    KC_0,   KC_1,   KC_2,   KC_3,   KC_4,   KC_5,   KC_6,   KC_7,   /* ROW L */ \
    KC_0,   KC_1,   KC_2,   KC_3,   KC_4,   KC_5,   KC_6,   KC_7,   /* ROW M */ \
    KC_0,   KC_1,   KC_2,   KC_3,   KC_4,   KC_5,   KC_6,   KC_7,   /* ROW N */ \
    KC_0,   KC_1,   KC_2,   KC_3,   KC_4,   KC_5,   KC_6,   KC_7,   /* ROW O */ \
    KC_0,   KC_1,   KC_2,   KC_3,   KC_4,   KC_5,   KC_6,   KC_7,   /* ROW P */ \
    KC_0,   KC_1,   KC_2,   KC_3,   KC_4,   KC_5,   KC_6,   KC_7,   /* ROW Q */ \
    KC_0,   KC_1,   KC_2,   KC_3,   KC_4,   KC_5,   KC_6,   KC_7    /* ROW R */ \
  }};
#define TEST_ROWS \
/*  COL 0   COL 1   COL 2   COL 3   COL 4   COL 5   COL 6   COL 7 */ \
  {{ \
    KC_A,   KC_A,   KC_A,   KC_A,   KC_A,   KC_A,   KC_A,   KC_A,   /* ROW A */ \
    KC_B,   KC_B,   KC_B,   KC_B,   KC_B,   KC_B,   KC_B,   KC_B,   /* ROW B */ \
    KC_C,   KC_C,   KC_C,   KC_C,   KC_C,   KC_C,   KC_C,   KC_C,   /* ROW C */ \
    KC_D,   KC_D,   KC_D,   KC_D,   KC_D,   KC_D,   KC_D,   KC_D,   /* ROW D */ \
    KC_E,   KC_E,   KC_E,   KC_E,   KC_E,   KC_E,   KC_E,   KC_E,   /* ROW E */ \
    KC_F,   KC_F,   KC_F,   KC_F,   KC_F,   KC_F,   KC_F,   KC_F,   /* ROW F */ \
    KC_G,   KC_G,   KC_G,   KC_G,   KC_G,   KC_G,   KC_G,   KC_G,   /* ROW G */ \
    KC_H,   KC_H,   KC_H,   KC_H,   KC_H,   KC_H,   KC_H,   KC_H,   /* ROW H */ \
    KC_I,   KC_I,   KC_I,   KC_I,   KC_I,   KC_I,   KC_I,   KC_I,   /* ROW I */ \
    KC_J,   KC_J,   KC_J,   KC_J,   KC_J,   KC_J,   KC_J,   KC_J,   /* ROW J */ \
    KC_K,   KC_K,   KC_K,   KC_K,   KC_K,   KC_K,   KC_K,   KC_K,   /* ROW K */ \
    KC_L,   KC_L,   KC_L,   KC_L,   KC_L,   KC_L,   KC_L,   KC_L,   /* ROW L */ \
    KC_M,   KC_M,   KC_M,   KC_M,   KC_M,   KC_M,   KC_M,   KC_M,   /* ROW M */ \
    KC_N,   KC_N,   KC_N,   KC_N,   KC_N,   KC_N,   KC_N,   KC_N,   /* ROW N */ \
    KC_O,   KC_O,   KC_O,   KC_O,   KC_O,   KC_O,   KC_O,   KC_O,   /* ROW O */ \
    KC_P,   KC_P,   KC_P,   KC_P,   KC_P,   KC_P,   KC_P,   KC_P,   /* ROW P */ \
    KC_Q,   KC_Q,   KC_Q,   KC_Q,   KC_Q,   KC_Q,   KC_Q,   KC_Q,   /* ROW Q */ \
    KC_R,   KC_R,   KC_R,   KC_R,   KC_R,   KC_R,   KC_R,   KC_R    /* ROW R */ \
  }};

#endif
