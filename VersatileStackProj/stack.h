#include "stdafx.h"
#include <string.h>

#pragma once

#ifndef STACK_H
#define STACK_H

typedef struct StackItem {
    void* data;
    struct StackItem* next;
} StackItem;

// Funkcje callback dla operacji na danych
typedef void (*SaveDataFunc)(void* data, FILE* file);
typedef void* (*LoadDataFunc)(FILE* file);
typedef void (*PrintDataFunc)(void* data);
typedef void (*FreeDataFunc)(void* data);
typedef int (*CompareDataFunc)(void* data, void* criteria);

typedef struct {
    StackItem* top;
    SaveDataFunc save_data;    // Funkcja zapisuj�ca element
    LoadDataFunc load_data;    // Funkcja odczytuj�ca element
    PrintDataFunc print_data;  // Funkcja wy�wietlaj�ca element
    FreeDataFunc free_data;    // Funkcja zwalniaj�ca pami�� elementu
    CompareDataFunc compare_data; // Funkcja por�wnuj�ca elementy
} Stack;

// Funkcje podstawowe stosu
Stack* initialize_stack(SaveDataFunc save_fn, LoadDataFunc load_fn,
    PrintDataFunc print_fn, FreeDataFunc free_fn,
    CompareDataFunc compare_fn);
void free_stack(Stack** stack);
void push(Stack* stack, void* data);
void* pop(Stack* stack);
void check_stack_integrity(Stack* stack);

//Funkcje operuj�ce na danych w stosie
void stack_save_to_file(Stack* stack, const char* filename);
void stack_load_from_file(Stack* stack, const char* filename);
void stack_print_all(Stack* stack);
void* stack_find_by_criteria(Stack* stack, void* criteria);

#endif