#ifndef ACTIVITY_H_
#define ACTIVITY_H_

#include "menu.h"

#include <stdbool.h>
#include <time.h>

/// Optional type, `value` is undefined if `present` is false.
#define Optional(T)                                                            \
  struct optional_##T {                                                        \
    bool present;                                                              \
    T value;                                                                   \
  }

/// Optional double type.
typedef Optional(double) OptionalDouble;

typedef enum ActivityError {
  ACTIVITY_OK = 0,
  /// Something went wrong trying to display this activity.
  ACTIVITY_DISPLAY_ERROR,
} ActivityError;

/// A logged work actvity.
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

/// Prints information about a passed activity.
ActivityError display_activity(Activity activity);

/// Menu for logging a new activity.
MenuError new_activity_menu(void *_menu_data, void *_item_data);
/// Status check for logging a new activity.
ItemStatus new_activity_menu_status(void *_menu_data, void *_item_data);

// New Activity Menu Items

/// Menu for setting a project ID to the activity.
MenuError set_activity_project(Activity *activity, void *_item_data);
/// Status check for project ID menu.
ItemStatus set_activity_project_status(Activity *activity, void *_item_data);

/// Menu item to assign a new description to the activity.
MenuError set_activity_description(Activity *activity, void *_item_data);
/// Activity description status check.
ItemStatus set_activity_description_status(Activity *activity,
                                           void *_item_data);

/// Menu item to assign a new duration to the activity.
MenuError set_activity_duration(Activity *activity, void *_item_data);
/// Activity duration status check.
ItemStatus set_activity_duration_status(Activity *activity, void *_item_data);

/// Menu item to assign/clear a custom rate for the activity.
MenuError set_activity_custom_rate(Activity *activity, void *_item_data);
/// Custom rate status check.
ItemStatus set_activity_custom_rate_status(Activity *activity,
                                           void *_item_data);

/// Menu item to save the activity to its associated project on the filesystem.
MenuError save_activity(Activity *activity, void *_item_data);
/// Status check for activity saving.
ItemStatus save_activity_status(Activity *activity, void *_item_data);

#endif
