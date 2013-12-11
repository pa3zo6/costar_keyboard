#define UT_Fn0 LLAYER(0)
#define UT_Fn1 LLAYER(1)
#define UT_Ft1 TLAYER(1, KEY_F)
#define UT_LSFTtZ TMODIFIER(LSFT, KEY_Z)
#define UT_sD {NORMAL, MODDED(D, LSFT)}
#define UT_RSFTts9 TMODIFIER(RSFT, MODDED(9, LSFT))

#define TEST_KEYMAP(k0, k1, k2, k3, k4, k5, k6, k7) { \
    k0, k1, k2, k3, k4, k5, k6, k7, \
    k0, k1, k2, k3, k4, k5, k6, k7, \
    k0, k1, k2, k3, k4, k5, k6, k7, \
    k0, k1, k2, k3, k4, k5, k6, k7, \
    k0, k1, k2, k3, k4, k5, k6, k7, \
    k0, k1, k2, k3, k4, k5, k6, k7, \
    k0, k1, k2, k3, k4, k5, k6, k7, \
    k0, k1, k2, k3, k4, k5, k6, k7, \
    k0, k1, k2, k3, k4, k5, k6, k7, \
    k0, k1, k2, k3, k4, k5, k6, k7, \
    k0, k1, k2, k3, k4, k5, k6, k7, \
    k0, k1, k2, k3, k4, k5, k6, k7, \
    k0, k1, k2, k3, k4, k5, k6, k7, \
    k0, k1, k2, k3, k4, k5, k6, k7, \
    k0, k1, k2, k3, k4, k5, k6, k7, \
    k0, k1, k2, k3, k4, k5, k6, k7, \
    k0, k1, k2, k3, k4, k5, k6, k7, \
    k0, k1, k2, k3, k4, k5, k6, k7, \
  }

#define TEST_UNIT_TEST {\
  TEST_KEYMAP(KC_A,   KC_B,UT_RSFTts9,  UT_sD, UT_LSFTtZ, UT_Ft1,   KC_0, UT_Fn1), \
  TEST_KEYMAP(KC_A,   KC_B,      KC_C,   KC_D,      KC_E,   KC_F,   KC_1, UT_Fn0), \
};
