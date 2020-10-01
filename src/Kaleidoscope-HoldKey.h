#pragma once

#include "Kaleidoscope.h"
#include "Kaleidoscope-Ranges.h"

// Required in order to add the key to the keymap.
//
// You need to create `kaleidoscope::ranges::HOLDKEY` in order to use
// this plugin.
#define Key_HoldKey Key{kaleidoscope::ranges::HOLDKEY}

// The maximum number of keys that can be held simultaneously.
//
// Note: Recompile the plugin itself if you update this value!!
#define HOLDKEY_COUNT 6

#define HOLD_FAIL_TIMEOUT_MILLIS 3000

namespace kaleidoscope {
namespace plugin {

// State of the plugin. Used to listen for the next key press.
// Most event transitions take place in `onKeyswitchEvent`,
// WAS_HOLDING -> WAITING happens in `afterEachCycle`.
enum HoldKeyState {
  WAITING,
  LISTENING,
  HOLDING,
  WAS_HOLDING,
  HOLD_FAILED,
};

// HoldKey_:
//
//  * Press `Key_HoldKey`
//  * At the end of the next key cycle (`afterEachCycle`), HoldKey_ records
//    upto HOLDKEY_COUNT keys to be held.
//    * If too many keys are held, HoldKey_ will hold no keys and attempt to
//      flash Key_HoldKey.
//  * Press any key, HoldKey_ disengages holding all the keys.
//
class HoldKey_ : public kaleidoscope::Plugin {
 public:
  HoldKey_(void);

  static bool holdableKey(Key mapped_key);

  EventHandlerResult onKeyswitchEvent(Key& mapped_key, KeyAddr addr, uint8_t key_state);
  EventHandlerResult afterEachCycle();

 private:
  HoldKeyState state;
  Key hold_[HOLDKEY_COUNT];
  KeyAddr special_key_; // only valid after a transition from WAITING to LISTENING.
  uint32_t fail_start_;
};

}
}

extern kaleidoscope::plugin::HoldKey_ HoldKey;
