#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "functions.h"

enum Command { UNKNOWN, ECHO, EXIT, TYPE };

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
      case UNKNOWN:
        printf("%s: command not found\n", command_str);
        break;
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
  const char* token = strtok(temp, " ");
  int count = 0;
  while (token != NULL) {
    count++;
    token = strtok(NULL, " ");
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
  token = strtok(copy, " ");
  int i = 0;
  while (token != NULL) {
    token = strtok(NULL, " ");
    if (token != NULL) args[i++] = strdup(token);
  }
  free(copy);

  return args;
}

char** get_paths() {
  const char* path = getenv("PATH");
  const char* delim = ":;";
  char* temp = strdup(path);
  const char* token = strtok(temp, delim);

  int count = 0;
  while (token != NULL) {
    count++;
    token = strtok(NULL, delim);
  }
  free(temp);

  char** paths = malloc((count) * sizeof(char*));
  // for (int i = 0; i < count; i++) paths[i] = NULL;

  char* copy = strdup(path);
  int i = 0;
  token = strtok(copy, delim);
  while (token != NULL) {
    token = strtok(NULL, delim);
    if (token != NULL) paths[i++] = strdup(token);
  }
  paths[i] = NULL;
  free(copy);

  return paths;
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

int type_command(char** args) {
  if (args == NULL) return -1;

  for (int i = 0; args[i] != NULL; i++) {
    const enum Command command = parse_command(args[i]);
    switch (command) {
      case ECHO:
      case TYPE:
      case EXIT:
        printf("%s is a shell builtin\n", args[i]);
        continue;
      case UNKNOWN: {
        int found = 0;
        char** paths = get_paths();
        for (int j = 0; paths[j] != NULL; j++) {
          if (strcmp(paths[j], args[i]) == 0) {
            found++;
          }
          printf("%s\n", paths[j]);
        }
        free(paths);
        if (found == 0) printf("%s: not found\n", args[i]);
      }
      default:
        return 1;
    }
  }

  return 0;
}