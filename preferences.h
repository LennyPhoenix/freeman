#ifndef PREFERENCES_H_
#define PREFERENCES_H_

// Use X-macros here as all the logic for each preference is identical, and this
// makes it very easy to implement logic for all preferences at once.
#define PREFERENCES_TABLE                                                      \
  X(rent, "Rent")                                                              \
  X(living_costs, "Living Costs")                                              \
  X(savings_goal, "Savings Goal")

typedef struct Preferences {
#define X(symbol, _disp) double symbol;
  PREFERENCES_TABLE
#undef X
} Preferences;

#include "menu.h"

/// Gets the status of the preference menu.
ItemStatus preferences_status(void *_menu_data, void *_item_data);
/// Opens the preference menu.
MenuError preferences_menu(void *_menu_data, void *_item_data);

// Define menu items for each preference.
#define X(symbol, _disp)                                                       \
  ItemStatus symbol##_status(Preferences *preferences, void *_item_data);      \
  MenuError update_##symbol(Preferences *preferences, void *_item_data);
PREFERENCES_TABLE
#undef X

#endif
