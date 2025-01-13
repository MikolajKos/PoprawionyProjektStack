#include "stdafx.h"
#include "user_interface.h"
#include "MY_STUDENT.h"
#include "cli_mess.h"
#include "data.h"

void UserMenu(void) {
	int option, type_option;
	Stack* stack = interf_initialize_stack();

	do {
		printf_s("Wybierz rodzaj danych:\n");
		printf_s("  1 - student\n  2 - samochod\n");
		if (scanf_s("%d", &type_option) != 1) {
			while (getchar() != '\n'); // Czyszczenie bufora
			throw_cli_mess(CLI_MESS_OPTION_WARN);
			continue;
		}

		if (type_option != 1 && type_option != 2) {
			throw_cli_mess(CLI_MESS_OPTION_WARN);
		}
		else {
			break;
		}
	} while (true);

	do {
		printf_s("  1 - zwolnij pamiec stosu\n  2 - dodaj nowy element\n  3 - pobierz pierwszy element\n  4 - znajdz element\n  5 - wypisz wszystkie elementy stosu\n  6 - zapisz na dysku\n  7 - odczytaj z dysku\n  8 - Zainicjalizuj stos\n  0 - Wyjdz\n\n");

		if (scanf_s("%d", &option) != 1) {
			while (getchar() != '\n');
			printf_s("Nieprawidlowa opcja!\n");
			continue;
		}

		switch (option) {
		case 1:
			//SF: warning C4047: 'function': 'Stack *' differs in levels of indirection from 'Stack **'
			interf_free_stack(stack);
			break;
		case 2:
			interf_push(stack, type_option);
			break;
		case 3:
			interf_pop(stack, type_option);
			break;
		case 4:
			interf_find_by_criteria("stackdata.bin", type_option);
			break;
		case 5:
			interf_peek_all(stack, type_option);
			break;
		case 6:
			interf_save_to_file(stack, "stackdata.bin", type_option);
			break;
		case 7:
			interf_load_from_file("stackdata.bin", type_option);
			break;
		case 8:
			if (stack) interf_free_stack(&stack);
			stack = interf_initialize_stack();
			break;
		case 0:
			if (stack) interf_free_stack(&stack);
			printf_s("Do widzenia!\n");
			break;
		default:
			printf_s("Nieprawidlowa opcja!\n");
			break;
		}
	} while (option != 0);
}

void interf_free_stack(Stack* stack) {
	if (stack) {
		free_stack(stack);
		stack = NULL;
	}
}

void interf_push(Stack* stack, int option) {
	if (!stack) {
		throw_cli_mess(CLI_MESS_UNINITIALIZED_STACK);
		return;
	}

	switch (option) {
	case 1: {
		char surname[50];
		int birth_year;
		int field_of_study;

		printf_s("Podaj nazwisko: ");
		while (getchar() != '\n'); // Czyszczenie bufora
		if (fgets(surname, sizeof(surname), stdin)) {
			surname[strcspn(surname, "\n")] = 0; // Usuniêcie znaku nowej linii
		}

		printf_s("Podaj rok urodzenia: ");
		while (scanf_s("%d", &birth_year) != 1) {
			printf_s("Nieprawidlowy rok. Sprobuj ponownie: ");
			while (getchar() != '\n');
		}

		print_study_fields();
		printf_s("Podaj kierunek studiow: ");
		while (scanf_s("%d", &field_of_study) != 1 ||
			field_of_study < MA || field_of_study > IF) {
			printf_s("Nieprawidlowy kierunek. Sprobuj ponownie: ");
			while (getchar() != '\n');
		}

		MyStudent* student = create_student(surname, birth_year, field_of_study);
		if (student) {
			push(stack, student);
		}
		break;
	}
	case 2:
		throw_cli_mess(CLI_MESS_WRONG_TYPE);
		break;
	}
}
																									   
void interf_pop(Stack* stack, int option) {		
	if (!stack) {
		throw_cli_mess(CLI_MESS_UNINITIALIZED_STACK);
		return;
	}

	switch (option){
		case 1: {
			MyStudent* result;
			if (!(result = (void*)pop(stack))) {
				break;
			}
			else {
				printf_s("Surname: %s, Birth Year: %d, Field: %s \n",
					result->surname,
					result->birth_year,
					sfields_text[result->sfield]);
			}
			break;
		}
		case 2: {
			throw_cli_mess(CLI_MESS_WRONG_TYPE);
			break;
		}
		default:
			break;
	}																			   
}																									   

