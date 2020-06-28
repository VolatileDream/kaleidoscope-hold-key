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

EventHandlerResult HoldKey_::onKeyswitchEvent(Key &mapped_key, KeyAddr addr, uint8_t key_state) {
  if (mapped_key == Key_HoldKey && state == WAITING) {
    // Start listening if the key is being released.
    if (keyToggledOff(key_state)) { state = LISTENING; }
  } else if (state == LISTENING && keyToggledOff(key_state) && holdableKey(mapped_key)) {
    // Only record the key on key-up, this means we don't interfere with the
    // user pressing and releasing keys.
    hold_ = mapped_key;
    state = HOLDING;
  } else if (state == HOLDING && holdableKey(mapped_key)) {
    // Something got pressed, stop holding.
    state = WAS_HOLDING;
  }

  return EventHandlerResult::OK;
}

EventHandlerResult HoldKey_::afterEachCycle() {
  switch (state) {
   default:
   case WAITING:
   case LISTENING:
    break;
   case HOLDING:
    Runtime.hid().keyboard().pressKey(hold_, false);
    break;
   case WAS_HOLDING:
    Runtime.hid().keyboard().releaseKey(hold_);
    state = WAITING;
    break;
  }
  return EventHandlerResult::OK;
}

}
}

kaleidoscope::plugin::HoldKey_ HoldKey;
