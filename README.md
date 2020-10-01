# Kaleidoscope-HoldKey

This is a plugin for [Kaleidoscope][fw]. It allows users to hold up to 6 keys
(already on their keyboard) until another key is pressed. It adds a single
special key which records all pressed keys, which it continues to hold for the
user until another key is pressed. It ignores layer changing key presses.

This can be used to:

 * Hold down Shift to open grub on boot
 * Hold down Backspace to delete a document
 * Hold down W+A to walk forward and to the left

# Setup

You'll need to patch `Kaleidoscope-Ranges.h` to add the following into the enum:

```
enum : uint16_t {

  ...
  TURBO,
+  HOLDKEY,
  ...

}
```

---

# Example

```
// Support key holding
#include "Kaleidoscope-HoldKey.h"

...

KEYMAPS(
  // Add the Special Key.
  [LAYER_QWERTY] = KEPMAP_STACKED(..., Key_HoldKey, ...),
  ...
);

...


KALEIDOSCOPE_INIT_PLUGINS(
  ...
  HoldKey,
  ...
);

```

 [fw]: https://github.com/keyboardio/Kaleidoscope
