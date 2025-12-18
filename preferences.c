#include "preferences.h"

#include "date.h"
#include "filesystem.h"
#include "input.h"
#include "menu.h"

#include <ctype.h>
#include <stddef.h>
#include <stdio.h>

ItemStatus preferences_status(void *_menu_data, void *_item_data) {
  // Default to available
  ItemStatus status = {.available = true, .prompt = {0}};

  // Get existing preferences
  Preferences preferences;
  FileError error = fs_get_preferences(&preferences);
  if (error) {
    status.available = false;
    sprintf(
        status.prompt,
        "Failed to read preferences file (error %d), try restarting `freeman`",
        error);
    return status;
  }

  // List all unassigned preferences, using X macro tables to generate code for
  // each item
  char missing[64] = {0};
#define X(symbol, display_name)                                                \
  if (!preferences.symbol) {                                                   \
    if (*missing) {                                                            \
      sprintf(missing, "%s, " display_name, missing);                          \
    } else {                                                                   \
      sprintf(missing, display_name);                                          \
    }                                                                          \
  }
  PREFERENCES_TABLE
#undef X

  // Display any missing preferences to main menu
  if (*missing) {
    sprintf(status.prompt, "Update Preferences (Missing: %s)", missing);
  }

  return status;
}

MenuError preferences_menu(void *_menu_data, void *_item_data) {
  // Load preferences
  Preferences preferences;
  FileError error = fs_get_preferences(&preferences);
  if (error) {
    return MENU_ITEM_ERROR;
  }

  // Generate menu item automatically for each preference (more X macro tables!)
#define X(symbol, display)                                                     \
  MenuItem symbol = {                                                          \
      .default_prompt = "Set " display,                                        \
      .function = (MenuItemFn)update_##symbol,                                 \
      .status_check = (StatusCheckFn)symbol##_status,                          \
  };
  PREFERENCES_TABLE
#undef X

  // Add all menu items to list
  MenuItem items[] = {
#define X(symbol, _display) symbol,
      PREFERENCES_TABLE
#undef X
  };
  size_t item_c = sizeof(items) / sizeof(MenuItem);
  MenuItem *items_pointer = items;

  Menu menu = {
      .title = "Preferences",
      .items = &items_pointer,
      .item_c = &item_c,
      .menu_data = &preferences,
  };

  return open_menu(&menu);
}

// Define status check functions for each preference, look at LSP for expansion
#define X(pref_symbol, display_name)                                           \
  ItemStatus pref_symbol##_status(Preferences *preferences,                    \
                                  void *_item_data) {                          \
    ItemStatus status = {                                                      \
        .available = true,                                                     \
        .prompt = {0},                                                         \
    };                                                                         \
                                                                               \
    if (preferences->pref_symbol) {                                            \
      sprintf(status.prompt, "Update " display_name " (£%.2f/day)",            \
              preferences->pref_symbol);                                       \
    } else {                                                                   \
      sprintf(status.prompt, "Set " display_name);                             \
    }                                                                          \
                                                                               \
    return status;                                                             \
  }
PREFERENCES_TABLE
#undef X

// Define update functions for each preference type, look at LSP for expansion
#define X(symbol, display_name)                                                \
  MenuError update_##symbol(Preferences *preferences, void *_item_data) {      \
    unsigned int days = 0;                                                     \
                                                                               \
    while (!days) {                                                            \
      printf("[M]onthly, [W]eekly or [D]aily? ([C]ancel)\n: ");                \
      char input = tolower(getc(stdin));                                       \
      flush_input_buffer();                                                    \
                                                                               \
      switch (input) {                                                         \
      case 'w':                                                                \
        days = 7;                                                              \
        break;                                                                 \
      case 'm':                                                                \
        days = days_this_month();                                              \
        break;                                                                 \
      case 'd':                                                                \
        days = 1;                                                              \
        break;                                                                 \
      case 'c':                                                                \
        return MENU_OK;                                                        \
      }                                                                        \
    }                                                                          \
                                                                               \
    printf("Enter value as a decimal: ");                                      \
                                                                               \
    double value;                                                              \
    while (read_double(&value)) {                                              \
      printf("Invalid number.\n: ");                                           \
    }                                                                          \
    value /= days;                                                             \
                                                                               \
    printf(display_name " = £%.2f/day\n", value);                              \
                                                                               \
    preferences->symbol = value;                                               \
                                                                               \
    FileError error = fs_set_preferences(*preferences);                        \
    if (error) {                                                               \
      printf("Failed to save preferences...\n");                               \
      return MENU_ITEM_ERROR;                                                  \
    }                                                                          \
                                                                               \
    return MENU_OK;                                                            \
  }
PREFERENCES_TABLE
#undef X
