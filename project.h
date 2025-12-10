#ifndef PROJECT_H_
#define PROJECT_H_

#include "activity.h"
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

#endif
