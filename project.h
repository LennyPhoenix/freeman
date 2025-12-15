#ifndef PROJECT_H_
#define PROJECT_H_

#include "activity.h"
#include "menu.h"

#include <stddef.h>

typedef struct Project {
  /// Unique ID of the project
  unsigned long id;

  /// Project name
  char name[64];
  /// Default hourly rate
  double default_rate;

  /// List of logged activities for this project
  Activity *activities;
  size_t activity_c;
} Project;

MenuError projects_menu(void *_menu_data, void *_item_data);
ItemStatus projects_status(void *_menu_data, void *_item_data);

typedef struct ProjectMenuItemData {
  /// Pointer to the project this menu item corresponds with.
  Project *project;
  /// Index (NOT ID!!!) of the project this menu item corresponds with.
  size_t index;
} ProjectMenuItemData;

typedef struct ProjectMenuData {
  /// Array of project pointers.
  Project **projects;
  /// Project count.
  size_t project_c;

  /// Menu item list.
  MenuItem *menu_items;
  /// Item data list.
  ProjectMenuItemData *menu_item_data;
  /// Item count.
  size_t item_c;
} ProjectMenuData;

/// Loads the projects into the menu data struct.
MenuError free_project_menu(ProjectMenuData *menu_data);
MenuError reload_projects(ProjectMenuData *menu_data);

MenuError add_project(ProjectMenuData *menu_data, void *_item_data);

MenuError project_item_menu(ProjectMenuData *menu_data,
                            ProjectMenuItemData *item_data);

MenuError project_name(Project *project, void *_item_data);
ItemStatus project_name_status(Project *project, void *_item_data);

MenuError project_default_rate(Project *project, void *_item_data);
ItemStatus project_default_rate_status(Project *project, void *_item_data);

MenuError project_commit(Project *project, ProjectMenuData *project_menu_data);
ItemStatus project_commit_status(Project *project,
                                 ProjectMenuData *project_menu_data);

MenuError project_delete(Project *project, ProjectMenuData *project_menu_data);

#endif
