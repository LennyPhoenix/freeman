#ifndef INPUT_H_
#define INPUT_H_

#include <stdlib.h>

#define INPUT_BUFFER_SIZE (128)

typedef enum InputError {
  INPUT_OK = 0,
  /// Input failed a validity check.
  INPUT_INVALID,
  /// Input could not be read from stdin.
  INPUT_READ_ERROR,
} InputError;

/// Discards any leftover input.
void flush_input_buffer(void);
/// Waits for the enter key to be pressed by the user.
void wait_for_enter(void);

/// Reads an int from stdin.
InputError read_int(int *val_out);
/// Reads a double from stdin
InputError read_double(double *val_out);
/// Reads a string from stdin
InputError read_string(char *buffer);
/// Reads a time duration (HH:MM or MMM) from stdin
InputError read_duration(unsigned long *hours_out, unsigned long *minutes_out);

/// Checks if a string is a valid float
InputError validate_float_string(const char *input);
/// Checks if a string is a valid integer
InputError validate_int_string(const char *input);

#endif
