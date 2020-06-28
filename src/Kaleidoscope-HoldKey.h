#pragma once

#include "Kaleidoscope.h"
#include "Kaleidoscope-Ranges.h"

#define Key_HoldKey Key{kaleidoscope::ranges::HOLDKEY}

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
};

class HoldKey_ : public kaleidoscope::Plugin {
 public:
  HoldKey_(void) {}

  static bool holdableKey(Key mapped_key);

  EventHandlerResult onKeyswitchEvent(Key& mapped_key, KeyAddr addr, uint8_t key_state);
  EventHandlerResult afterEachCycle();

 private:
  HoldKeyState state = WAITING;
  Key hold_ = Key_NoKey;
};

}
}

extern kaleidoscope::plugin::HoldKey_ HoldKey;
