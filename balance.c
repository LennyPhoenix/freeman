#include "balance.h"

#include "activity.h"
#include "date.h"
#include "error.h"
#include "filesystem.h"
#include "input.h"
#include "menu.h"

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

MenuError balance_menu(void *_menu_data, void *_item_data) {
  BalanceMenuData data;
  FileError error = fs_get_project_list(&data.projects, &data.project_c);
  if (error) {
    printf("Failed to get project list (error %d)\n", error);
    return MENU_ITEM_ERROR;
  }
  data.t = time(NULL);

  bool no_predict = false;
  bool predict = true;

  MenuItem daily = {
      .function = (MenuItemFn)daily_balance,
      .default_prompt = "Balance today",
      .status_check = NULL,
      .item_data = NULL,
  };
  MenuItem week_so_far = {
      .function = (MenuItemFn)weekly_balance,
      .default_prompt = "Balance so far this week",
      .status_check = NULL,
      .item_data = &no_predict,
  };
  MenuItem whole_week = {
      .function = (MenuItemFn)weekly_balance,
      .default_prompt = "Balance for whole week",
      .status_check = NULL,
      .item_data = &predict,
  };
  MenuItem month_so_far = {
      .function = (MenuItemFn)monthly_balance,
      .default_prompt = "Balance so far this month",
      .status_check = NULL,
      .item_data = &no_predict,
  };
  MenuItem whole_month = {
      .function = (MenuItemFn)monthly_balance,
      .default_prompt = "Balance for whole month",
      .status_check = NULL,
      .item_data = &predict,
  };

  MenuItem items[] = {daily, week_so_far, whole_week, month_so_far,
                      whole_month};
  size_t item_c = sizeof(items) / sizeof(MenuItem);
  MenuItem *items_pointer = items;

  Menu menu = {.menu_data = &data,
               .items = &items_pointer,
               .item_c = &item_c,
               .title = "Calculate..."};

  PROPAGATE(MenuError, open_menu, (&menu));

  error = fs_free_project_list(data.projects, data.project_c);
  if (error) {
    printf("Failed to free project list (error %d)", error);
    return MENU_ITEM_ERROR;
  }

  return MENU_OK;
}

MenuError daily_balance(BalanceMenuData *menu_data, void *_item_data) {
  printf("\nActivities today:\n");
  size_t filtered_activity_c = 0;
  Activity **filtered_activities = NULL;
  for (int project_index = 0; project_index < menu_data->project_c;
       project_index++) {
    Project *project = menu_data->projects[project_index];
    for (int activity_index = 0; activity_index < project->activity_c;
         activity_index++) {
      Activity *activity = project->activities + activity_index;

      time_t activity_time = (time_t)activity->time;

      if (is_same_day(menu_data->t, activity_time)) {
        display_activity(*activity);
        filtered_activity_c++;
        filtered_activities = realloc(filtered_activities,
                                      sizeof(Activity *) * filtered_activity_c);

        filtered_activities[filtered_activity_c - 1] = activity;
      }
    }
  }

  double balance, expenses, earnings;
  BalanceError error = calc_balance(1, filtered_activities, filtered_activity_c,
                                    &balance, &expenses, &earnings);
  if (error) {
    printf("Failed to calculate balance (error %d)\n", error);
    return MENU_ITEM_ERROR;
  }

  struct tm tm;
  localtime_r(&menu_data->t, &tm);

  printf("\nCalculated for %.4d/%.2d/%.2d:\n", tm.tm_year + 1900, tm.tm_mon + 1,
         tm.tm_mday);
  printf("Earnings: +£%.2f\n", earnings);
  printf("Expenses: -£%.2f\n", expenses);
  if (balance >= 0) {
    printf("Balance: +£%.2f\n", balance);
  } else {
    printf("Balance: -£%.2f\n", -balance);
  }

  free(filtered_activities);

  wait_for_enter();

  return MENU_OK;
}

