#ifndef WRITING_H
#define WRITING_H
#include "utils.h"
#include <stdbool.h>
#include <stdio.h>
#include <conio.h>  

char* text;
int length;
int cursorX;
int cursorY;
int scrollX;
int scrollY;

void handleKeyW(char c);

void renderW();

void initW();

int getMaxLine();

int getTextIndex();

void goToIndex(int index);

#endif // WRITING_H