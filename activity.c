#include "activity.h"

#include "error.h"
#include "filesystem.h"
#include "input.h"
#include "menu.h"
#include "project.h"

#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

ActivityError display_activity(Activity activity) {
  Project *project;
  FileError error = fs_load_project(activity.project_id, &project);
  if (error) {
    printf("Failed to load project with ID %zu (error %d)\n",
           activity.project_id, error);
    return ACTIVITY_DISPLAY_ERROR;
  }

  time_t log_time = (time_t)activity.time;
  struct tm *current_time = localtime(&log_time);

  double duration = ((double)activity.minutes / 60.0) + activity.hours;

  double rate;
  if (activity.rate.present) {
    rate = activity.rate.value;
  } else {
    rate = project->default_rate;
  }

  double earnings = rate * duration;

  printf(
      "%.4d-%.2d-%.2d %.2d:%.2d | Duration: %.2zu:%.2zu | Rate: £%.2f/hour | "
      "Earnings: £%.2f | Project: %s | %s\n",
      current_time->tm_year + 1900, current_time->tm_mon + 1,
      current_time->tm_mday, current_time->tm_hour, current_time->tm_min,
      activity.hours, activity.minutes, rate, earnings, project->name,
      activity.description);

  error = fs_free_project(project);
  if (error) {
    printf("Failed to free project (error %d)\n", error);
    return ACTIVITY_DISPLAY_ERROR;
  }

  return ACTIVITY_OK;
}

MenuError new_activity_menu(void *_menu_data, void *_item_data) {
  Activity activity = {0};

  MenuItem set_project_item = {
      .function = (MenuItemFn)set_activity_project,
      .status_check = (StatusCheckFn)set_activity_project_status,
      .default_prompt = "Select Project",
      .item_data = NULL,
  };
  MenuItem set_description_item = {
      .function = (MenuItemFn)set_activity_description,
      .status_check = (StatusCheckFn)set_activity_description_status,
      .default_prompt = "Set Description",
      .item_data = NULL,
  };
  MenuItem set_duration_item = {
      .function = (MenuItemFn)set_activity_duration,
      .status_check = (StatusCheckFn)set_activity_duration_status,
      .default_prompt = "Set Duration",
      .item_data = NULL,
  };
  MenuItem set_rate_item = {
      .function = (MenuItemFn)set_activity_custom_rate,
      .status_check = (StatusCheckFn)set_activity_custom_rate_status,
      .default_prompt = "Set Custom Rate?",
      .item_data = NULL,
  };
  MenuItem save_activity_item = {
      .function = (MenuItemFn)save_activity,
      .status_check = (StatusCheckFn)save_activity_status,
      .default_prompt = "Save Activity",
      .item_data = NULL,
  };

  MenuItem items[] = {set_project_item, set_description_item, set_duration_item,
                      set_rate_item, save_activity_item};
  size_t item_c = sizeof(items) / sizeof(MenuItem);
  MenuItem *items_pointer = items;

  Menu menu = {
      .menu_data = &activity,
      .item_c = &item_c,
      .items = &items_pointer,
      .title = "Log Activity",
  };

  PROPAGATE(MenuError, open_menu, (&menu));

  return MENU_OK;
}

ItemStatus new_activity_menu_status(void *_menu_data, void *_item_data) {
  ItemStatus status = {0};

  Project **projects;
  size_t project_c;

  FileError error = fs_get_project_list(&projects, &project_c);
  if (error) {
    sprintf(status.prompt, "Failed to load project list (error %d)", error);
    status.available = false;
    return status;
  }

  // Allow if project count > 0
  status.available = project_c;
  if (!project_c) {
    sprintf(status.prompt,
            "Must create a project before logging activities, see above!");
  }

  error = fs_free_project_list(projects, project_c);
  if (error) {
    sprintf(status.prompt, "Failed to free project list (error %d)", error);
    status.available = false;
  }

  return status;
}

// Menu item function for [`set_activity_project`]
MenuError assign_activity_project_id(Activity *activity, Project *project) {
  activity->project_id = project->id;
  return MENU_EXIT;
}

MenuError set_activity_project(Activity *activity, void *_item_data) {
  Project **projects;
  size_t project_c;
  FileError error = fs_get_project_list(&projects, &project_c);
  if (error) {
    printf("Failed to get project list (error %d)\n", error);
    return MENU_ITEM_ERROR;
  }

  MenuItem *items = calloc(project_c, sizeof(MenuItem));

  for (int i = 0; i < project_c; i++) {
    MenuItem *item = items + i;
    Project *project = projects[i];

    item->function = (MenuItemFn)assign_activity_project_id;
    item->item_data = project;
    item->default_prompt = project->name;
  }

  Menu menu = {
      .title = "Select Project for Activity",
      .item_c = &project_c,
      .items = &items,
      .menu_data = activity,
  };

  PROPAGATE(MenuError, open_menu, (&menu));

  error = fs_free_project_list(projects, project_c);
  if (error) {
    printf("Failed to free project list (error %d)\n", error);
    return MENU_ITEM_ERROR;
  }

  return MENU_OK;
};

