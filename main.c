#include "input.h"
#include "menu.h"

#include <stdio.h>
#include <string.h>

MenuError hello_world(void *data) {
  printf("Hello, world!\n");
  wait_for_enter();
  return MENU_OK;
}

MenuError increment(int *counter) {
  *counter += 1;
  printf("Counter now at %d\n", *counter);
  wait_for_enter();
  return MENU_OK;
}

ItemStatus check_increment_status(int *counter) {
  ItemStatus status = {
      .available = true,
      .prompt = {0},
  };

  if (*counter > 10) {
    status.available = false;
    strcpy(status.prompt, "I've run out of fingers...");
  } else {
    sprintf(status.prompt, "Increment counter! (%d)", *counter);
  }

  return status;
}

int main(void) {
  MenuItem item_1 = {
      .default_prompt = "Say Hi",
      .function = hello_world,
      .status_check = NULL,
  };

  MenuItem item_2 = {
      .default_prompt = "Increment Counter",
      .function = (MenuItemFn)increment,
      .status_check = (StatusCheckFn)check_increment_status,
  };

  MenuItem items[] = {item_1, item_2};
  size_t item_c = sizeof(items) / sizeof(MenuItem);

  int counter = 0;
  Menu menu = {
      .title = "Main Menu",
      .menu_data = &counter,
      .items = items,
      .item_c = item_c,
  };

  open_menu(&menu);
}
