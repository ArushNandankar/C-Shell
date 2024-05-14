#ifndef _MAIN_H_
#define _MAIN_H_

void remove_empty_lines(char *pepath, char *home_directory);
int isSubstring(const char *str, const char *substr);
void handle_input(char *input, char *home_directory, char *current_directory, char *last_directory, char *last_directory_temp, int background, char *previous_command, char *pepath, char *input_copy);
void removeConsecutiveDuplicates(const char *pepath, const char *home_directory);
void modify_input(char *input);
void check_bg_process();

#endif
