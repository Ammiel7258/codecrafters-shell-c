#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char *argv[]) {
  char command[1024];
  // Flush after every printf
  setbuf(stdout, NULL);

  printf("$ ");
  fgets(command, sizeof(command), stdin);
  command[strcspn(command, "\n")] = 0;
  printf("%s: command not found", command);

  return 0;
}
