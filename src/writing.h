#ifndef WRITING_H
#define WRITING_H
#include "utils.h"
#include <stdbool.h>
#include <stdio.h>
#include <conio.h>  

extern char* text;
extern int length;
extern int cursorX;
extern int cursorY;
extern int scrollX;
extern int scrollY;
extern int selStart;
extern int selEnd;

void handleKeyW(char c);

void renderW();

void initW();

int getMaxLine();

int getTextIndex();

void goToIndex(int index);

#endif // WRITING_H