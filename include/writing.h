#ifndef WRITING_H
#define WRITING_H
#include "utils.h"
#include "draw.h"
#include <stdbool.h>
#include <stdio.h>







int get_max_line();

int get_text_index();

void goto_index(int index);

void move_cursor_left();

void move_cursor_right();

void move_cursor_up();

void move_cursor_down();

#endif // WRITING_H