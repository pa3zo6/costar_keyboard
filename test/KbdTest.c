#include <assert.h>
#include <setjmp.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "CuTest.h"
#include "../models/common.h"

uint8_t keyboard_modifier_keys=0;
uint8_t keyboard_keys[6]={0,0,0,0,0,0};

extern uint8_t active_layer;

#define MAX_LOG 10

int log_keyboard_keys[MAX_LOG][6]={
  {0,0,0,0,0,0},
  {0,0,0,0,0,0},
  {0,0,0,0,0,0},
  {0,0,0,0,0,0},
  {0,0,0,0,0,0},
  {0,0,0,0,0,0},
  {0,0,0,0,0,0},
  {0,0,0,0,0,0},
  {0,0,0,0,0,0},
  {0,0,0,0,0,0},
};

uint8_t log_mods[10] = {0,0,0,0,0,0,0,0,0,0};

int send_count = 0;
int stash_send_count = 0;

uint8_t usb_keyboard_send(void) {
  uint8_t i;
  for(i=0; i<6; i++) {
    log_keyboard_keys[send_count][i] = keyboard_keys[i];
    /*printf("%d ", keyboard_keys[i]);*/
  }
  /*printf("\n");*/

  log_mods[send_count] = keyboard_modifier_keys;
  send_count ++;
  return 0;
}

void key_press(uint8_t k);

#define START_KEY_CAPTURE send_count = 0;
#define EXPECT_SENT(cnt) CuAssertIntEquals(tc, cnt, send_count);

#define SHOULDNOT_SEND stash_send_count = send_count;
#define VERIFY_DIDNOT_SEND CuAssertIntEquals(tc, stash_send_count, send_count);

#define VERIFY send_count = 0;
#define KEY(key) CuAssertIntEquals(tc, KEY_##key, log_keyboard_keys[send_count][0]);
#define KEYS2(key1,key2) KEY(key1); CuAssertIntEquals(tc, KEY_##key2, log_keyboard_keys[send_count][1]);

#define THEN send_count++;

#define NOKEYS  CuAssertIntEquals(tc, 0, log_keyboard_keys[send_count][0]);
#define WITH(mods) CuAssertIntEquals(tc, mods, log_mods[send_count]);
#define NOMODS CuAssertIntEquals(tc, 0, log_mods[send_count]);

void TestKey(CuTest* tc)
{
  START_KEY_CAPTURE;

  key_press(0);//A
  key_release(0);

  key_press(0);//A
  key_release(0);

  EXPECT_SENT(4);
  VERIFY KEY(A);
  THEN   NOKEYS;
  THEN   KEY(A);
  THEN   NOKEYS;
}

void TestKeyModded(CuTest* tc)
{
  START_KEY_CAPTURE;

  key_press(3);//sD
  key_release(3);

  EXPECT_SENT(3);
  VERIFY NOKEYS WITH(LSFT);
  THEN KEY(D) WITH(LSFT);
  THEN NOKEYS;
}


void TestLayerLock(CuTest* tc)
{
  START_KEY_CAPTURE;

  SHOULDNOT_SEND;
  key_press(7);//toggle layer to 1
  CuAssertIntEquals(tc, 1, active_layer);
  key_release(7);
  CuAssertIntEquals(tc, 1, active_layer);
  VERIFY_DIDNOT_SEND;

  key_press(6);//0 or 1 depending on layer
  key_release(6);

  SHOULDNOT_SEND;
  key_press(7);//toggle layer to 0
  CuAssertIntEquals(tc, 0, active_layer);
  key_release(7);
  CuAssertIntEquals(tc, 0, active_layer);
  VERIFY_DIDNOT_SEND;

  key_press(6);//0 or 1 depending on layer
  key_release(6);

  VERIFY KEY(1);
  THEN   NOKEYS;

  THEN KEY(0);
  THEN   NOKEYS;
}

void TestModModded(CuTest* tc)
{
  START_KEY_CAPTURE;

  SHOULDNOT_SEND;
  key_press(7);//toggle layer to 1
  CuAssertIntEquals(tc, 1, active_layer);
  key_release(7);
  CuAssertIntEquals(tc, 1, active_layer);
  VERIFY_DIDNOT_SEND;

  key_press(2);//sLALT
  key_release(2);//sLALT

  SHOULDNOT_SEND;
  key_press(7);//toggle layer to 0
  CuAssertIntEquals(tc, 0, active_layer);
  key_release(7);
  CuAssertIntEquals(tc, 0, active_layer);
  VERIFY_DIDNOT_SEND;

  VERIFY NOKEYS WITH(LSFT|LALT);
  THEN NOKEYS;
}

