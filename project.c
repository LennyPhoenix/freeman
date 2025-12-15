#include "project.h"

#include "error.h"
#include "filesystem.h"
#include "input.h"
#include "menu.h"

#include <ctype.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

ItemStatus projects_status(void *_menu_data, void *_item_data) {
  ItemStatus status = {
      .available = true,
      .prompt = {0},
  };

  Project **projects;
  size_t project_c;
  FileError error = fs_get_project_list(&projects, &project_c);
  if (error) {
    sprintf(status.prompt, "Error reading project list (FileError %d)", error);
    return status;
  }

  if (project_c) {
    sprintf(status.prompt, "Manage Projects (%zu)", project_c);
  } else {
    sprintf(status.prompt, "Add a Project!");
  }

  error = fs_free_project_list(projects, project_c);
  if (error) {
    sprintf(status.prompt, "Error freeing project list (FileError %d)", error);
    return status;
  }

  return status;
}

MenuError projects_menu(void *_menu_data, void *_item_data) {
  Project **projects = NULL;
  size_t project_c = 0;
  size_t item_c = 0;
  MenuItem *menu_items = NULL;
  ProjectMenuItemData *menu_item_data = NULL;

  ProjectMenuData menu_data = {
      .projects = &projects,
      .project_c = &project_c,
      .menu_item_data = &menu_item_data,
      .menu_items = &menu_items,
      .item_c = &item_c,
  };

  PROPAGATE(MenuError, reload_projects, (&menu_data));

  Menu menu = {
      .menu_data = &menu_data,
      .items = &menu_items,
      .item_c = &item_c,
      .title = "Manage Projects",
  };
  PROPAGATE(MenuError, open_menu, (&menu));

  PROPAGATE(MenuError, free_project_menu, (&menu_data));

  return MENU_OK;
}

MenuError project_item_menu(ProjectMenuData *menu_data,
                            ProjectMenuItemData *item_data) {
  // Make temporary copy so that changes must be saved manually
  Project project = *item_data->project;

  MenuItem name_item = {
      .item_data = NULL,
      .function = (MenuItemFn)project_name,
      .default_prompt = "Update Name",
      .status_check = (StatusCheckFn)project_name_status,
  };

  MenuItem default_rate_item = {
      .item_data = NULL,
      .function = (MenuItemFn)project_default_rate,
      .default_prompt = "Update Default Rate",
      .status_check = (StatusCheckFn)project_default_rate_status,
  };

  MenuItem save_item = {
      .item_data = menu_data,
      .function = (MenuItemFn)project_commit,
      .default_prompt = "Save and Exit",
      .status_check = (StatusCheckFn)project_commit_status,
  };

  MenuItem delete_item = {
      .item_data = menu_data,
      .function = (MenuItemFn)project_delete,
      .default_prompt = "Delete Project",
      .status_check = NULL,
  };

  MenuItem items[] = {name_item, default_rate_item, save_item, delete_item};
  size_t item_c = sizeof(items) / sizeof(MenuItem);
  MenuItem *items_pointer = items;

  Menu menu = {
      .item_c = &item_c,
      .items = &items_pointer,
      .menu_data = &project,
      .title = "Edit Project",
  };

  PROPAGATE(MenuError, open_menu, (&menu));

  return MENU_OK;
}

MenuError add_project(ProjectMenuData *menu_data, void *_item_data) {
  unsigned long id = 0;
  for (int i = 0; i < *menu_data->project_c; i++) {
    Project *project = (*menu_data->projects)[i];
    unsigned long project_id = project->id;

    if (project_id > id) {
      id = project_id;
    }
  }
  id++;

  Project project = {
      .id = id,
      .name = {0},
      .activities = NULL,
      .activity_c = 0,
      .default_rate = 0.0,
  };

  MenuItem name_item = {
      .item_data = NULL,
      .function = (MenuItemFn)project_name,
      .default_prompt = "Set Name",
      .status_check = (StatusCheckFn)project_name_status,
  };

  MenuItem default_rate_item = {
      .item_data = NULL,
      .function = (MenuItemFn)project_default_rate,
      .default_prompt = "Set Default Rate",
      .status_check = (StatusCheckFn)project_default_rate_status,
  };

  MenuItem commit_item = {
      .item_data = menu_data, // for reallocation
      .function = (MenuItemFn)project_commit,
      .default_prompt = "Save Project",
      .status_check = (StatusCheckFn)project_commit_status,
  };

  MenuItem items[] = {name_item, default_rate_item, commit_item};
  size_t item_c = sizeof(items) / sizeof(MenuItem);
  MenuItem *items_pointer = items;

  Menu menu = {
      .title = "Add Project",
      .item_c = &item_c,
      .items = &items_pointer,
      .menu_data = &project,
  };

  return open_menu(&menu);
}

