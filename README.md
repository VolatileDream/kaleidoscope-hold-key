# Kaleidoscope-HoldKey

This is a plugin for [Kaleidoscope][fw]. It allows users to hold _one_ specific
key (already on their keyboard) until another key is pressed. It adds a single
special key to start listening, and then records the next key press which it
continues to hold down until another key is pressed. It ignores layer changes.

This can be used to:

 * Hold down Shift to open grub on boot
 * Hold down W to walk forward
 * Hold down Backspace to delete a document

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
