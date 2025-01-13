#include "stdafx.h"
#include "MY_STUDENT.h"
#include "cli_mess.h"
#include "stack.h"

//SF W zadaniu bylo zrealizowac zapis-odczyt, wydruk i poszukiwanie danych w obsludze stosu, 
//jednak Pan przelozyl to na obsluge interfejsu
//Tym samym unknal Pan otdzielenia obslugi KONTENERA od obslugi danych - jednego z podstawowych momentow zadania projektowego.
//Prosze zrealizowac te czynnosci w obsludze stosu.

const char* sfields_text[] = { "Matematyka", "Programowanie", "Filologia Angielska", "Informatyka"};

// Funkcje pomocnicze (static)
static int is_valid_study_field(StudyField field) {
    return field >= MA && field <= IF;
}

static int matches_criteria(MyStudent* student, SearchCriteria* criteria) {
    if (criteria->surname && strcmp(student->surname, criteria->surname) != 0) {
        return 0;
    }
    if (criteria->birth_year != -1 && student->birth_year != criteria->birth_year) {
        return 0;
    }
    if (criteria->sfield != -1 && student->sfield != criteria->sfield) {
        return 0;
    }
    return 1;
}

void find_and_print_students_by_criteria(const char* filename, const char* surname, int birth_year, int sfield) {
    FILE* file;
    if (fopen_s(&file, filename, "rb") != 0) {
        throw_cli_mess(CLI_MESS_FILE_ERROR);
        return;
    }

    SearchCriteria* criteria = create_search_criteria(surname, birth_year, sfield);
    if (!criteria) {
        fclose(file);
        return;
    }

    int found = 0;
    MyStudent* student;

    while (!feof(file)) {
        student = (MyStudent*)load_student_data(file);
        if (!student) break;

        if (matches_criteria(student, criteria)) {
            print_student_data(student);
            found++;
        }

        free_student(student);
    }

    fclose(file);
    free_search_criteria(criteria);

    if (found == 0) {
        printf("Nie znaleziono studentow spelniajacych podane kryteria.\n");
    }
    else {
        printf("Znaleziono %d studentow spelniajacych kryteria.\n", found);
    }
}

/**
 * @brief Tworzy nowego studenta.
 *
 * @param surname Nazwisko studenta.
 * @param birth_year Rok urodzenia studenta.
 * @param sfield Kierunek studiów studenta (typ wyliczeniowy StudyField).
 *
 * @return WskaŸnik na nowo utworzon¹ strukturê MyStudent.
 *         W przypadku b³êdu alokacji pamiêci zg³asza CLI_MESS_ALLOC_ERROR.
 *
 * @details Funkcja alokuje pamiêæ dla nowego obiektu typu MyStudent oraz dla jego pola surname.
 *          Nastêpnie kopiuje nazwisko, ustawia rok urodzenia i kierunek studiów, a na koñcu
 *          zwraca wskaŸnik do utworzonego obiektu.
 *          W przypadku b³êdu alokacji pamiêci dla studenta lub nazwiska, funkcja zg³asza b³¹d.
 */
MyStudent* create_student(const char* surname, int birth_year, StudyField sfield) {
    if (!surname || !is_valid_study_field(sfield)) {
        throw_cli_mess(CLI_MESS_INVALID_ARGUMENT);
        return NULL;
    }

    MyStudent* student = (MyStudent*)malloc(sizeof(MyStudent));
    if (!student) {
        throw_cli_mess(CLI_MESS_ALLOC_ERROR);
        return NULL;
    }

    size_t surname_len = strlen(surname) + 1;
    student->surname = (char*)malloc(surname_len);
    if (!student->surname) {
        free(student);
        throw_cli_mess(CLI_MESS_ALLOC_ERROR);
        return NULL;
    }

    strcpy_s(student->surname, surname_len, surname);
    student->birth_year = birth_year;
    student->sfield = sfield;

    return student;
}

/**
 * @brief Zwalnia pamiêæ zajmowan¹ przez obiekt MyStudent.
 *
 * @param student WskaŸnik na strukturê MyStudent, która ma zostaæ zwolniona.
 *
 * @details Funkcja zwalnia pamiêæ przydzielon¹ dla pola `surname` oraz samej struktury `student`.
 */
void free_student(MyStudent* student) {
    if (!student) return;

    if (student->surname) {
        free(student->surname);
        student->surname = NULL;
    }
    free(student);
}

void save_student_data(void* data, FILE* file) {
    MyStudent* student = (MyStudent*)data;
    if (!student || !file) return;

    size_t surname_len = strlen(student->surname) + 1;
    fwrite(&surname_len, sizeof(size_t), 1, file);
    fwrite(student->surname, sizeof(char), surname_len, file);
    fwrite(&student->birth_year, sizeof(int), 1, file);
    fwrite(&student->sfield, sizeof(StudyField), 1, file);
}

void* load_student_data(FILE* file) {
    if (!file || feof(file)) return NULL;

    size_t surname_len;
    if (fread(&surname_len, sizeof(size_t), 1, file) != 1) return NULL;

    char* surname = (char*)malloc(surname_len);
    if (!surname) {
        throw_cli_mess(CLI_MESS_ALLOC_ERROR);
        return NULL;
    }

    int birth_year;
    StudyField sfield;

    if (fread(surname, sizeof(char), surname_len, file) != surname_len ||
        fread(&birth_year, sizeof(int), 1, file) != 1 ||
        fread(&sfield, sizeof(StudyField), 1, file) != 1) {
        free(surname);
        return NULL;
    }

    MyStudent* student = create_student(surname, birth_year, sfield);
    free(surname);
    return student;
}

