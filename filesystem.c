#include "filesystem.h"

#include "error.h"
#include "preferences.h"

#include <cyaml/cyaml.h>
#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

FileError fs_expand_from_home(const char *path, char *path_out) {
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
  Filepath config_dir;
  PROPAGATE(FileError, fs_expand_from_home, (CONFIG_DIRECTORY, config_dir));
  if (access(config_dir, F_OK)) {
    printf("Initialising config directory...\n");
    if (mkdir(config_dir, DEFAULT_PERMISSIONS)) {
      return FILE_CREATE_ERROR;
    }
  }

  Filepath preferences_file;
  PROPAGATE(FileError, fs_expand_from_home,
            (PREFERENCES_FILE, preferences_file));
  if (access(preferences_file, F_OK)) {
    printf("Initialising preferences file...\n");
    PROPAGATE(FileError, fs_init_preferences, ());
  }

  Filepath projects_dir;
  PROPAGATE(FileError, fs_expand_from_home, (PROJECTS_DIRECTORY, projects_dir));
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
#define X(symbol, _disp)                                                       \
  CYAML_FIELD_FLOAT(#symbol, CYAML_FLAG_DEFAULT, Preferences, symbol),
    PREFERENCES_TABLE
#undef X
        CYAML_FIELD_END,
};
static const cyaml_schema_value_t PREFERENCES_SCHEMA = {
    CYAML_VALUE_MAPPING(CYAML_FLAG_POINTER, Preferences,
                        PREFERENCES_MAPPING_SCHEMA),
};

FileError fs_init_preferences(void) {
  Filepath preferences_file;
  PROPAGATE(FileError, fs_expand_from_home,
            (PREFERENCES_FILE, preferences_file));

  static const Preferences DEFAULT_PREFERENCES = {
      .rent = 0,
      .living_costs = 0,
      .savings_goal = 0,
  };

  PROPAGATE(FileError, fs_set_preferences, (DEFAULT_PREFERENCES));

  return FILE_OK;
}

FileError fs_set_preferences(Preferences preferences) {
  Filepath preferences_file;
  PROPAGATE(FileError, fs_expand_from_home,
            (PREFERENCES_FILE, preferences_file));

  cyaml_err_t error = cyaml_save_file(preferences_file, &CYAML_CONFIG,
                                      &PREFERENCES_SCHEMA, &preferences, 0);

  if (error) {
    return FILE_CYAML_SAVE_ERROR;
  }

  return FILE_OK;
}

FileError fs_get_preferences(Preferences *preferences_out) {
  Filepath preferences_file;
  PROPAGATE(FileError, fs_expand_from_home,
            (PREFERENCES_FILE, preferences_file));

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

// Activity
#include "activity.h"

static const cyaml_schema_field_t OPTIONAL_DOUBLE_MAPPING_SCHEMA[] = {
    CYAML_FIELD_BOOL("present", CYAML_FLAG_DEFAULT, OptionalDouble, present),
    CYAML_FIELD_FLOAT("value", CYAML_FLAG_DEFAULT, OptionalDouble, value),
    CYAML_FIELD_END,
};
static const cyaml_schema_field_t ACTIVITY_MAPPING_SCHEMA[] = {
    CYAML_FIELD_STRING("description", CYAML_FLAG_DEFAULT, Activity, description,
                       1),
    CYAML_FIELD_UINT("hours", CYAML_FLAG_DEFAULT, Activity, hours),
    CYAML_FIELD_UINT("minutes", CYAML_FLAG_DEFAULT, Activity, minutes),
    CYAML_FIELD_MAPPING("rate", CYAML_FLAG_DEFAULT, Activity,
                        rate, OPTIONAL_DOUBLE_MAPPING_SCHEMA),
    CYAML_FIELD_UINT("time", CYAML_FLAG_DEFAULT, Activity, time),
    CYAML_FIELD_UINT("project_id", CYAML_FLAG_DEFAULT, Activity, project_id),
    CYAML_FIELD_END,
};
static const cyaml_schema_value_t ACTIVITY_VALUE_SCHEMA = {
    CYAML_VALUE_MAPPING(CYAML_FLAG_DEFAULT, Activity, ACTIVITY_MAPPING_SCHEMA),
};

// Project
#include "project.h"

static const cyaml_schema_field_t PROJECT_MAPPING_SCHEMA[] = {
    CYAML_FIELD_UINT("id", CYAML_FLAG_DEFAULT, Project, id),
    CYAML_FIELD_STRING("name", CYAML_FLAG_DEFAULT, Project, name, 1),
    CYAML_FIELD_FLOAT("default_rate", CYAML_FLAG_DEFAULT, Project,
                      default_rate),
    CYAML_FIELD_SEQUENCE_COUNT("activities", CYAML_FLAG_POINTER_NULL, Project,
                               activities, activity_c, &ACTIVITY_VALUE_SCHEMA,
                               0, CYAML_UNLIMITED),
    CYAML_FIELD_END,
};
static const cyaml_schema_value_t PROJECT_VALUE_SCHEMA = {
    CYAML_VALUE_MAPPING(CYAML_FLAG_POINTER, Project, PROJECT_MAPPING_SCHEMA),
};

FileError fs_get_project_path(unsigned long id, char *path_out) {
  PROPAGATE(FileError, fs_expand_from_home, (PROJECTS_DIRECTORY, path_out));

  if (!sprintf(path_out, "%s/%zu.yaml", path_out, id)) {
    return FILE_PATH_ERROR;
  }

  return FILE_OK;
}

FileError fs_get_project_list(Project ***projects_out, size_t *project_c_out) {
  Filepath project_dir;
  PROPAGATE(FileError, fs_expand_from_home, (PROJECTS_DIRECTORY, project_dir));

  DIR *directory = opendir(project_dir);

  if (!directory) {
    return FILE_DIRECTORY_ERROR;
  }

  Project **projects = NULL;
  size_t project_c = 0;

  struct dirent *entry;
  Filepath project_path;
  while ((entry = readdir(directory))) {
    if (entry->d_type == DT_REG) {
      sprintf(project_path, "%s/%s", project_dir, entry->d_name);

      // Resize
      project_c++;
      projects = realloc(projects, sizeof(Project *) * project_c);

      // Load Project
      cyaml_err_t error =
          cyaml_load_file(project_path, &CYAML_CONFIG, &PROJECT_VALUE_SCHEMA,
                          (void **)projects + project_c - 1, NULL);

      if (error) {
        project_c--;
        printf("Failed to load project %s (error %d)\n", project_path, error);
      }
    }
  }

  if (closedir(directory)) {
    return FILE_DIRECTORY_ERROR;
  }

  *projects_out = projects;
  *project_c_out = project_c;

  return FILE_OK;
}

FileError fs_save_project(Project project) {
  Filepath project_path;
  PROPAGATE(FileError, fs_get_project_path, (project.id, project_path));

  cyaml_err_t error = cyaml_save_file(project_path, &CYAML_CONFIG,
                                      &PROJECT_VALUE_SCHEMA, &project, 0);

  if (error) {
    return FILE_CYAML_SAVE_ERROR;
  }

  return FILE_OK;
}

FileError fs_load_project(unsigned long id, Project **project_out) {
  Filepath project_path;
  PROPAGATE(FileError, fs_get_project_path, (id, project_path));

  cyaml_err_t error =
      cyaml_load_file(project_path, &CYAML_CONFIG, &PROJECT_VALUE_SCHEMA,
                      (void **)project_out, 0);

  if (error) {
    return FILE_CYAML_LOAD_ERROR;
  }

  return FILE_OK;
}

FileError fs_free_project(Project *project) {
  cyaml_err_t error =
      cyaml_free(&CYAML_CONFIG, &PROJECT_VALUE_SCHEMA, project, 0);

  if (error) {
    printf("Error while freeing project (error %d)\n", error);
    return FILE_CYAML_FREE_ERROR;
  }

  return FILE_OK;
}

FileError fs_free_project_list(Project **projects, size_t project_c) {
  for (int i = 0; i < project_c; i++) {
    Project *project = projects[i];
    cyaml_err_t error =
        cyaml_free(&CYAML_CONFIG, &PROJECT_VALUE_SCHEMA, project, 0);

    if (error) {
      printf("Error while freeing project %d of %zu...\n", i + 1,
             project_c + 1);
      return FILE_CYAML_FREE_ERROR;
    }
  }

  free(projects);

  return FILE_OK;
}

FileError fs_delete_project(Project project) {
  Filepath project_path;
  PROPAGATE(FileError, fs_get_project_path, (project.id, project_path));

  int error = remove(project_path);
  if (error) {
    printf("Failed to delete project (error %d)\n", error);
    return FILE_DELETE_ERROR;
  }

  return FILE_OK;
}
