#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char *argv[]) {
  char command[1024];
  // Flush after every printf
  setbuf(stdout, NULL);

  while (1) {
    printf("$ ");
    fgets(command, sizeof(command), stdin);

    command[strcspn(command, "\n")] = 0;

    if (strcmp(command, "exit") == 0) break;
    else {
      printf("%s: command not found\n", command);
    }
  }

  return 0;
}
