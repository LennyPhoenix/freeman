#ifndef MENU_H_
#define MENU_H_

#include <stdbool.h>
#include <stdlib.h>

#define PROMPT_SIZE (64)

typedef enum {
  MENU_OK = 0,
  MENU_DISPLAY_ERROR = 1,
  MENU_ITEM_ERROR = 2,
  MENU_INVALID_CHOICE = 3,
} MenuError;

/// Status of a menu item.
typedef struct {
  bool available;
  char prompt[PROMPT_SIZE];
} ItemStatus;

/// Function pointer to get the current status of a menu item.
typedef ItemStatus (*StatusCheckFn)(void *menu_data);
/// Function pointer to execute a menu item.
typedef MenuError (*MenuItemFn)(void *menu_data);

/// A selectable item within a menu.
typedef struct {
  StatusCheckFn status_check;
  MenuItemFn function;
  char *default_prompt;
} MenuItem;

/// A menu, contains any number of menu items and some arbitrary
/// data that can be passed around.
typedef struct {
  char *title;
  void *menu_data;
  MenuItem *items;
  size_t item_c;
} Menu;

/// Opens a menu.
MenuError open_menu(Menu *menu);

/// Prints out a menu to stdout.
MenuError display_menu(Menu *menu);

/// Reads a menu selection from stin, validating with the item count.
MenuError get_menu_choice(size_t item_c, int *choice_out);

#endif
