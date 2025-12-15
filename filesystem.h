#ifndef FILESYSTEM_H_
#define FILESYSTEM_H_

#include <stdlib.h>

#define CONFIG_DIRECTORY ".config/freeman"
#define PREFERENCES_FILE CONFIG_DIRECTORY "/preferences.yaml"
#define PROJECTS_DIRECTORY CONFIG_DIRECTORY "/projects"
#define DEFAULT_PERMISSIONS 0755

typedef char Filepath[1024];

typedef enum FileError {
  FILE_OK = 0,
  /// Something went wrong trying to create a file or directory.
  FILE_CREATE_ERROR,
  /// Something went wrong trying to delete a file or directory.
  FILE_DELETE_ERROR,
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
  /// Error browsing a directory.
  FILE_DIRECTORY_ERROR,
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

#include "project.h"

/// Write a new project file.
FileError fs_get_project_path(unsigned long id, char *path_out);
/// Browses the projects directory and returns an (owned) list of all loaded
/// projects.
FileError fs_get_project_list(Project ***projects_out, size_t *project_c_out);
/// Saves a project to the projects directory.
FileError fs_save_project(Project project);
/// Deletes a project.
FileError fs_delete_project(Project project);
/// Loads a specific project from the projects directory.
FileError fs_load_project(unsigned long id, Project *project_out);
/// Frees a project list.
FileError fs_free_project_list(Project **projects, size_t project_c);

#endif
