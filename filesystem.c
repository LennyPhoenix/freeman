#include "filesystem.h"

#include <cyaml/cyaml.h>
#include <dirent.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

#include "error.h"
#include "preferences.h"

FileError get_home_path(const char *path, char *path_out) {
  const char *home_dir = getenv("HOME");

  if (home_dir) {
    if (!sprintf(path_out, "%s/%s", home_dir, path)) {
      return FILE_PATH_ERROR;
    }
  } else {
    return FILE_HOME_ERROR;
  }

  return FILE_OK;
}

FileError fs_ensure(void) {
  char config_dir[128];
  PROPAGATE(FileError, get_home_path, (CONFIG_DIRECTORY, config_dir));
  if (access(config_dir, F_OK)) {
    printf("Intialising config directory...\n");
    if (mkdir(config_dir, DEFAULT_PERMISSIONS)) {
      return FILE_CREATE_ERROR;
    }
  }

  char preferences_file[128];
  PROPAGATE(FileError, get_home_path, (PREFERENCES_FILE, preferences_file));
  if (access(preferences_file, F_OK)) {
    printf("Initialising preferences file...\n");
    PROPAGATE(FileError, fs_init_preferences, ());
  }

  char projects_dir[128];
  PROPAGATE(FileError, get_home_path, (PROJECTS_DIRECTORY, projects_dir));
  if (access(projects_dir, F_OK)) {
    printf("Initialising projects directory...\n");
    if (mkdir(projects_dir, DEFAULT_PERMISSIONS)) {
      return FILE_CREATE_ERROR;
    }
  }

  return FILE_OK;
}

// CYAML
static const cyaml_config_t CYAML_CONFIG = {
    .log_fn = cyaml_log,
    .mem_fn = cyaml_mem,
    .log_level = CYAML_LOG_WARNING,
};

// Preferences
static const cyaml_schema_field_t PREFERENCES_MAPPING_SCHEMA[] = {
    CYAML_FIELD_FLOAT("rent", CYAML_FLAG_DEFAULT, Preferences, rent),
    CYAML_FIELD_FLOAT("living_costs", CYAML_FLAG_DEFAULT, Preferences,
                      living_costs),
    CYAML_FIELD_FLOAT("savings_goal", CYAML_FLAG_DEFAULT, Preferences,
                      savings_goal),
    CYAML_FIELD_END,
};
static const cyaml_schema_value_t PREFERENCES_SCHEMA = {
    CYAML_VALUE_MAPPING(CYAML_FLAG_POINTER, Preferences,
                        PREFERENCES_MAPPING_SCHEMA),
};

FileError fs_init_preferences(void) {
  char preferences_file[128];
  PROPAGATE(FileError, get_home_path, (PREFERENCES_FILE, preferences_file));

  static const Preferences DEFAULT_PREFERENCES = {
      .rent = 0,
      .living_costs = 0,
      .savings_goal = 0,
  };

  PROPAGATE(FileError, fs_set_preferences, (DEFAULT_PREFERENCES));

  return FILE_OK;
}

FileError fs_set_preferences(Preferences preferences) {
  char preferences_file[128];
  PROPAGATE(FileError, get_home_path, (PREFERENCES_FILE, preferences_file));

  cyaml_err_t error = cyaml_save_file(preferences_file, &CYAML_CONFIG,
                                      &PREFERENCES_SCHEMA, &preferences, 0);

  if (error) {
    return FILE_CYAML_SAVE_ERROR;
  }

  return FILE_OK;
}

FileError fs_get_preferences(Preferences *preferences_out) {
  char preferences_file[128];
  PROPAGATE(FileError, get_home_path, (PREFERENCES_FILE, preferences_file));

  Preferences *loaded_preferences;
  cyaml_err_t error =
      cyaml_load_file(preferences_file, &CYAML_CONFIG, &PREFERENCES_SCHEMA,
                      (void **)&loaded_preferences, NULL);

  if (error) {
    return FILE_CYAML_LOAD_ERROR;
  }

  memcpy(preferences_out, loaded_preferences, sizeof(Preferences));

  error = cyaml_free(&CYAML_CONFIG, &PREFERENCES_SCHEMA, loaded_preferences, 0);
  if (error) {
    return FILE_CYAML_FREE_ERROR;
  }

  return FILE_OK;
}