void print_student_data(void* data) {
    MyStudent* student = (MyStudent*)data;
    if (!student) return;

    printf("Student: %s, Rok urodzenia: %d, Kierunek: %s\n",
        student->surname,
        student->birth_year,
        sfields_text[student->sfield]);
}

void free_student_data(void* data) {
    printf("\n=== Rozpoczêcie zwalniania danych studenta ===\n");
    if (!data) {
        printf("B³¹d: data jest NULL\n");
        return;
    }

    MyStudent* student = (MyStudent*)data;
    printf("Adres struktury studenta: %p\n", (void*)student);
    printf("Adres nazwiska: %p\n", (void*)student->surname);
    if (student->surname) {
        printf("Nazwisko: %s\n", student->surname);
        printf("Zwalnianie nazwiska...\n");
        free(student->surname);
        student->surname = NULL;
        printf("Nazwisko zwolnione\n");
    }

    printf("Zwalnianie struktury studenta...\n");
    free(student);
    printf("Struktura studenta zwolniona\n");
    printf("=== Zakoñczenie zwalniania danych studenta ===\n\n");
}


int compare_student_data(void* data, void* criteria_ptr) {
    MyStudent* student = (MyStudent*)data;
    SearchCriteria* criteria = (SearchCriteria*)criteria_ptr;

    if (!student || !criteria) return -1;
    return matches_criteria(student, criteria) ? 0 : 1;
}

SearchCriteria* create_search_criteria(const char* surname, int birth_year, StudyField sfield) {
    SearchCriteria* criteria = (SearchCriteria*)malloc(sizeof(SearchCriteria));
    if (!criteria) {
        throw_cli_mess(CLI_MESS_ALLOC_ERROR);
        return NULL;
    }

    if (surname) {
        size_t len = strlen(surname) + 1;
        criteria->surname = (char*)malloc(len);
        if (!criteria->surname) {
            free(criteria);
            throw_cli_mess(CLI_MESS_ALLOC_ERROR);
            return NULL;
        }
        strcpy_s(criteria->surname, len, surname);
    }
    else {
        criteria->surname = NULL;
    }

    criteria->birth_year = birth_year;
    criteria->sfield = sfield;

    return criteria;
}

void free_search_criteria(SearchCriteria* criteria) {
    if (!criteria) return;
    if (criteria->surname) free(criteria->surname);
    free(criteria);
}

void print_study_fields() {
    printf("Dostepne kierunki studiow:\n");
    for (int i = MA; i <= IF; i++) {
        printf("%d - %s\n", i, sfields_text[i]);
    }
}



Stack* initialize_student_stack() {
    return initialize_stack(
        save_student_data,
        load_student_data,
        print_student_data,
        free_student_data,
        compare_student_data
    );
}


/*void read_student_from_file(Stack* stack, const char* filename) {
    if (!stack || !filename) {
        throw_cli_mess(CLI_MESS_INVALID_ARGUMENT);
        return;
    }

    FILE* file = fopen(filename, "rb");
    if (!file) {
        throw_cli_mess(CLI_MESS_FILE_ERROR);
        return;
    }

    while (1) {
        // Odczyt d³ugoœci nazwiska
        //SF dlugosc nazwiska trzeba dodac do struktury studenta, wyznaczyc przed zapisem i zapisac do pliku razem z innymi skladowymi obiektu MY_STUDENT

        size_t surname_len;
        if (fread(&surname_len, sizeof(size_t), 1, file) != 1) {
            break; // Koniec pliku
        }

        // Alokowanie pamiêci dla nazwiska
        char* surname = (char*)malloc(surname_len);
        if (!surname) {
            throw_cli_mess(CLI_MESS_ALLOC_ERROR);
            fclose(file);
            return;
        }

        // Odczyt nazwiska
        if (fread(surname, sizeof(char), surname_len, file) != surname_len) {
            free(surname);
            throw_cli_mess(CLI_MESS_FILE_READ_ERROR);
            fclose(file);
            return;
        }

        int birth_year;
        StudyField sfield;
        if (fread(&birth_year, sizeof(int), 1, file) != 1 ||
            fread(&sfield, sizeof(StudyField), 1, file) != 1) {
            free(surname);
            throw_cli_mess(CLI_MESS_FILE_READ_ERROR);
            fclose(file);
            return;
        }

        MyStudent* student = (MyStudent*)malloc(sizeof(MyStudent));
        if (!student) {
            free(surname);
            throw_cli_mess(CLI_MESS_ALLOC_ERROR);
            fclose(file);
            return;
        }

        student->surname = surname;
        student->birth_year = birth_year;
        student->sfield = sfield;

        //SF Student nie ma zadnego pojecia, do ktorego kontenera jego beda dodawali. Dla tego jest to funkcjonalnosc interfejsu, a nie studenta.
        // Dodajemy studenta na stos
        push(stack, student);
    }

    fclose(file);
}*/