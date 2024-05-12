#ifndef _PEEK_H_
#define _PEEK_H_

void print_entry(const char *name, mode_t mode);
void print_info(const char *path, const char *name, mode_t mode);
void execute_peek(const char *path, int show_hidden, int show_info, char *home_directory, char *last_directory);

#endif
