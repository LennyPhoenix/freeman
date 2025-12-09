#ifndef MENU_H_
#define MENU_H_

#include <stdbool.h>
#include <stdlib.h>

#define PROMPT_SIZE (64)

typedef enum MenuError {
  MENU_OK = 0,
  /// Something went wrong printing the menu.
  MENU_DISPLAY_ERROR = 1,
  /// Something went wrong executing a menu item.
  MENU_ITEM_ERROR = 2,
  /// User entered an invalid menu choice.
  MENU_INVALID_CHOICE = 3,
} MenuError;

/// Status of a menu item.
typedef struct ItemStatus {
  /// Is this menu item accessible right now?
  bool available;
  /// Prompt to display in the menu for this item.
  char prompt[PROMPT_SIZE];
} ItemStatus;

/// Function pointer to get the current status of a menu item.
typedef ItemStatus (*StatusCheckFn)(void *menu_data);
/// Function pointer to execute a menu item.
typedef MenuError (*MenuItemFn)(void *menu_data);

/// A selectable item within a menu.
typedef struct MenuItem {
  /// (Optional) Function pointer to check the status of this item.
  StatusCheckFn status_check;
  /// Function pointer to execute this item.
  MenuItemFn function;
  /// Default prompt to display if no status check is provided.
  char *default_prompt;
} MenuItem;

/// A menu, contains any number of menu items and some arbitrary
/// data that can be passed around.
typedef struct Menu {
  /// Main title, displayed at the top of the menu.
  char *title;
  /// Shared data to pass between menu items.
  void *menu_data;
  /// List of items in the menu.
  MenuItem *items;
  /// Item count.
  size_t item_c;
} Menu;

/// Opens a menu.
MenuError open_menu(Menu *menu);

/// Prints out a menu to stdout.
MenuError display_menu(Menu *menu);

/// Reads a menu selection from stin, validating with the item count.
MenuError get_menu_choice(size_t item_c, int *choice_out);

#endif
