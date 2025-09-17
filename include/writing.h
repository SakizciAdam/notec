#ifndef WRITING_H
#define WRITING_H
#include "utils.h"
#include "draw.h"
#include <stdbool.h>
#include <stdio.h>







int getMaxLine();

int getTextIndex();

void goToIndex(int index);

void moveCursorLeft();

void moveCursorRight();

void moveCursorUp();

void moveCursorDown();

#endif // WRITING_H