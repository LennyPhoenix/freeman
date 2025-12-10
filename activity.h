#ifndef ACTIVITY_H_
#define ACTIVITY_H_

#include <time.h>

#define Optional(T)                                                            \
  struct optional_##T {                                                        \
    bool present;                                                              \
    T value;                                                                   \
  }

typedef Optional(double) OptionalDouble;

typedef struct Activity {
  /// Optional description for this specific activity.
  char description[256];

  /// Hours spent on this activity.
  unsigned int hours;
  /// Minutes spent on this activity.
  unsigned int minutes;

  /// Custom hourly rate.
  OptionalDouble custom_rate;

  /// Time that this activity was logged.
  time_t time;
} Activity;

#endif
