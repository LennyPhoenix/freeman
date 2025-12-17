#include "date.h"

#include <stdbool.h>
#include <time.h>

bool is_leap_year(struct tm *tm) {
  struct tm t = {0};
  t.tm_year = tm->tm_year;
  t.tm_mon = 11; // December
  t.tm_mday = 31;
  mktime(&t);

  return t.tm_yday == 365; // 365 means 366 days total (0-365)
}

unsigned int days_this_month(void) {
  time_t current_time = time(NULL);
  struct tm *local_time = localtime(&current_time);

  unsigned int days;
  switch (local_time->tm_mon) {
  case 0:  // Jan
  case 2:  // March
  case 4:  // May
  case 6:  // July
  case 7:  // August
  case 9:  // October
  case 11: // December
    days = 31;
    break;
  case 1:
    // February, need to check for leap year
    if (is_leap_year(local_time)) {
      days = 29;
    } else {
      days = 28;
    }
    break;
  default: // Everything else is 30 days
    days = 30;
  }

  return days;
}
