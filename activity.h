#ifndef ACTIVITY_H_
#define ACTIVITY_H_

#include "menu.h"

#include <stdbool.h>
#include <time.h>

#define Optional(T)                                                            \
  struct optional_##T {                                                        \
    bool present;                                                              \
    T value;                                                                   \
  }

typedef Optional(double) OptionalDouble;

typedef enum ActivityError {
    ACTIVITY_OK = 0,
    /// Something went wrong trying to display this activity.
    ACTIVITY_DISPLAY_ERROR,
} ActivityError;

typedef struct Activity {
  /// Optional description for this specific activity.
  char description[256];

  /// Hours spent on this activity.
  unsigned long hours;
  /// Minutes spent on this activity.
  unsigned long minutes;

  /// Hourly rate for this activity.
  ///
  /// Optional in unsaved activities, will default to project rate when saving.
  OptionalDouble rate;

  /// Time that this activity was logged.
  ///
  /// ulong for portability.
  unsigned long time;

  /// The ID of the project that this activity was logged to.
  unsigned long project_id;
} Activity;

ActivityError display_activity(Activity activity);

MenuError new_activity_menu(void *_menu_data, void *_item_data);
ItemStatus new_activity_menu_status(void *_menu_data, void *_item_data);

// New Activity Menu Items
MenuError set_activity_project(Activity *activity, void *_item_data);
ItemStatus set_activity_project_status(Activity *activity, void *_item_data);

MenuError set_activity_description(Activity *activity, void *_item_data);
ItemStatus set_activity_description_status(Activity *activity,
                                           void *_item_data);

MenuError set_activity_duration(Activity *activity, void *_item_data);
ItemStatus set_activity_duration_status(Activity *activity, void *_item_data);

MenuError set_activity_custom_rate(Activity *activity, void *_item_data);
ItemStatus set_activity_custom_rate_status(Activity *activity,
                                           void *_item_data);

MenuError save_activity(Activity *activity, void *_item_data);
ItemStatus save_activity_status(Activity *activity, void *_item_data);

#endif