MenuError weekly_balance(BalanceMenuData *menu_data, bool *predict) {
  struct tm week_start_tm;
  localtime_r(&menu_data->t, &week_start_tm);

  // Get to midnight
  week_start_tm.tm_hour = 0;
  week_start_tm.tm_min = 0;
  week_start_tm.tm_sec = 0;

  // Offset to monday
  week_start_tm.tm_mday -= week_start_tm.tm_wday - 1;
  time_t week_start = mktime(&week_start_tm);
  time_t week_end = week_start + 7 * 24 * 60 * 60;

  printf("\nActivities this week:\n");
  size_t filtered_activity_c = 0;
  Activity **filtered_activities = NULL;
  for (int project_index = 0; project_index < menu_data->project_c;
       project_index++) {
    Project *project = menu_data->projects[project_index];
    for (int activity_index = 0; activity_index < project->activity_c;
         activity_index++) {
      Activity *activity = project->activities + activity_index;

      time_t activity_time = (time_t)activity->time;

      if (activity_time >= week_start && activity_time < week_end) {
        display_activity(*activity);
        filtered_activity_c++;
        filtered_activities = realloc(filtered_activities,
                                      sizeof(Activity *) * filtered_activity_c);

        filtered_activities[filtered_activity_c - 1] = activity;
      }
    }
  }

  struct tm week_end_tm, current_tm;
  localtime_r(&week_end, &week_end_tm);
  localtime_r(&menu_data->t, &current_tm);

  unsigned int days;
  if (*predict) {
    days = 7;
  } else {
    days = current_tm.tm_wday + 1;
  }

  double balance, expenses, earnings;
  BalanceError error =
      calc_balance(days, filtered_activities, filtered_activity_c, &balance,
                   &expenses, &earnings);
  if (error) {
    printf("Failed to calculate balance (error %d)\n", error);
    return MENU_ITEM_ERROR;
  }

  week_end_tm.tm_mday -= 1;
  mktime(&week_end_tm);

  struct tm end_tm;
  if (*predict) {
    end_tm = week_end_tm;
  } else {
    end_tm = current_tm;
  }

  printf("\nCalculated for %.4d/%.2d/%.2d-%.4d/%.2d/%.2d:\n",
         week_start_tm.tm_year + 1900, week_start_tm.tm_mon + 1,
         week_start_tm.tm_mday, end_tm.tm_year + 1900, end_tm.tm_mon + 1,
         end_tm.tm_mday);
  printf("Earnings: +£%.2f\n", earnings);
  printf("Expenses: -£%.2f\n", expenses);
  if (balance >= 0) {
    printf("Balance: +£%.2f\n", balance);
  } else {
    printf("Balance: -£%.2f\n", -balance);
  }

  free(filtered_activities);

  wait_for_enter();

  return MENU_OK;
}

MenuError monthly_balance(BalanceMenuData *menu_data, bool *predict) {
  printf("\nActivities this month:\n");
  size_t filtered_activity_c = 0;
  Activity **filtered_activities = NULL;
  for (int project_index = 0; project_index < menu_data->project_c;
       project_index++) {
    Project *project = menu_data->projects[project_index];
    for (int activity_index = 0; activity_index < project->activity_c;
         activity_index++) {
      Activity *activity = project->activities + activity_index;

      time_t activity_time = (time_t)activity->time;

      if (is_same_month(menu_data->t, activity_time)) {
        display_activity(*activity);
        filtered_activity_c++;
        filtered_activities = realloc(filtered_activities,
                                      sizeof(Activity *) * filtered_activity_c);

        filtered_activities[filtered_activity_c - 1] = activity;
      }
    }
  }

  struct tm current_time;
  localtime_r(&menu_data->t, &current_time);

  unsigned int days;
  if (*predict) {
    days = days_this_month();
  } else {
    days = current_time.tm_mday;
  }

  double balance, expenses, earnings;
  BalanceError error =
      calc_balance(days, filtered_activities, filtered_activity_c, &balance,
                   &expenses, &earnings);
  if (error) {
    printf("Failed to calculate balance (error %d)\n", error);
    return MENU_ITEM_ERROR;
  }

  int year = current_time.tm_year + 1900;
  int month = current_time.tm_mon + 1;
  printf("\nCalculated for %.4d/%.2d/01-%.4d/%.2d/%.2d:\n", year, month, year,
         month, days);
  printf("Earnings: +£%.2f\n", earnings);
  printf("Expenses: -£%.2f\n", expenses);
  if (balance >= 0) {
    printf("Balance: +£%.2f\n", balance);
  } else {
    printf("Balance: -£%.2f\n", -balance);
  }

  free(filtered_activities);

  wait_for_enter();

  return MENU_OK;
}

BalanceError calc_balance(unsigned int days, Activity **activities,
                          size_t activity_c, double *balance_out,
                          double *expenses_out, double *earnings_out) {
  PROPAGATE(BalanceError, calc_expenses, (days, expenses_out));
  PROPAGATE(BalanceError, calc_earnings,
            (activities, activity_c, earnings_out));

  *balance_out = *earnings_out - *expenses_out;

  return BALANCE_OK;
}

BalanceError calc_expenses(unsigned int days, double *expenses_out) {
  Preferences preferences;
  FileError error = fs_get_preferences(&preferences);
  if (error) {
    printf("Failed to load preferences (error %d)\n", error);
    return BALANCE_EXPENSES_ERROR;
  }

  *expenses_out =
      (preferences.living_costs + preferences.rent + preferences.savings_goal) *
      days;

  return BALANCE_OK;
}

BalanceError calc_earnings(Activity **activities, size_t activity_c,
                           double *earnings_out) {
  double earnings = 0;
  for (int i = 0; i < activity_c; i++) {
    Activity *activity = activities[i];

    double duration = ((double)activity->minutes / 60.0) + activity->hours;

    if (activity->rate.present) {
      earnings += activity->rate.value * duration;
    } else {
      Project *project;
      FileError error = fs_load_project(activity->project_id, &project);
      if (error) {
        printf("Failed to load project with ID %zu (error %d)\n",
               activity->project_id, error);
        return BALANCE_EARNINGS_ERROR;
      }

      earnings += project->default_rate * duration;

      error = fs_free_project(project);
      if (error) {
        printf("Failed to free project (error %d)\n", error);
        return BALANCE_EARNINGS_ERROR;
      }
    }
  }

  *earnings_out = earnings;

  return BALANCE_OK;
}
