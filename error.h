#ifndef ERROR_H_
#define ERROR_H_

/// Similar to the `?` operator in Rust, simply propagates an error if not OK.
#define PROPAGATE(errtype, function, args)                                     \
  {                                                                            \
    errtype err_##function = function args;                                    \
    if (err_##function) {                                                      \
      return err_##function;                                                   \
    }                                                                          \
  }

#endif
