#ifndef BALANCE_H_
#define BALANCE_H_

#include "activity.h"
#include "menu.h"
#include "preferences.h"
#include "project.h"

#include <time.h>

typedef enum BalanceError {
  BALANCE_OK = 0,
  /// Something went wrong calculating expenses.
  BALANCE_EXPENSES_ERROR,
  /// Something went wrong calculating earnings.
  BALANCE_EARNINGS_ERROR,
} BalanceError;

/// Data passed to each balance calculation menu item.
typedef struct BalanceMenuData {
  /// Projects list (array of Project pointers)
  Project **projects;
  /// Project count
  size_t project_c;

  /// Time of menu opening
  time_t t;
} BalanceMenuData;

/// Menu for calculating the balance for various date ranges.
MenuError balance_menu(void *_menu_data, void *_item_data);

/// Menu item to show the balance for today.
MenuError daily_balance(BalanceMenuData *menu_data, void *_item_data);
/// Menu item to show the balance this month.
MenuError monthly_balance(BalanceMenuData *menu_data, bool *predict);
/// Menu item to show the balance this week.
MenuError weekly_balance(BalanceMenuData *menu_data, bool *predict);

/// Calculates the balance for a given set of days and activities, returning the
/// balance, expenses, and earnings for this period.
BalanceError calc_balance(unsigned int days, Activity **activities,
                          size_t activity_c, double *balance_out,
                          double *expenses_out, double *earnings_out);

/// Calculates the expenses for a given set of days.
BalanceError calc_expenses(unsigned int days, double *expenses_out);
/// Calculates the total earnings for a given set of activities.
BalanceError calc_earnings(Activity **activities, size_t activity_c,
                           double *earnings_out);

#endif
