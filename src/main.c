#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "functions.h"

int main() {
  // Flush after every printf
  setbuf(stdout, NULL);

  // REPL
  char input[1024];
  while (1) {
    printf("$ ");
    fgets(input, sizeof(input), stdin);
    input[strcspn(input, "\n")] = 0;

    char* command = get_command(input);
    char** args = get_args(input);

    // If the token is...
    // exit:
    if (strcmp(command, "echo") == 0) {
      echo_command(args);
      free(args);
      free(command);
    }
    else if (strcmp(command, "exit") == 0) {
      free(args);
      free(command);
      break;
    }
    else {
      printf("%s: command not found\n", command);
      free(args);
      free(command);
    }
  }

  return 0;
}

char* get_command(char* c) {
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

  /*
   * allocate memory for addresses to each word
   * make sure the last point is a NULL address
   * first space will be skipped because we do
   * not need the command just the args
   */
  char** args = malloc((count) * sizeof(char*));

  /*
   * assign token to each slot, starting with the
   * second token because first is the
   */
  char* copy = strdup(params);
  token = strtok(copy, " ");
  int i = 0;
  while (token != NULL) {
    token = strtok(NULL, " ");
    args[i++] = strdup(token);
  }
  args[i] = NULL;
  free(copy);

  return args;
}

void echo_command(char** args) {
  if (args != NULL) {
    // Iterate until we find the NULL pointer we added at the end
    for (int i = 0; args[i] != NULL; i++) {
      printf("Argument %d: %s\n", i, args[i]);
    }
  }
  printf("\n");
}