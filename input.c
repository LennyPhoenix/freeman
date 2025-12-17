#include "input.h"

#include "error.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void flush_input_buffer(void) {
  int ch;
  while ((ch = getchar()) != '\n' && ch != EOF) { /* discard */
  }
}

void wait_for_enter(void) {
  printf("Press enter to continue...");
  flush_input_buffer();
}

InputError read_string(char *buffer) {
  // Read user input
  if (!fgets(buffer, INPUT_BUFFER_SIZE, stdin)) {
    flush_input_buffer();
    return INPUT_READ_ERROR;
  }

  // Cut string at newline
  buffer[strcspn(buffer, "\n")] = '\0';

  return INPUT_OK;
}

InputError read_float(double *val_out) {
  char input[INPUT_BUFFER_SIZE];

  PROPAGATE(InputError, read_string, (input));

  if (validate_float_string(input)) {
    return INPUT_INVALID;
  }

  *val_out = atof(input);

  return INPUT_OK;
}

InputError read_duration(unsigned long *hours_out, unsigned long *minutes_out) {
  char input[INPUT_BUFFER_SIZE];
  PROPAGATE(InputError, read_string, (input));

  // If HH:MM is provided, minutes will be MM and hours will be HH, if MMM is
  // provided then hours will be MMM.
  char *second = input;
  char *first = strsep(&second, ":");

  if (!second) {
    char *minutes = first;

    // No hours only minutes
    if (validate_int_string(minutes)) {
      return INPUT_INVALID;
    }

    int mins = atoi(input);

    *hours_out = mins / 60;
    *minutes_out = mins % 60;
  } else {
    char *hours_str = first;
    char *minutes_str = second;

    if (strlen(minutes_str) > 2 || validate_int_string(hours_str) ||
        validate_int_string(minutes_str)) {
      return INPUT_INVALID;
    }

    int minutes = atoi(minutes_str);
    int hours = atoi(hours_str);
    if (minutes >= 60) {
      return INPUT_INVALID;
    }

    *hours_out = (unsigned long)hours;
    *minutes_out = (unsigned long)minutes;
  }

  return INPUT_OK;
}

InputError read_int(int *val_out) {
  char input[INPUT_BUFFER_SIZE];

  PROPAGATE(InputError, read_string, (input));

  if (validate_int_string(input)) {
    return INPUT_INVALID;
  }

  *val_out = atoi(input);

  return INPUT_OK;
}

InputError validate_float_string(const char *input) {
  if (input == NULL || *input == '\0') { // check if empty
    return INPUT_INVALID;
  }

  // Optional sign
  if (*input == '+' || *input == '-') {
    input++;
  }

  // Must have at least one digit after optional sign
  if (*input == '\0') {
    return INPUT_INVALID;
  }

  // (0-9)+(\.(0-9)*)?
  while (*input != '.') {
    if (*input == '\0') {
      return INPUT_OK;
    }

    if (*input < '0' || *input > '9') {
      return INPUT_INVALID;
    }

    input++;
  }

  input++;

  while (*input != '\0') {
    if (*input < '0' || *input > '9') {
      return INPUT_INVALID;
    }

    input++;
  }

  return INPUT_OK;
}

InputError validate_int_string(const char *input) {
  if (input == NULL || *input == '\0') {
    return INPUT_INVALID;
  }

  // Optional sign
  if (*input == '+' || *input == '-') {
    input++;
  }

  // Must have at least one digit after optional sign
  if (*input == '\0') {
    return INPUT_INVALID;
  }

  // (0-9)+
  while (*input != '\0') {
    if (*input < '0' || *input > '9') {
      return INPUT_INVALID;
    }

    input++;
  }

  return INPUT_OK;
}
