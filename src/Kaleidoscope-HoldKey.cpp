#include "Kaleidoscope.h"
#include "Kaleidoscope-HoldKey.h"
#include "Kaleidoscope-Ranges.h"
#include "Kaleidoscope-LEDControl.h"
#include "kaleidoscope/keyswitch_state.h"

namespace kaleidoscope {
namespace plugin {

__attribute__((weak)) bool HoldKey_::holdableKey(Key mapped_key) {
  //*
  return !(mapped_key.getFlags() & SWITCH_TO_KEYMAP);
  /*/
  return (Key_A <= mapped_key && mapped_key <= Key_Z)
      || (Key_1 <= mapped_key && mapped_key <= Key_0)
      || (Key_LeftControl <= mapped_key && mapped_key <= Key_RightGui);
  //*/
}

static void reset_array(Key held[HOLDKEY_COUNT]) {
  for (uint8_t i = 0; i < HOLDKEY_COUNT; i++) {
    held[i] = Key_NoKey;
  }
}

HoldKey_::HoldKey_() : state(WAITING), fail_start_(0) {
  reset_array(hold_);
}

EventHandlerResult HoldKey_::onKeyswitchEvent(Key &mapped_key, KeyAddr addr, uint8_t key_state) {
  if (mapped_key == Key_HoldKey && (state == WAITING || state == HOLD_FAILED)) {
    special_key_ = addr;
    // Start listening if the key is being released.
    if (keyToggledOff(key_state)) { state = LISTENING; }
  } else if (state == HOLDING && holdableKey(mapped_key) && keyToggledOn(key_state)) {
    // Something got pressed, stop holding.
    state = WAS_HOLDING;
  }

  return EventHandlerResult::OK;
}

// returns false if more than HOLDKEY_COUNT keys were held, or zero keys were held.
static bool update_held_keys(Key held[HOLDKEY_COUNT]) {
  reset_array(held);
  // Setup the array.
  uint8_t held_count = 0;
  for (uint8_t r = 0; r < Kaleidoscope.device().matrix_rows; r++) {
    for (uint8_t c = 0; c < Kaleidoscope.device().matrix_columns; c++) {
      KeyAddr addr = KeyAddr(r, c);

      if (!Kaleidoscope.device().isKeyswitchPressed(addr)) continue;

      if (held_count < HOLDKEY_COUNT) {
        // Update the array if we aren't holding too many keys.
        held[held_count] = Layer.lookupOnActiveLayer(addr);
      }
      held_count++;
    }
  }
  if (held_count > HOLDKEY_COUNT) {
    // Array was updated as we went, clear it because too many keys were held.
    reset_array(held);
  }
  return 0 < held_count && held_count <= HOLDKEY_COUNT;
}

static void update_keys(Key held[HOLDKEY_COUNT], bool holding) {
  for (uint8_t i = 0; i < HOLDKEY_COUNT; i++) {
    Key k = held[i];
    if (k != Key_NoKey) {
      if (holding) {
        Runtime.hid().keyboard().pressKey(k, false);
      } else {
        Runtime.hid().keyboard().releaseKey(k);
      }
    }
  }
}

EventHandlerResult HoldKey_::afterEachCycle() {
  switch (state) {
   default:
   case WAITING:
    break;
   case LISTENING:
    if (!update_held_keys(hold_)) {
      state = HOLD_FAILED;
      fail_start_ = Runtime.millisAtCycleStart();
    } else {
      state = HOLDING;
    }
    break;
   case WAS_HOLDING:
    state = WAITING;
    // fallthrough.
   case HOLDING:
    update_keys(hold_, state == HOLDING);
    if (state == HOLDING) {
      ::LEDControl.setCrgbAt(special_key_, breath_compute(85));
    } else {
      ::LEDControl.refreshAt(special_key_);
    }
    break;
   case HOLD_FAILED:
    if (!Runtime.hasTimeExpired(fail_start_, HOLD_FAIL_TIMEOUT_MILLIS)) {
      ::LEDControl.setCrgbAt(special_key_, breath_compute(0));
    } else {
      ::LEDControl.refreshAt(special_key_);
    }
    break;
  }
  return EventHandlerResult::OK;
}

}
}

kaleidoscope::plugin::HoldKey_ HoldKey;
