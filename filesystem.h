#ifndef FILESYSTEM_H_
#define FILESYSTEM_H_

#include <stdlib.h>

#define CONFIG_DIRECTORY ".config/freeman"
#define PREFERENCES_FILE CONFIG_DIRECTORY "/preferences.yaml"
#define PROJECTS_DIRECTORY CONFIG_DIRECTORY "/projects"
#define DEFAULT_PERMISSIONS 0755

typedef enum FileError {
  FILE_OK = 0,
  /// Something went wrong trying to create a file or directory.
  FILE_CREATE_ERROR,
  /// Something went wrong saving a YAML file.
  FILE_CYAML_SAVE_ERROR,
  /// Something went wrong loading a YAML file.
  FILE_CYAML_LOAD_ERROR,
  /// Something went wrong freeing cyaml allocated data.
  FILE_CYAML_FREE_ERROR,
  /// The home directory could not be found.
  FILE_HOME_ERROR,
  /// Error while concatinating full path.
  FILE_PATH_ERROR,
} FileError;

/// Ensures that the filesystem is readable, initialising it if not.
FileError fs_ensure(void);
/// Expands a path relative to the home directory.
FileError fs_expand_from_home(const char *path, char *path_out);

#include "preferences.h"

/// Initialises the preferences file.
FileError fs_init_preferences(void);
/// Reads and deserialises the preferences file.
FileError fs_get_preferences(Preferences *preferences_out);
/// Serialises preferences and writes to the preferences file.
FileError fs_set_preferences(Preferences preferences);

#endif
