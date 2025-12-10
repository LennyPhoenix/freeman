#include "filesystem.h"
#include "menu.h"

#include <stdio.h>
#include <string.h>

int main(void) {
  FileError error = fs_ensure();
  if (error) {
    printf("Something went wrong, file error %d\n", error);
  }

  MenuItem pref_menu = {
      .default_prompt = "Update Preferences",
      .function = preferences_menu,
      .status_check = preferences_status,
  };

  MenuItem items[] = {pref_menu};
  size_t item_c = sizeof(items) / sizeof(MenuItem);

  Menu menu = {
      .title = "Freeman",
      .menu_data = NULL,
      .items = items,
      .item_c = item_c,
  };

  open_menu(&menu);
}
