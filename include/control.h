#ifndef CONTROL_H
#define CONTROL_H

#include "writing.h"

void set_status_text(const char* input);

void save_file();

void find_text();

void select_all();

void clear_selection();

void move_cursor_or_selection_left();

void move_cursor_or_selection_right();

void move_cursor_or_selection_up();

void move_cursor_or_selection_down();

void toggle_selection_mode();

void quit();


void goto_line();
#endif // CONTROL_H
