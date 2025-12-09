#ifndef INPUT_H_
#define INPUT_H_

#include <stdlib.h>

#define INPUT_BUFFER_SIZE (128)

typedef enum {
  INPUT_OK = 0,
  INPUT_INVALID = 1,
  INPUT_READ_ERROR = 2,
} InputError;

void flush_input_buffer(void);
void wait_for_enter(void);

InputError read_int(int *val_out);
InputError read_float(double *val_out);
InputError read_string(char *buffer);
InputError read_duration(int *hours_out, int *minutes_out);

InputError validate_float_string(const char *input);
InputError validate_int_string(const char *input);

#endif
