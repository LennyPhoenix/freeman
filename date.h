#ifndef DATE_H_
#define DATE_H_

#include <stdbool.h>
#include <time.h>

/// Determines if the current year is a leap year.
bool is_leap_year(struct tm tm);

/// Determines if two timestamps are on the same day.
bool is_same_day(time_t t1, time_t t2);
/// Determines if two timestamps are on the same month.
bool is_same_month(time_t t1, time_t t2);

/// Fetches the number of days in the current month.
unsigned int days_this_month(void);

#endif