MenuError project_name(Project *project, void *_item_data) {
  printf("Enter the project title");

  while (read_string(project->name)) {
    printf("Invalid input\n");
  }

  return MENU_OK;
}

ItemStatus project_name_status(Project *project, void *_item_data) {
  ItemStatus status = {
      .available = true,
      .prompt = "Set Name",
  };

  if (*project->name) {
    sprintf(status.prompt, "Update Name (%s)", project->name);
  }

  return status;
}

MenuError project_default_rate(Project *project, void *_item_data) {
  printf("Enter default hourly rate (£/hour) for the project");
  while (read_float(&project->default_rate)) {
    printf("Invalid input\n");
  }
  return MENU_OK;
}

ItemStatus project_default_rate_status(Project *project, void *_item_data) {
  ItemStatus status = {
      .available = true,
  };

  sprintf(status.prompt, "Update Rate (£%.2f/hour)", project->default_rate);

  return status;
}

MenuError project_commit(Project *project, ProjectMenuData *project_menu_data) {
  FileError error = fs_save_project(*project);

  PROPAGATE(MenuError, reload_projects, (project_menu_data));

  return MENU_EXIT;
}

MenuError project_delete(Project *project, ProjectMenuData *project_menu_data) {
  printf("Are you sure you want to delete this project? All activities "
         "associated will be erased.\n");

  bool loop = true;
  while (loop) {
    printf("Y/N: ");
    char input = tolower(getc(stdin));
    flush_input_buffer();

    switch (input) {
    case 'y':
      loop = false;
      break;
    case 'n':
      return MENU_OK;
    default:
      continue;
    }
  }

  FileError error = fs_delete_project(*project);

  PROPAGATE(MenuError, reload_projects, (project_menu_data));

  return MENU_EXIT;
}

ItemStatus project_commit_status(Project *project,
                                 ProjectMenuData *project_menu_data) {
  ItemStatus status = {0};

  if (*project->name) {
    status.available = true;
  } else {
    status.available = false;
    sprintf(status.prompt, "Project requires a name before it can be saved");
  }

  return status;
}

MenuError free_project_menu(ProjectMenuData *menu_data) {
  FileError error =
      fs_free_project_list(*menu_data->projects, *menu_data->project_c);
  if (error) {
    printf("Failed to free project list (error %d)\n", error);
    return MENU_ITEM_ERROR;
  }
  *menu_data->projects = NULL;
  *menu_data->project_c = 0;

  free(*menu_data->menu_items);
  free(*menu_data->menu_item_data);

  return MENU_OK;
}

MenuError reload_projects(ProjectMenuData *menu_data) {
  if (*menu_data->projects) {
    PROPAGATE(MenuError, free_project_menu, (menu_data));
  }

  FileError error =
      fs_get_project_list(menu_data->projects, menu_data->project_c);
  if (error) {
    printf("Failed to read project list (FileError %d)\n", error);
    return MENU_ITEM_ERROR;
  }

  *menu_data->item_c = *menu_data->project_c + 1;
  *menu_data->menu_items = calloc(*menu_data->item_c, sizeof(MenuItem));
  *menu_data->menu_item_data = calloc(*menu_data->project_c, sizeof(MenuItem));

  for (int i = 0; i < *menu_data->project_c; i++) {
    Project *project = (*menu_data->projects)[i];
    MenuItem *menu_item = *menu_data->menu_items + i;
    ProjectMenuItemData *item_data = *menu_data->menu_item_data + i;

    // Build Menu Item
    menu_item->item_data = item_data;
    menu_item->function = (MenuItemFn)project_item_menu;
    menu_item->default_prompt = project->name;

    // Build Menu Item Data
    item_data->project = project;
    item_data->index = i;
  }

  MenuItem *add_project_item = *menu_data->menu_items + *menu_data->project_c;
  add_project_item->default_prompt = "New Project";
  add_project_item->function = (MenuItemFn)add_project;

  return MENU_OK;
}
