#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <limits.h>
#include <sys/wait.h>
#include "functions.h"

enum Command { UNKNOWN, ECHO, EXIT, TYPE, PWD, CD };

int main() {
  // Flush after every printf
  setbuf(stdout, NULL);

  // REPL
  char input[1024];
  while (1) {
    printf("$ ");
    fgets(input, sizeof(input), stdin);
    input[strcspn(input, "\n")] = 0;

    char* command_str = get_command(input);
    char** args = get_args(input);
    const enum Command command = parse_command(command_str);

    switch (command) {
      case ECHO:
        echo_command(args);
        break;
      case TYPE:
        type_command(args);
        break;
      case EXIT:
        exit_command();
        break;
      case PWD:
        pwd_command();
        break;
      case CD:
        cd_command(input);
        break;
      case UNKNOWN: {
        // if not a builtin, try seeing if the command is an executable...
        const int is_a_command = run_command(command_str, args);
        if (is_a_command != 0) {
          printf("%s: command not found\n", command_str);
          break;
        }
        break;
      }
    }

    free(args);
    free(command_str);
  }

  return 0;
}

enum Command parse_command(const char* command) {
  if (strcmp(command, "echo") == 0) {
    return ECHO;
  }
  else if (strcmp(command, "exit") == 0) {
    return EXIT;
  }
  else if (strcmp(command, "type") == 0) {
    return TYPE;
  }
  else if (strcmp(command, "pwd") == 0) {
    return PWD;
  }
  else if (strcmp(command, "cd") == 0) {
    return CD;
  }
  else {
    return UNKNOWN;
  }
}

char* get_command(const char* c) {
  const size_t len = strlen(c)+1;

  char* command = malloc(len);
  strcpy(command, c);
  strtok(command, " ");

  return command;
}

char** get_args(const char* params) {
  // find out how many arguments are in params
  char* temp = strdup(params);
  const char* delim = " ";
  const char* token = strtok(temp, delim);
  int count = 0;
  while (token != NULL) {
    count++;
    token = strtok(NULL, delim);
  }
  free(temp);

  /*
   * allocate memory for addresses to each word
   * make sure the last point is a NULL address
   * first space will be skipped because we do
   * not need the command just the args
   * also allocate all memory to NULL
   */
  char** args = malloc((count) * sizeof(char*));
  for (int i = 0; i < count; i++) args[i] = NULL;

  /*
   * assign token to each slot, starting with the
   * second token because first is the
   */
  char* copy = strdup(params);
  token = strtok(copy, delim);
  int i = 0;
  while (token != NULL) {
    token = strtok(NULL, delim);
    if (token != NULL) args[i++] = strdup(token);
  }
  free(copy);

  return args;
}

char* get_executable(const char* exe) {
  const char* path_env = getenv("PATH");
  if (path_env == NULL) return NULL;

  char* copy = strdup(path_env);
  const char* delim = ":;";
  const char* token = strtok(copy, delim);

  char full_path[PATH_MAX];
  while (token != NULL) {
    snprintf(full_path, sizeof(full_path), "%s/%s", token, exe);

    if (access(full_path, X_OK) == 0) {
      char* result = strdup(full_path);
      free(copy);
      return result;
    }

    token = strtok(NULL, delim);
  }

  return NULL;
}

int cd_command(const char* dir) {
  // if there is more than one argument print error
  char* dir_copy = strdup(dir);
  int count = 0;
  const char* delim = " ";
  const char* token = strtok(dir_copy, delim);
  char* path = NULL;

  while (token != NULL) {
    count++;
    if (count == 2) {
      path = strdup(token);
    }
    token = strtok(NULL, delim);
  }
  free(dir_copy);

  if (count != 2) {
    if (count > 2) {
      printf("cd: too many arguments\n");
    }
    free(path);
    return 1;
  }

  if (strcmp(path, "~") == 0) {
    free(path);
    return chdir(getenv("HOME"));
  }

  if (chdir(path) != 0) {
    printf("cd: %s: No such file or directory\n", path);
    free(path);
    return 1;
  }

  free(path);
  return 0;
}

int pwd_command() {
  char cwd[PATH_MAX];
  if (getcwd(cwd, sizeof(cwd)) == NULL) {
    return 1;
  }
  printf("%s\n", cwd);
  return 0;
}

int run_command(char* cmd, char** args) {
  if (cmd == NULL || args == NULL) return 1;
  char* executable = get_executable(cmd);
  if (executable == NULL) return 1;

  const pid_t pid = fork();

  if (pid == 0) {
    int count = 0;
    while (args[count] != NULL) count++;

    // allocate a chunk of memory that is size of cmd + args + null terminator
    char** argv = malloc((count + 2) * sizeof(char*));
    argv[0] = cmd;
    for (int i = 0; i < count; i++) {
      argv[i+1] = args[i];
    }
    argv[count+1] = NULL;

    execv(executable, argv);
    exit(-1);
  }
  else if (pid < 0) exit(-1);
  else wait(NULL);

  free(executable);
  return 0;
}

int type_command(char** args) {
  if (args == NULL) return 1;

  // iterate through all the arguments provided in CLI
  for (int i = 0; args[i] != NULL; i++) {
    const enum Command command = parse_command(args[i]);
    switch (command) {
      case ECHO:
      case PWD:
      case TYPE:
      case CD:
      case EXIT:
        printf("%s is a shell builtin\n", args[i]);
        continue;
      case UNKNOWN: {
        char* executable = get_executable(args[i]);
        if (executable == NULL) {
          printf("%s: not found\n", args[i]);
          break;
        }

        printf("%s is %s\n", args[i], executable);
        free(executable);
        break;
      }
    }
  }

  return 0;
}

int echo_command(char** args) {
  if (args != NULL) {
    for (int i = 0; args[i] != NULL; i++) {
      printf("%s ", args[i]);
    }
    printf("\n");
    return 0;
  }

  return 1;
}

void exit_command() {
  exit(0);
}