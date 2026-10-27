#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <inttypes.h> 
#include <sys/stat.h>
#include <sys/types.h>
#include "hz.h"
#include "stack.h"

//Mawmawmawmawmaw.....
// [MAW] Стандартные команды
// [MAW] Регистры
// [mew] Метки, прыжки
// [   ] Функции
// [   ] SQRT =(
// [   ]

const char* BIN_FILE = "lst.bin";

struct code_t{
    byte_t* data;
    size_t size;
};

cpu_err_t InitCodeArray(code_t* code, const char* bin_file);

cpu_err_t ExecuteCode(code_t* code);

cpu_err_t BinaryOperation(const int operation, stack_t* stack);

void FreeCode(code_t* code);

void CpuDump(int* registers);

int main() {
    code_t code = {0};
    cpu_err_t err = CPU_OK;

    err = InitCodeArray(&code, BIN_FILE);
    if (err != CPU_OK) {
        CpuShowError(err);
        return 1;
    }

    err = ExecuteCode(&code);
    if (err != CPU_OK) {
        CpuShowError(err);
        return 1;
    }

    FreeCode(&code);

    return 0;
}

cpu_err_t InitCodeArray(code_t* code, const char* bin_file) {
    assert(code);
    assert(bin_file);

    struct stat buf = {0};
    int status = stat(bin_file, &buf);
    assert(status != -1);
    code->size = buf.st_size;

    code->data = (byte_t*)calloc(code->size, sizeof(byte_t));

    FILE* fp = fopen(bin_file, "rb");

    if (fp == NULL)
        return CPU_FOPEN;

    fread(code->data, sizeof(byte_t), code->size, fp);

    // for (size_t i = 0; i < code->size; i++) {
    //     printf("%" PRIu8 "\n", code->data[i]);
    // }
    putchar('\n');

    if (fclose(fp) < 0)
        return CPU_FCLOSE;

    return CPU_OK;
}

cpu_err_t ExecuteCode(code_t* code) {
    assert(code);

    size_t ip = 0;
    int arg = 0;
    int registers[4] = {};
    stack_err_t stack_err = STACK_OK;
    cpu_err_t cpu_err = CPU_OK;
    stack_t stack = {0};
    StackInit(&stack, 8);
    bool exe_continue = true;

    while (exe_continue) {

        StackDump(&stack);
        CpuDump(registers);

        if (ip >= code->size)
            return CPU_UNEXPECTED_IP; //nu tipo hlt netu

        if ((code->data[ip] & OPCODE_BITS) == PUSH || (code->data[ip] & OPCODE_BITS) == POP) {
            printf(" %d", *(int*)(code->data + ip + 1));
        }

        putchar('\n');

        switch (code->data[ip] & OPCODE_BITS) {
            case PUSH:
            {
                assert(ip + 4 < code->size); 
                arg = *(int*)(code->data + ip + 1);
                if (code->data[ip] & REGISTER_BIT)
                    arg = registers[*(int*)(code->data + ip + 1)];

                stack_err = StackPush(&stack, arg);

                if (stack_err != STACK_OK) {
                    StackShowError(stack_err);
                    return CPU_STACK;
                }

                ip += sizeof(byte_t) + sizeof(int);
                break;
            }
            case POP: 
            {
                assert(ip + 4 < code->size);
                if (code->data[ip] & REGISTER_BIT) {
                    stack_err = StackPop(&stack, &arg);
                    if (stack_err != STACK_OK) {
                        StackShowError(stack_err);
                        return CPU_STACK;
                    }

                    registers[*(int*)((size_t)code->data + ip + 1)] = arg;
                }

                ip += sizeof(byte_t) + sizeof(int);
                break;
            }
            case ADD:
            case SUB:
            case MUL:
            case DIV:
            {
                cpu_err = BinaryOperation(code->data[ip], &stack);
                if (cpu_err != CPU_OK) 
                    return cpu_err;

                ++ip;
                break;
            }
            case IN:
            {
                int return_value = scanf("%d", &arg);

                if (return_value != 1)
                    return CPU_SCANF;

                stack_err = StackPush(&stack, arg);
                if (stack_err != STACK_OK) {
                    StackShowError(stack_err);
                    return CPU_STACK;
                }
                
                ++ip;
                break;
            }
            case OUT:
            {
                stack_err = StackTop(&stack, &arg);
                if (stack_err != STACK_OK) {
                    StackShowError(stack_err);
                    return CPU_STACK;
                }

                printf("%d", arg);

                ++ip;
                break;
            }
            case HLT:
            {
                exe_continue = false;
                break;
            }
            default:
                return CPU_UNKNOWN_ERROR;
        }
    }

    StackFree(&stack);

    return CPU_OK;
}
//PUSH x
//ADD
//SUB
//MUL
//DIV
//IN
//OUT
//HLT
//...

cpu_err_t BinaryOperation(const int operation, stack_t* stack) {
    assert(stack);

    int a = 0, b = 0;
    stack_err_t err = STACK_OK;

    err = StackPop(stack, &b);
    if (err != STACK_OK) {
        StackShowError(err);
        return CPU_STACK;
    }

    err = StackPop(stack, &a);
    if (err != STACK_OK) {
        StackShowError(err);
        return CPU_STACK;
    }

    if (operation == (int)DIV && b == 0)
        return CPU_DIV_BY_ZERO;

    int result = 0;
    switch (operation) {
        case ADD:
            result = a + b;
            break;
        case SUB:   
            result = a - b;
            break;
        case MUL:
            result = a * b;
            break;
        case DIV:
            result = a / b;
            break;
        default:
            return CPU_UNKNOWN_ERROR;
    }
    err = StackPush(stack, result);
    if (err != STACK_OK) {
        StackShowError(err);
        return CPU_STACK;
    }

    return CPU_OK;
}

void FreeCode(code_t* code) {
    assert(code);

    free(code->data);
    code->data = NULL;
    code->size = 0;
}

void CpuDump(int* registers) {
    printf("Registers: AX          BX          CX          DX\n");
    printf("           %-11d %-11d %-11d %-11d\n", registers[0], registers[1], registers[2], registers[3]);
}