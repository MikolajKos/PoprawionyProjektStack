#include "stdafx.h"
#include "cli_mess.h"
#include "stack.h"

void check_stack_integrity(Stack* stack) {
	printf("\n=== Sprawdzanie integralno�ci stosu ===\n");
	printf("Adres stosu: %p\n", (void*)stack);
	printf("Wska�niki do funkcji:\n");
	printf("save_data: %p\n", (void*)stack->save_data);
	printf("load_data: %p\n", (void*)stack->load_data);
	printf("print_data: %p\n", (void*)stack->print_data);
	printf("free_data: %p\n", (void*)stack->free_data);
	printf("compare_data: %p\n", (void*)stack->compare_data);
	printf("===================================\n\n");
}

Stack* initialize_stack(SaveDataFunc save_fn, LoadDataFunc load_fn,
	PrintDataFunc print_fn, FreeDataFunc free_fn,
	CompareDataFunc compare_fn) {
	Stack* stack = (Stack*)malloc(sizeof(Stack));
	if (!stack) {
		throw_cli_mess(CLI_MESS_ALLOC_ERROR);
		return NULL;
	}

	stack->top = NULL;
	stack->save_data = save_fn;
	stack->load_data = load_fn;
	stack->print_data = print_fn;
	stack->free_data = free_fn;
	stack->compare_data = compare_fn;

	//SF 'initialize_stack': must return a value
	return stack;
}

/**
 * @brief Zwalnia pami�� zajmowan� przez stos i jego elementy.
 *
 * @param stack Podw�jny wska�nik na struktur� stosu. Pozwala ustawi� oryginalny wska�nik na NULL po zwolnieniu pami�ci.
 *
 * @details Funkcja iteracyjnie zwalnia pami�� zajmowan� przez elementy stosu i dane w nich przechowywane.
 *          Nast�pnie zwalnia pami�� samej struktury stosu i ustawia wska�nik na NULL, aby unikn�� ponownego u�ycia.
 *          Je�eli wska�nik jest NULL lub wskazuje na NULL, zg�aszany jest b��d CLI_MESS_UNINITIALIZED_STACK.
 *
 * @return Funkcja nie zwraca warto�ci.
 *
 * @note Zabezpieczona przed ponownym zwalnianiem tego samego stosu.
 */
void free_stack(Stack** stack) {
	if (!stack || !(*stack)) {
		throw_cli_mess(CLI_MESS_UNINITIALIZED_STACK);
		return;
	}

	// Zachowaj wska�nik do funkcji free_data
	FreeDataFunc free_fn = (*stack)->free_data;
	if (!free_fn) {
		printf("Ostrze�enie: free_data jest NULL\n");
	}

	int items_freed = 0;
	StackItem* current = (*stack)->top;

	while (current != NULL) {
		StackItem* next = current->next;

		// Zwalnianie danych
		if (current->data && free_fn) {
			printf("Pr�ba zwolnienia danych pod adresem: %p\n", current->data);
			free_fn(current->data);
			current->data = NULL;
		}

		// Zwalnianie elementu stosu
		free(current);
		items_freed++;
		current = next;
	}

	// Zwalnianie struktury stosu
	free(*stack);
	*stack = NULL;

	printf("Zwolniono %d element�w stosu\n", items_freed);
}

/**
 * @brief Dodaje nowy element na szczyt stosu.
 *
 * @param stack Wska�nik na struktur� stosu.
 * @param data Wska�nik na dane, kt�re maj� zosta� dodane do stosu.
 *
 * @note W przypadku b��du alokacji zg�asza CLI_MESS_ALLOC_ERROR.
 */
void push(Stack* stack, void* data) {
	if (!stack) {
		throw_cli_mess(CLI_MESS_UNINITIALIZED_STACK);
		return;
	}

	StackItem* new_item = (StackItem*)malloc(sizeof(StackItem));
	if (!new_item) {
		throw_cli_mess(CLI_MESS_ALLOC_ERROR);
		return;
	}

	new_item->data = data;
	new_item->next = stack->top;
	stack->top = new_item;
}

/**
 * @brief Usuwa i zwraca element znajduj�cy si� na szczycie stosu.
 *
 * @param stack Wska�nik na struktur� stosu, z kt�rego ma zosta� usuni�ty element.
 *
 * @return Wska�nik na dane przechowywane w usuni�tym elemencie.
 *         Je�li stos jest pusty, zwraca NULL i zg�asza CLI_MESS_STACK_EMPTY.
 *
 * @details Funkcja usuwa element znajduj�cy si� na szczycie stosu, zwalnia pami�� zajmowan� przez ten element,
 *          aktualizuje wska�nik top stosu oraz zwraca wska�nik na dane usuni�tego elementu.
 */
void* pop(Stack* stack) {
	if (!stack) {
		throw_cli_mess(CLI_MESS_UNINITIALIZED_STACK);
		return NULL;
	}

	if (!stack->top) {
		throw_cli_mess(CLI_MESS_STACK_EMPTY);
		return NULL;
	}

	StackItem* top_item = stack->top;
	void* data = top_item->data;
	stack->top = top_item->next;
	free(top_item);

	return data;
}

void stack_print_all(Stack* stack) {
	if (!stack) {
		throw_cli_mess(CLI_MESS_UNINITIALIZED_STACK);
		return;
	}

	if (!stack->print_data) {
		throw_cli_mess(CLI_MESS_INVALID_ARGUMENT);
		return;
	}

	if (!stack->top) {
		throw_cli_mess(CLI_MESS_STACK_EMPTY);
		return;
	}

	StackItem* current = stack->top;
	int index = 1;
	while (current) {
		printf("%d. ", index++);
		stack->print_data(current->data);
		current = current->next;
	}
}

void stack_save_to_file(Stack* stack, const char* filename) {
	if (!stack || !filename) {
		throw_cli_mess(CLI_MESS_INVALID_ARGUMENT);
		return;
	}

	if (!stack->save_data) {
		throw_cli_mess(CLI_MESS_INVALID_OPERATION);
		return;
	}

	FILE* file = fopen(filename, "wb");
	if (!file) {
		throw_cli_mess(CLI_MESS_FILE_ERROR);
		return;
	}

	StackItem* current = stack->top;
	while (current) {
		stack->save_data(current->data, file);
		current = current->next;
	}

	fclose(file);
}

void stack_load_from_file(Stack* stack, const char* filename) {
	if (!stack || !filename) {
		throw_cli_mess(CLI_MESS_INVALID_ARGUMENT);
		return;
	}

	if (!stack->load_data) {
		throw_cli_mess(CLI_MESS_INVALID_OPERATION);
		return;
	}

	FILE* file = fopen(filename, "rb");
	if (!file) {
		throw_cli_mess(CLI_MESS_FILE_ERROR);
		return;
	}

	// Czyszczenie istniej�cego stosu
	while (stack->top) {
		void* data = pop(stack);
		if (stack->free_data) {
			stack->free_data(data);
		}
	}

	while (!feof(file)) {
		void* data = stack->load_data(file);
		if (data) {
			push(stack, data);
		}
	}

	fclose(file);
}

void* stack_find_by_criteria(Stack* stack, void* criteria) {
    if (!stack || !criteria) {
        throw_cli_mess(CLI_MESS_INVALID_ARGUMENT);
        return NULL;
    }

    if (!stack->compare_data) {
        throw_cli_mess(CLI_MESS_INVALID_OPERATION);
        return NULL;
    }

    StackItem* current = stack->top;
    while (current) {
        if (stack->compare_data(current->data, criteria) == 0) {
            return current->data;
        }
        current = current->next;
    }

    return NULL;
}
