#include <stdio.h>
#include <stdlib.h>

typedef enum stack_type {
  GENERAL,
  ARGUMENT,
  RETURN
}

typedef struct {
  stack_type type;
  int size;
  int array[size];
} stack

typedef struct {
    int index;
} register_t;


void initialize_stack(stack *s) {
    for (int i = 0; i < STACK_SIZE; i++) {
        t_register_stack[i].index = -1; // Initialize to -1 indicating empty
    }
}

t_register* t_pop() {
    for (int i = STACK_SIZE - 1; i >= 0; i--) {
        if (t_register_stack[i].index != -1) {
            t_register *top_register = malloc(sizeof(t_register));
            if (top_register == NULL) {
                fprintf(stderr, "Memory allocation failed during pop\n");
                exit(EXIT_FAILURE);
            }
            *top_register = t_register_stack[i];
            t_register_stack[i].index = -1;
            return top_register;
        }
    }
    return NULL;
}

int t_push(t_register *reg) {
    for (int i = 0; i < STACK_SIZE; i++) {
        if (t_register_stack[i].index == -1) {
            t_register_stack[i] = *reg;
            return 1;
        }
    }
    return 0; // Stack overflow
}

void fill_stack() {
    for (int i = 0; i < STACK_SIZE; i++) {
        t_register *reg = malloc(sizeof(t_register));
        if (reg != NULL) {
            reg->index = i;
            if (!t_push(reg)) {
                fprintf(stderr, "Stack overflow while filling\n");
                free(reg); // Free memory if push fails
                exit(EXIT_FAILURE);
            }
            free(reg); // Free memory after pushing onto stack
        } else {
            fprintf(stderr, "Memory allocation failed for register %d\n", i);
            exit(EXIT_FAILURE);
        }
    }
}

int main() {
    initialize_stack();
    fill_stack();

    t_register *reg;
    reg = t_pop();

    printf("Popped register: %d\n", reg->index);

    t_push(reg);

    return 0;
}
