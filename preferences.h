#ifndef PREFERENCES_H_
#define PREFERENCES_H_

typedef struct Preferences {
  double rent;
  double living_costs;
  double savings_goal;
} Preferences;

#include "menu.h"

ItemStatus preferences_status(void *_);
MenuError preferences_menu(void *_);

ItemStatus rent_status(void *_);
MenuError update_rent(void *_);

ItemStatus living_costs_status(void *_);
MenuError update_living_costs(void *_);

ItemStatus savings_goal_status(void *_);
MenuError update_savings_goal(void *_);

#endif
