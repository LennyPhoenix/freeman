#include "date.h"

#include <time.h>

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
    if (local_time->tm_yday == 364) {
      days = 27;
    } else {
      days = 28;
    }
    break;
  default: // Everything else is 30 days
    days = 30;
  }

  return days;
}
