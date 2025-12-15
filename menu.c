#include "menu.h"

#include "input.h"

#include <stdio.h>
#include <string.h>

MenuError open_menu(Menu *menu) {
  while (true) {
    // Print menu
    if (display_menu(menu)) {
      return MENU_DISPLAY_ERROR;
    }

    // Get user input
    int choice;
    while (get_menu_choice(*menu->item_c, &choice)) {
      printf("Invalid choice\n");
    }

    // Exit
    if (choice == *menu->item_c) {
      break;
    }

    // Get selected item
    MenuItem *item = *menu->items + choice;
    ItemStatus status;
    bool available = true;

    // Check status, if applicable
    if (item->status_check) {
      status = item->status_check(menu->menu_data, item->item_data);
      available = status.available;
    }

    // Call if available, otherwise display an error
    if (available) {
      MenuError error = item->function(menu->menu_data, item->item_data);
      if (error == MENU_EXIT) {
        return MENU_OK;
      } else if (error) {
        return error;
      }
    } else {
      // Code path is only possible if status_check was not NULL, avoiding UB
      printf("That option is unavailable right now: %s\n", status.prompt);
      wait_for_enter();
    }
  }

  return MENU_OK;
}

MenuError display_menu(Menu *menu) {
  printf("\n= %s =\n", menu->title);

  for (int i = 0; i < *menu->item_c; i++) {
    MenuItem *item = *menu->items + i;

    char prompt[PROMPT_SIZE];
    strcpy(prompt, item->default_prompt);

    // 10 is a generous number of digits, just being safe
    const size_t MARKER_SIZE = 10;
    char marker[MARKER_SIZE];
    sprintf(marker, "%d", i + 1);

    if (item->status_check) {
      ItemStatus status = item->status_check(menu->menu_data, item->item_data);

      if (!status.available) {
        sprintf(marker, "X");
      }

      if (*status.prompt) {
        strcpy(prompt, status.prompt);
      }
    }

    printf("%s. %s\n", marker, prompt);
  }

  printf("%zu. Exit\n", *menu->item_c + 1);

  return MENU_OK;
}

MenuError get_menu_choice(size_t item_c, int *choice_out) {
  int choice = 0;

  while (read_int(&choice)) {
  }

  if (choice < 1 || choice > item_c + 1) {
    return MENU_INVALID_CHOICE;
  }

  *choice_out = choice - 1;

  return MENU_OK;
}
