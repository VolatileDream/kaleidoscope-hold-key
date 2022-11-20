#pragma once

#include "Kaleidoscope.h"
#include "Kaleidoscope-Ranges.h"

#include "kaleidoscope/KeyEventTracker.h"

// Required in order to add the key to the keymap.
//
// You need to create `kaleidoscope::ranges::HOLDKEY` in order to use
// this plugin.
constexpr Key Key_HoldKey = Key{kaleidoscope::ranges::HOLDKEY};

// The maximum number of keys that can be held simultaneously.
//
// Note: Recompile the plugin itself if you update this value!!
#define HOLDKEY_COUNT 6

#define HOLD_FAIL_TIMEOUT_MILLIS 3000

namespace kaleidoscope {
namespace plugin {

// State of the plugin.
enum HoldKeyState {
  WAITING,
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
  static bool holdableKey(Key mapped_key);

  // onKeyswitchEvent is used to listen for Key_HoldKey being released, and any
  // follow up key releases (up to HOLDKEY_COUNT) that should remain held.
  EventHandlerResult onKeyEvent(KeyEvent &event);
  EventHandlerResult afterEachCycle();

 private:
  KeyEventTracker event_tracker_;
  HoldKeyState state = WAITING;
  // only valid during HOLDING and HOLD_FAILED.
  KeyAddr special_key_ = KeyAddr::none();
  uint8_t held_ = 0;
  KeyEvent hold_[HOLDKEY_COUNT];

  uint32_t fail_start_ = 0; // time when

  bool addHeldKey(const KeyEvent &k);
  void releaseHeldKeys();
};

}
}

extern kaleidoscope::plugin::HoldKey_ HoldKey;
