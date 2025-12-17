#ifndef DATE_H_
#define DATE_H_

#include <stdbool.h>
#include <time.h>

/// Determines if the current year is a leap year.
bool is_leap_year(struct tm *tm);

/// Fetches the number of days in the current month.
unsigned int days_this_month(void);

#endif