ItemStatus set_activity_project_status(Activity *activity, void *_item_data) {
  ItemStatus status = {
      .available = true,
      .prompt = {0},
  };

  if (activity->project_id) {
    Project *project;
    FileError error = fs_load_project(activity->project_id, &project);
    if (error) {
      sprintf(status.prompt, "Project ID invalid, please reassign!");
      return status;
    }

    sprintf(status.prompt, "Update Project (%s)", project->name);

    error = fs_free_project(project);
    if (error) {
      sprintf(status.prompt, "Failed to free project\n");
      status.available = false;
      return status;
    }
  }

  return status;
}

MenuError set_activity_description(Activity *activity, void *_item_data) {
  printf("Enter activity description\n: ");

  while (read_string(activity->description)) {
    printf("Invalid input\n: ");
  }

  return MENU_OK;
}

ItemStatus set_activity_description_status(Activity *activity,
                                           void *_item_data) {
  ItemStatus status = {
      .available = true,
      .prompt = "Set Description",
  };

  if (*activity->description) {
    sprintf(status.prompt, "Update Description (%s)", activity->description);
  }

  return status;
}

MenuError set_activity_duration(Activity *activity, void *_item_data) {
  printf("Enter activity duration (hours:minutes / minutes)\n: ");

  while (read_duration(&activity->hours, &activity->minutes)) {
    printf("Invalid input\n: ");
  }

  return MENU_OK;
}

ItemStatus set_activity_duration_status(Activity *activity, void *_item_data) {
  ItemStatus status = {
      .available = true,
      .prompt = "Set Duration",
  };

  if (activity->hours || activity->minutes) {
    sprintf(status.prompt, "Update Duration (%zu:%.2zu)", activity->hours,
            activity->minutes);
  }

  return status;
}

MenuError set_activity_custom_rate(Activity *activity, void *_item_data) {
  printf(
      "Enter a custom rate (£/hour) for this activity, or [r]eset to project "
      "default\n: ");

  char c = getc(stdin);
  if (tolower(c) == 'r') {
    flush_input_buffer();
    memset(&activity->rate, 0, sizeof(OptionalDouble));
    printf("Clearing custom rate for this activity...\n");
    return MENU_OK;
  } else {
    ungetc(c, stdin);
  }

  if (read_float(&activity->rate.value)) {
    printf("Invalid input\n: ");
  }

  activity->rate.present = true;

  return MENU_OK;
}

ItemStatus set_activity_custom_rate_status(Activity *activity,
                                           void *_item_data) {
  ItemStatus status = {
      .available = true,
      .prompt = "Set custom rate?",
  };

  if (activity->rate.present) {
    sprintf(status.prompt, "Update/clear custom rate (£%.2f/hour)",
            activity->rate.value);
  } else if (activity->project_id) {
    Project *project;
    FileError error = fs_load_project(activity->project_id, &project);
    if (error) {
      sprintf(status.prompt, "Failed to load project with ID %zu (error %d)",
              activity->project_id, error);
      return status;
    }

    sprintf(status.prompt, "Set custom rate? (Project default: £%.2f/hour)",
            project->default_rate);
  }

  return status;
}

MenuError save_activity(Activity *activity, void *_item_data) {
  Project *project;
  FileError error = fs_load_project(activity->project_id, &project);
  if (error) {
    printf("Failed to load project with ID %zu (error %d)\n",
           activity->project_id, error);
    return MENU_ITEM_ERROR;
  }

  if (!activity->rate.present) {
    activity->rate.value = project->default_rate;
    activity->rate.present = true;
  }
  activity->time = (unsigned long)time(NULL);

  project->activity_c++;
  project->activities =
      realloc(project->activities, sizeof(Activity) * project->activity_c);
  memcpy(project->activities + project->activity_c - 1, activity,
         sizeof(Activity));

  error = fs_save_project(*project);
  if (error) {
    printf("Failed to save project %s, ID %zu (error %d)\n", project->name,
           project->id, error);

    fs_free_project(project);
    return MENU_ITEM_ERROR;
  }

  error = fs_free_project(project);
  if (error) {
    printf("Failed to free project (error %d)\n", error);
    return MENU_ITEM_ERROR;
  }

  return MENU_EXIT;
}

ItemStatus save_activity_status(Activity *activity, void *_item_data) {
  ItemStatus status = {.available = true, .prompt = {0}};

  if (!activity->project_id) {
    sprintf(status.prompt, "Must set project before saving, see above!");
    status.available = false;
  } else if (!(activity->minutes || activity->hours)) {
    sprintf(status.prompt, "Must set duration before saving, see above!");
    status.available = false;
  } else if (!*activity->description) {
    sprintf(status.prompt, "Must set description before saving, see above!");
    status.available = false;
  } else {
    double duration = ((double)activity->minutes / 60.0) + activity->hours;
    double earnings;
    if (activity->rate.present) {
      earnings = activity->rate.value * duration;
    } else {
      Project *project;
      FileError error = fs_load_project(activity->project_id, &project);
      if (error) {
        sprintf(status.prompt, "Failed to load project with ID %zu (error %d)",
                activity->project_id, error);
        status.available = false;
        return status;
      }

      earnings = project->default_rate * duration;

      error = fs_free_project(project);
      if (error) {
        sprintf(status.prompt, "Failed to free project");
        status.available = false;
        return status;
      }
    }

    sprintf(status.prompt, "Save Activity (Earnings: £%.2f)", earnings);
  }

  return status;
}
