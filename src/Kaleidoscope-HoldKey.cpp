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

bool HoldKey_::addHeldKey(const KeyEvent &k) {
  if (held_ >= HOLDKEY_COUNT) {
    return false;
  }
  hold_[held_] = k;
  held_++;
  return true;
}

void HoldKey_::releaseHeldKeys() {
  for (uint8_t i = 0; i < held_; i++) {
    Kaleidoscope.handleKeyEvent(hold_[i]);
  }
  held_ = 0;
}

EventHandlerResult HoldKey_::onKeyEvent(KeyEvent &event) {
  // When the HOLD key is pressed, we do nothing. Only when it is released do
  // we start tracking key events:
  //  * any release event gets swallowed, the released key is recorded.
  //  * any key press causes us to bail.

  if (state == HOLDING && holdableKey(event.key)) {
    if (keyToggledOff(event.state)) {
      if (addHeldKey(event)) {
        // We're attempting to delay the key release until the user presses
        // more keys at some later point in time, ergo: ABORT.
        return EventHandlerResult::ABORT;
      } else {
        // When we fail to handle the hold (ie: too many keys held)
        // release all keys and signal that the hold failed.
        state = HOLD_FAILED;
        fail_start_ = Runtime.millisAtCycleStart();
        releaseHeldKeys();
      }
    } else if (keyToggledOn(event.state)) {
      state = WAS_HOLDING;
      releaseHeldKeys();
    }
    // Events for keys staying held are ignored.

  // When not holding down keys that are released, the only key press that we
  // have any interest in is our special Key_HoldKey.
  } else if (event.key == Key_HoldKey) {
    // Record the key address to light it up later.
    special_key_ = event.addr;
    // No event is received when there are no keys to hold down, but we still
    // want to switch to HOLD_FAILED in that case: so we query the device for
    // how many keys are being held down, but we don't (yet) need to know what
    // they are.
    //
    // Toggle On events are safely ignored.
    if (keyToggledOff(event.state)) {
      if (Kaleidoscope.device().pressedKeyswitchCount() > 0) {
        state = HOLDING;
      } else {
        state = HOLD_FAILED;
        fail_start_ = Runtime.millisAtCycleStart();
      }
    } else if (state == HOLDING) {
      // This key also counts for interrupting the HOLDING state.
      state = WAS_HOLDING;
      releaseHeldKeys();
    }
    // This is our key, now it's been handled.
    return EventHandlerResult::OK;
  }

  return EventHandlerResult::OK;
}

EventHandlerResult HoldKey_::afterEachCycle() {
  switch (state) {
   default:
   case WAITING:
    break;
   case WAS_HOLDING:
    state = WAITING;
    ::LEDControl.refreshAt(special_key_);
    break;
   case HOLDING:
    ::LEDControl.setCrgbAt(special_key_, breath_compute(85));
    break;
   case HOLD_FAILED:
    // This state expires eventually.
    if (Runtime.hasTimeExpired(fail_start_, HOLD_FAIL_TIMEOUT_MILLIS)) {
      ::LEDControl.refreshAt(special_key_);
      state = WAITING;
    } else {
      // Until the state expires, breath the HOLD key.
      ::LEDControl.setCrgbAt(special_key_, breath_compute(0));
    }
    break;
  }
  return EventHandlerResult::OK;
}

}
}

kaleidoscope::plugin::HoldKey_ HoldKey;