void TestLayerLockActsAsLayerShift(CuTest *tc) {
  START_KEY_CAPTURE;

  SHOULDNOT_SEND;
  key_press(7);//;layer 1
  CuAssertIntEquals(tc, 1, active_layer);
  VERIFY_DIDNOT_SEND;

  make_dualrole_modifier_possible();//pretend enough time has passed
  key_press(6);//0 or 1 depending on layer
  key_release(6);

  SHOULDNOT_SEND;
  key_release(7);
  CuAssertIntEquals(tc, 0, active_layer);
  VERIFY_DIDNOT_SEND;

  key_press(6);//0 or 1 depending on layer
  key_release(6);

  VERIFY KEY(1);
  THEN   NOKEYS;

  THEN KEY(0);
  THEN   NOKEYS;
}

void TestLayerShiftOrKeyCanTap(CuTest *tc) {
  START_KEY_CAPTURE;

  SHOULDNOT_SEND;
  key_press(5);//F or layer 1
  CuAssertIntEquals(tc, 1, active_layer);
  VERIFY_DIDNOT_SEND;

  key_release(5);
  CuAssertIntEquals(tc, 0, active_layer);

  key_press(0);
  key_release(0);
  CuAssertIntEquals(tc, 0, active_layer);

  VERIFY KEY(F);
  THEN   NOKEYS;

  THEN KEY(A);
  THEN   NOKEYS;
}

void TestLayerShiftOrKeyCanShift(CuTest *tc) {
  START_KEY_CAPTURE;

  SHOULDNOT_SEND;
  key_press(5);
  CuAssertIntEquals(tc, 1, active_layer);
  VERIFY_DIDNOT_SEND;

  make_dualrole_modifier_possible();//pretend enough time has passed

  key_press(6);//0 or 1 depending on layer
  key_release(6);

  SHOULDNOT_SEND;
  key_release(5);
  CuAssertIntEquals(tc, 0, active_layer);
  VERIFY_DIDNOT_SEND;

  VERIFY KEY(1);
  THEN   NOKEYS;
}

void TestModifierOrKeyCanTap(CuTest *tc) {
  START_KEY_CAPTURE;

  key_press(4);//shift or Z
  key_release(4);


  EXPECT_SENT(4);
  VERIFY NOKEYS WITH(LSFT);
  THEN   NOKEYS NOMODS;
  THEN   KEY(Z) NOMODS;
  THEN   NOKEYS NOMODS;
}

void TestModifierOrKeyCanMod(CuTest *tc) {
  START_KEY_CAPTURE;

  key_press(4);//shift or Z
  make_dualrole_modifier_possible();//pretend enough time has passed
  key_press(0);//A
  key_release(0);
  key_release(4);

  key_press(0);//A
  key_release(0);

  EXPECT_SENT(6);
  VERIFY NOKEYS WITH(LSFT);
  THEN   KEY(A) WITH(LSFT);
  THEN   NOKEYS WITH(LSFT);
  THEN   NOKEYS NOMODS;

  THEN   KEY(A) NOMODS;
  THEN   NOKEYS NOMODS;
}

void TestModifierTapModded(CuTest *tc) {
  START_KEY_CAPTURE;

  key_press(2);//rshift or )
  key_release(2);

  EXPECT_SENT(5);
  VERIFY NOKEYS WITH(RSFT);
  THEN   NOKEYS NOMODS;
  THEN   NOKEYS WITH(LSFT);
  THEN   KEY(9) WITH(LSFT);
  THEN   NOKEYS NOMODS;
}

void TestModifierTapRelease(CuTest *tc) {
  START_KEY_CAPTURE;

  key_press(4);//lshift or z
  make_dualrole_modifier_possible();//pretend enough time has passed
  key_press(0);
  key_release(4);
  key_release(0);

  VERIFY NOKEYS WITH(LSFT);
  THEN   KEY(A) WITH(LSFT);
  THEN   KEY(A) NOMODS;
  THEN   NOKEYS NOMODS;
}

