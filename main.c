#include "activity.h"
#include "balance.h"
#include "filesystem.h"
#include "menu.h"
#include "preferences.h"
#include "project.h"

#include <cyaml/cyaml.h>
#include <stdio.h>
#include <string.h>

int main(void) {
  // Check that the filesystem is intact
  FileError error = fs_ensure();
  if (error) {
    printf("Something went wrong, file error %d\n", error);
  }

  // Main Menu
  MenuItem preferences_menu_item = {
      .default_prompt = "Update Preferences",
      .function = preferences_menu,
      .status_check = preferences_status,
      .item_data = NULL,
  };

  MenuItem projects_menu_item = {
      .default_prompt = "Manage Projects",
      .item_data = NULL,
      .status_check = projects_status,
      .function = projects_menu,
  };

  MenuItem log_activity_item = {
      .default_prompt = "Log Activity",
      .item_data = NULL,
      .status_check = new_activity_menu_status,
      .function = new_activity_menu,
  };

  MenuItem balance_menu_item = {
      .default_prompt = "Calculate Balance",
      .item_data = NULL,
      .status_check = NULL,
      .function = balance_menu,
  };

  MenuItem items[] = {preferences_menu_item, projects_menu_item,
                      log_activity_item, balance_menu_item};
  size_t item_c = sizeof(items) / sizeof(MenuItem);
  MenuItem *items_pointer = items;

  Menu menu = {
      .title = "freeman",
      .menu_data = NULL,
      .items = &items_pointer,
      .item_c = &item_c,
  };
  return open_menu(&menu);
}
