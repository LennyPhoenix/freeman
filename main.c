#include "filesystem.h"
#include "menu.h"
#include "project.h"

#include <cyaml/cyaml.h>
#include <stdio.h>
#include <string.h>

int main(void) {
  FileError error = fs_ensure();
  if (error) {
    printf("Something went wrong, file error %d\n", error);
  }

  Project project = {
      .name = "Test Project A",
      .id = 0,
      .activities = NULL,
      .activity_c = 0,
      .default_rate = 20.0,
  };
  fs_save_project(project);

  Project **projects;
  size_t project_c;
  fs_get_project_list(&projects, &project_c);
  for (int i = 0; i < project_c; i++) {
    Project *project = projects[i];
    printf("Found project: %s\n", project->name);
  }
  fs_free_project_list(projects, project_c);

  MenuItem pref_menu = {
      .default_prompt = "Update Preferences",
      .function = preferences_menu,
      .status_check = preferences_status,
  };

  MenuItem items[] = {pref_menu};
  size_t item_c = sizeof(items) / sizeof(MenuItem);

  Menu menu = {
      .title = "freeman",
      .menu_data = NULL,
      .items = items,
      .item_c = item_c,
  };

  open_menu(&menu);
}