void TestRollover(CuTest *tc) {
  START_KEY_CAPTURE;

  CuAssertIntEquals(tc, 0, active_layer);

  SHOULDNOT_SEND;
  key_press(5);//F or layer 1
  VERIFY_DIDNOT_SEND;
  CuAssertIntEquals(tc, 1, active_layer);

  key_press(6);//0 or 1 depending on layer
  EXPECT_SENT(3);
  CuAssertIntEquals(tc, 0, active_layer);//layer already reset

  key_release(5);
  CuAssertIntEquals(tc, 0, active_layer);

  key_release(6);
  EXPECT_SENT(4);

  VERIFY KEY(F);
  THEN   NOKEYS;
  THEN   KEY(0);
  THEN   NOKEYS;
}

void TestCancel(CuTest *tc) {
  START_KEY_CAPTURE;

  SHOULDNOT_SEND;
  key_press(5);//F or layer 1
  VERIFY_DIDNOT_SEND;
  make_dualrole_tap_impossible();//pretend too much time has passed

  SHOULDNOT_SEND;
  key_release(5);
  VERIFY_DIDNOT_SEND;
}

void TestComplicated(CuTest *tc) {
  START_KEY_CAPTURE;

  SHOULDNOT_SEND;
  key_press(5);//F or layer 1
  VERIFY_DIDNOT_SEND;
  CuAssertIntEquals(tc, 1, active_layer);
  make_dualrole_modifier_possible();//pretend enough time has passed

  key_press(0);//LSFT of LBRACKET
  key_release(0);

  CuAssertIntEquals(tc, 1, active_layer);

  SHOULDNOT_SEND;
  key_release(5);
  VERIFY_DIDNOT_SEND;

  CuAssertIntEquals(tc, 0, active_layer);

  EXPECT_SENT(4);
  VERIFY NOKEYS WITH(LSFT);
  THEN NOKEYS NOMODS;
  THEN KEY(LBRACKET);
  THEN NOKEYS;
}

void TestSpaceFn(CuTest *tc) {
  START_KEY_CAPTURE;

  key_press(3);
  key_release(3);

  SHOULDNOT_SEND;
  key_press(16*8+6);//SPACE
  CuAssertIntEquals(tc, 1, active_layer);
  VERIFY_DIDNOT_SEND;

  key_release(16*8+6);//SPACE
  CuAssertIntEquals(tc, 0, active_layer);

  SHOULDNOT_SEND;
  key_press(16*8+6);//SPACE
  CuAssertIntEquals(tc, 1, active_layer);
  VERIFY_DIDNOT_SEND;

  make_dualrole_modifier_possible();//pretend enough time has passed

  key_press(11*8+3);//K or down
  key_release(11*8+3);

  SHOULDNOT_SEND;
  key_release(16*8+6);//SPACE
  CuAssertIntEquals(tc, 0, active_layer);
  VERIFY_DIDNOT_SEND;

  VERIFY NOKEYS WITH(RGUI);
  THEN   NOKEYS NOMODS;
  THEN   KEY(SPACE) NOMODS;
  THEN   NOKEYS NOMODS;

  THEN   KEY(DOWN);
  THEN   NOKEYS;
}

CuSuite* CuStringGetSuite(void)
{
  CuSuite* suite = CuSuiteNew();

#ifdef TEST_SPACEFN
  SUITE_ADD_TEST(suite, TestSpaceFn);
#else
  SUITE_ADD_TEST(suite, TestKey);
  SUITE_ADD_TEST(suite, TestKeyModded);
  SUITE_ADD_TEST(suite, TestLayerLock);
  SUITE_ADD_TEST(suite, TestModModded);
  SUITE_ADD_TEST(suite, TestLayerLockActsAsLayerShift);
  SUITE_ADD_TEST(suite, TestLayerShiftOrKeyCanShift);
  SUITE_ADD_TEST(suite, TestLayerShiftOrKeyCanTap);
  SUITE_ADD_TEST(suite, TestModifierOrKeyCanTap);
  SUITE_ADD_TEST(suite, TestModifierOrKeyCanMod);
  SUITE_ADD_TEST(suite, TestModifierTapModded);
  SUITE_ADD_TEST(suite, TestModifierTapRelease);
  SUITE_ADD_TEST(suite, TestRollover);
  SUITE_ADD_TEST(suite, TestCancel);
  SUITE_ADD_TEST(suite, TestComplicated);
#endif

  return suite;
}
