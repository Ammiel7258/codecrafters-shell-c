//
// Created by Ammie on 1/30/2026.
//

#ifndef CODECRAFTERS_SHELL_FUNCTIONS_H
#define CODECRAFTERS_SHELL_FUNCTIONS_H

char* get_command(char* c);
char** get_args(const char* params);
int process_c(char* c);

void echo_command(char** args);

#endif //CODECRAFTERS_SHELL_FUNCTIONS_H