void interf_peek_all(Stack* stack, int option) {
	if (!stack) {
		throw_cli_mess(CLI_MESS_UNINITIALIZED_STACK);
		return;
	}

	StackItem* current = stack->top;
	int count = 0;

	switch (option) {
	case 1: {
		while (current != NULL) {
			print_student_data(current->data);
			current = current->next;
			count++;
		}
		if (count == 0) {
			printf("Stos jest pusty.\n");
		}
		break;
	}
	case 2: {
		throw_cli_mess(CLI_MESS_WRONG_TYPE);
		break;
	}
	default:
		break;
	}
}

void interf_save_to_file(Stack* stack, const char* filename, int option) {
	if (!stack) {
		throw_cli_mess(CLI_MESS_UNINITIALIZED_STACK);
		return;
	}

	switch (option) {
	case 1: {
		FILE* file;
		if (fopen_s(&file, filename, "wb") != 0) {
			throw_cli_mess(CLI_MESS_FILE_ERROR);
			return;
		}

		StackItem* current = stack->top;
		//SF warning C4047: 'function': 'void **' differs in levels of indirection from 'StackItem *'
		while (current != NULL) {
			save_student_data(current->data, file);
			current = current->next;
		}

		fclose(file);
		printf("Dane zostaly zapisane do pliku.\n");
		break;
	}
	case 2: {
		throw_cli_mess(CLI_MESS_WRONG_TYPE);
		break;
	}
	default:
		break;
	}
}

void interf_load_from_file(const char* filename, int option) {
	switch (option) {
	case 1: {
		Stack** tmp_stack_ptr = malloc(sizeof(Stack*));
		if (!tmp_stack_ptr) {
			throw_cli_mess(CLI_MESS_ALLOC_ERROR);
			return;
		}

		*tmp_stack_ptr = initialize_stack(
			save_student_data,
			load_student_data,
			print_student_data,
			free_student_data,
			compare_student_data
		);

		FILE* file;
		if (fopen_s(&file, filename, "rb") != 0) {
			throw_cli_mess(CLI_MESS_FILE_ERROR);
			free(tmp_stack_ptr);
			return;
		}

		while (!feof(file)) {
			void* data = load_student_data(file);
			if (data) {
				push(*tmp_stack_ptr, data);
			}
		}
		fclose(file);

		// Wyœwietl wczytane dane
		interf_peek_all(*tmp_stack_ptr, option);

		// Zwolnij pamiêæ
		interf_free_stack(tmp_stack_ptr);
		free(tmp_stack_ptr);
		break;
	}
	case 2: {
		throw_cli_mess(CLI_MESS_WRONG_TYPE);
		break;
	}
	default:
		break;
	}
}

void interf_find_by_criteria(const char* filename, int option) {
	switch (option) {
	case 1: {
		char surname[60];
		int birth_year = -1;
		int sfield = -1;

		printf_s("Podaj szukane nazwisko [(x) - pomin]: ");
		while (getchar() != '\n'); // Czyszczenie bufora
		if (fgets(surname, sizeof(surname), stdin)) {
			surname[strcspn(surname, "\n")] = 0;
		}

		char* search_surname = (surname[0] == 'x') ? NULL : surname;

		printf_s("Podaj szukany rok urodzenia [(-1) - pomin]: ");
		while (scanf_s("%d", &birth_year) != 1) {
			printf_s("Nieprawidlowy rok. Sprobuj ponownie: ");
			while (getchar() != '\n');
		}

		print_study_fields();
		printf_s("Podaj szukany kierunek [(-1) - pomin]: ");
		while (scanf_s("%d", &sfield) != 1) {
			printf_s("Nieprawidlowy kierunek. Sprobuj ponownie: ");
			while (getchar() != '\n');
		}

		find_and_print_students_by_criteria(filename, search_surname, birth_year, sfield);
		break;
	}
	case 2:
		throw_cli_mess(CLI_MESS_WRONG_TYPE);
		break;
	}
}

Stack* interf_initialize_stack(void) {
	return initialize_stack(
		save_student_data,      // callback do zapisu
		load_student_data,      // callback do odczytu
		print_student_data,     // callback do wyœwietlania
		free_student_data,      // callback do zwalniania pamiêci
		compare_student_data    // callback do porównywania
	);
	//SF: warning C4716: 'interf_initialize_stack': must return a value
}
