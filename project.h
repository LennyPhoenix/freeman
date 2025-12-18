#ifndef PROJECT_H_
#define PROJECT_H_

#include "activity.h"
#include "menu.h"

#include <stddef.h>

#define PROJECT_START_ID (1000);

/// Unique ID and filename stem for a project.
typedef unsigned long ProjectId;

/// A project, collects a group of related activities.
typedef struct Project {
  /// Unique ID of the project
  ProjectId id;

  /// Project name
  char name[64];
  /// Default hourly rate
  double default_rate;

  /// List of logged activities for this project
  Activity *activities;
  size_t activity_c;
} Project;

/// Project management menu.
MenuError projects_menu(void *_menu_data, void *_item_data);
/// Status check for project menu.
ItemStatus projects_status(void *_menu_data, void *_item_data);

/// Specific data passed to a project sub-menu.
typedef struct ProjectMenuItemData {
  /// Pointer to the project this menu item corresponds with.
  Project *project;
  /// Index (NOT ID!!!) of the project this menu item corresponds with.
  size_t index;
} ProjectMenuItemData;

/// Data passed to all sub-menu.
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

/// Frees the projects and menu items from the project menu data struct.
MenuError free_project_menu(ProjectMenuData *menu_data);
/// Loads the projects into the menu data struct.
MenuError reload_projects(ProjectMenuData *menu_data);

/// Menu for creating a new project.
MenuError add_project(ProjectMenuData *menu_data, void *_item_data);

/// Menu for managing an existing project.
MenuError project_item_menu(ProjectMenuData *menu_data,
                            ProjectMenuItemData *item_data);

/// Menu item to assign a new name to a project.
MenuError project_name(Project *project, void *_item_data);
/// Status check for `project_name` menu item
ItemStatus project_name_status(Project *project, void *_item_data);

/// Menu item to assign a new default rate to a project.
MenuError project_default_rate(Project *project, void *_item_data);
/// Status check for `project_default_rate` menu item
ItemStatus project_default_rate_status(Project *project, void *_item_data);

/// Menu item to save a project to the filesystem.
MenuError project_commit(Project *project, ProjectMenuData *project_menu_data);
/// Status check for `project_commit` menu item
ItemStatus project_commit_status(Project *project,
                                 ProjectMenuData *project_menu_data);

/// Menu item to list a project's activities.
MenuError project_list_activities(Project *project, void *_item_data);

/// Menu item to delete a project from the filesystem.
MenuError project_delete(Project *project, ProjectMenuData *project_menu_data);

#endif
