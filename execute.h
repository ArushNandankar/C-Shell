#ifndef __EXECUTE_H
#define __EXECUTE_H

void execute_command(char *command, int background, char *home_directory, char *current_directory, char *last_directory, char *last_directory_temp, char *previous_command, char *pepath, char *input_copy, int *time);

#endif