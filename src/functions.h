//
// Created by Ammie on 1/30/2026.
//

#ifndef CODECRAFTERS_SHELL_FUNCTIONS_H
#define CODECRAFTERS_SHELL_FUNCTIONS_H

enum Command parse_command(const char* command);
char* get_command(const char* c);
char** get_args(const char* params);

int echo_command(char** args);
int type_command(const char* command_str, char** args);
void exit_command();

#endif //CODECRAFTERS_SHELL_FUNCTIONS_H