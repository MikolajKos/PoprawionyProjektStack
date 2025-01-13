#ifndef MY_STUDENT_H
#define MY_STUDENT_H

#include "stdafx.h"
#include "cli_mess.h"
#include <string.h>
#include "stack.h"

typedef enum {
    MA,  // Matematyka
    PR,  // Programowanie
    FA,  // Fizyka
    IF   // Informatyka
} StudyField;

// Struktura studenta
typedef struct {
    char* surname;
    int birth_year;
    StudyField sfield;
} MyStudent;

// Podstawowe operacje na studencie
MyStudent* create_student(const char* surname, int birth_year, StudyField sfield);
void free_student(MyStudent* student);

// Funkcje pomocnicze
void print_study_fields();
Stack* initialize_student_stack();

// Funkcje callback dla stosu
void save_student_data(void* data, FILE* file);
void* load_student_data(FILE* file);
void print_student_data(void* data);
void free_student_data(void* data);
int compare_student_data(void* data, void* criteria);

// Struktura kryteriów wyszukiwania
typedef struct {
    char* surname;
    int birth_year;
    StudyField sfield;
} SearchCriteria;

// Funkcje dla kryteriów wyszukiwania
SearchCriteria* create_search_criteria(const char* surname, int birth_year, StudyField sfield);
void free_search_criteria(SearchCriteria* criteria);
void find_and_print_students_by_criteria(const char* filename, const char* surname, int birth_year, int sfield);

extern const char* sfields_text[];

#endif