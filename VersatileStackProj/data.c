#include "stdafx.h"
#include "cli_mess.h"
#include "data.h"

void save_stack_to_file(Stack* stack, const char* filename, size_t data_size) {
    if (!stack || !filename) {
        throw_cli_mess(CLI_MESS_INVALID_ARGUMENT);
        return;
    }

    FILE* file = fopen(filename, "wb");
    if (!file) {
        throw_cli_mess(CLI_MESS_FILE_ERROR);
        return;
    }

    StackItem* current = stack->top;
    while (current) {
        if (fwrite(current->data, data_size, 1, file) != 1) {
            throw_cli_mess(CLI_MESS_FILE_WRITE_ERROR);
            fclose(file);
            return;
        }
        current = current->next;
    }

    fclose(file);
}

void load_stack_from_file(Stack* stack, const char* filename, size_t data_size) {
    if (!stack || !filename) {                                                  
        throw_cli_mess(CLI_MESS_INVALID_ARGUMENT);
        return;
    }

    FILE* file = fopen(filename, "rb");
    if (!file) {
        throw_cli_mess(CLI_MESS_FILE_ERROR);
        return;
    }

    void* buffer = malloc(data_size);
    if (!buffer) {
        throw_cli_mess(CLI_MESS_ALLOC_ERROR);
        fclose(file);
        return;
    }

    while (fread(buffer, data_size, 1, file) == 1) {
        void* data = malloc(data_size);
        if (!data) {
            throw_cli_mess(CLI_MESS_ALLOC_ERROR);
            free(buffer);
            fclose(file);
            return;
        }
        memcpy(data, buffer, data_size);
        push(stack, data);
    }

    free(buffer);
    fclose(file);
}