#pragma once

#ifndef FILE_STACK_H
#define FILE_STACK_H

#include "stack.h"

// Funkcje do zapisu i odczytu stosu z pliku binarnego
void save_stack_to_file(Stack* stack, const char* filename, size_t data_size);
void load_stack_from_file(Stack* stack, const char* filename, size_t data_size);

#endif // FILE_STACK_H