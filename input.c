#include "input.h"

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
    printf(": ");

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

  InputError string_error = read_string(input);
  if (string_error) {
    return string_error;
  }

  if (validate_float_string(input)) {
    return INPUT_INVALID;
  }

  *val_out = atof(input);

  return INPUT_OK;
}

InputError read_int(int *val_out) {
    char input[INPUT_BUFFER_SIZE];

    InputError string_error = read_string(input);
    if (string_error) {
        return string_error;
    }

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
