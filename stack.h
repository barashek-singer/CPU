#ifndef STACK_H_INCLUDED

#define STACK_H_INCLUDED

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <stdint.h>
#include <inttypes.h>
#include "colors.h"

#define stack_elem_specifier "d"
#define GetName(var) #var

#ifdef STACK_DEBUG
#define ON_DEBUG(...) __VA_ARGS__
#else
#define ON_DEBUG(...)
#endif

typedef int stack_elem_t;
typedef uint64_t hash_t;
const stack_elem_t POISON  = 0xDEADDEAD;
const size_t STACK_CAPACITY_COEF = 2;
const uint64_t BASE_CANARY = 0xD8D3792ABC287E99;

const hash_t START_HASH = 5381;
const hash_t HASH_COEF = 33;

enum stack_err_t {
    STACK_OK                = 0,
    STACK_OOM               = (1 << 0),
    STACK_NOT_INIT          = (1 << 1),
    STACK_EMPTY             = (1 << 2),
    STACK_BAD_SIZE          = (1 << 3),
    STACK_BAD_ARG           = (1 << 4),
    STACK_DOUBLE_INIT       = (1 << 5),
    STACK_DEAD_CANARY       = (1 << 6),
    STACK_DEAD_BY_POISON    = (1 << 7),
    STACK_INVALID_HASH      = (1 << 8)
};

struct stack_t {
    stack_elem_t* data;
    size_t size;
    size_t capacity;
    hash_t hash;
    
//˜˜˜˜˜ Init'˜
#ifdef STACK_DEBUG  //TODO ˜˜ ˜˜˜ ˜˜˜˜˜˜˜ ˜ ˜˜˜
    const char* file;
    const char* func;
    const char* name; //?
    int line;
#endif
};

stack_err_t StackVerify(const stack_t* stack);
//˜˜˜˜˜˜˜˜ ˜˜˜˜˜ ˜˜ ˜˜˜˜˜˜˜˜˜˜˜˜

void    StackDump(const stack_t* stack);
//˜˜˜˜˜ ˜˜˜˜˜

bool StackIsEmpty(const stack_t* stack);
//˜˜˜˜˜˜˜˜ ˜˜ ˜˜˜˜˜˜˜ ˜˜˜˜˜

stack_err_t StackInit(stack_t* stack, size_t capacity ON_DEBUG(, const char* file, const char* func, int line, const char* name));
//˜˜˜˜˜˜˜˜˜˜˜˜˜ ˜˜˜˜˜

stack_err_t StackPush(stack_t* stack, stack_elem_t new_elem);
//˜˜˜˜˜˜˜˜ ˜˜˜˜˜˜˜ ˜˜ ˜˜˜˜ ˜˜˜˜˜

stack_err_t  StackPop(stack_t* stack, stack_elem_t* elem);
//˜˜˜˜˜˜ ˜˜˜˜˜˜˜ ˜ ˜˜˜˜˜ ˜˜˜˜˜

stack_err_t  StackTop(const stack_t* stack, stack_elem_t* elem);
//˜˜˜˜˜˜˜˜˜˜ ˜˜˜˜˜˜˜ ˜ ˜˜˜˜˜ ˜˜˜˜˜

stack_err_t StackFree(stack_t* stack);
//˜˜˜˜˜˜˜˜˜˜ ˜˜˜˜˜

stack_err_t     StackMergeErrors(stack_err_t err1, stack_err_t err2);
//Îáúåäèíåíèå

stack_err_t StackIntersectErrors(stack_err_t err1, stack_err_t err2);
//Ïåðåñå÷åíèå îøèáîê

bool StackCheckError(stack_err_t full_err, stack_err_t cur_err);
//Ïðîâåðÿåò ñîäåðæèòüñÿ ëè cur_err â full_err P.S. return (full_err ñîäåðæèò cur_err)

void StackShowError(const stack_err_t err);
//Ïîêàç îøèáîê

hash_t CalcHash(const void* src, size_t size, hash_t start_hash);
//Ïîäñ÷¸ò õýøà

hash_t StackCalcHash(const stack_t* stack);
//Ïîäñ÷¸ò õýøà ñòåêà

void StackUnitTesting();
//Çàïóñê UNIT òåñòèðîâàíèÿ:
//{
    void StackUnitTestOOM();
    void StackUnitTestNotInit();
    void StackUnitTestEmpty();
    void StackUnitTestBadSize();
    void StackUnitTestBadArg();
    void StackUnitTestDoubleInit();
    void StackUnitTestDeadCanary();
    void StackUnitTestDeadByPoison();
    void StackUnitTestInvalidHash();
//}

stack_err_t StackInit(stack_t* stack, size_t capacity ON_DEBUG(, const char* file, const char* func, int line, const char* name)) {
    if (stack == NULL)
        return STACK_BAD_ARG;

    if (stack->data != NULL)
        return STACK_DOUBLE_INIT;

    if (capacity != 0)
        stack->capacity = (capacity / 8 + 1) * 8; //TODO ˜˜˜˜˜˜ ˜˜˜˜˜˜˜ ˜˜˜˜˜˜

    if (stack->capacity * sizeof(stack_elem_t) <= stack->capacity) //˜˜˜˜˜˜˜˜˜ ˜˜˜˜˜˜˜˜ ˜˜ ˜˜˜˜˜˜˜˜˜˜˜˜
        return STACK_OOM;

    stack->data = (stack_elem_t*)calloc(1, 2 * sizeof(uint64_t) + stack->capacity * sizeof(stack_elem_t)); //˜˜˜˜˜˜˜˜˜˜ ˜˜˜˜˜˜ ˜˜˜˜˜˜˜ ˜ ˜˜˜˜˜˜˜˜˜˜˜
    if (stack->data == NULL) //˜˜˜˜˜˜
        return STACK_OOM;

    stack->data = (stack_elem_t*)((size_t)stack->data + sizeof(uint64_t));

    stack->size = 0;

    *((uint64_t*)((size_t)stack->data - sizeof(size_t))) = ((uint64_t)stack ^ BASE_CANARY); //˜˜˜˜˜ ˜˜˜˜˜˜˜˜˜

    for (size_t i = 0; i <= stack->capacity; i++)
        stack->data[i] = POISON;

    *((uint64_t*)((size_t)stack->data + sizeof(stack_elem_t) * stack->capacity)) = ((uint64_t)stack ^ BASE_CANARY); //˜˜˜˜˜˜ ˜˜˜˜˜˜˜˜˜

#ifdef STACK_DEBUG
    stack->file = file;
    stack->func = func;
    stack->line = line;
    stack->name = name;
#endif

    stack->hash = StackCalcHash(stack);

    return STACK_OK;
}

stack_err_t StackVerify(const stack_t* stack) {
    stack_err_t err = STACK_OK;
    
    if (stack == NULL)
        return STACK_BAD_ARG;

    if (stack->data == NULL)
        return STACK_NOT_INIT;

    if (stack->size > stack->capacity)
        err = StackMergeErrors(err, STACK_BAD_SIZE);

    if (*((uint64_t*)((size_t)stack->data - sizeof(size_t))) != ((uint64_t)stack ^ BASE_CANARY) ||
            *((uint64_t*)((size_t)stack->data + sizeof(stack_elem_t) * stack->capacity)) != ((uint64_t)stack ^ BASE_CANARY)) {

        err = StackMergeErrors(err, STACK_DEAD_CANARY);
    }

    for (size_t i = stack->size; i < stack->capacity; i++)
        if (stack->data[i] != POISON)
            err = StackMergeErrors(err, STACK_DEAD_BY_POISON);

    if (stack->hash != StackCalcHash(stack))
        err = StackMergeErrors(err, STACK_INVALID_HASH);

    return err;
}

void StackDump(const stack_t* stack) {
    putchar('\n');

    static size_t cnt_dump = 1;
    ChooseModification(GREEN_TEXT);
    printf("%zu dump\n", cnt_dump++);

    stack_err_t err = StackVerify(stack);
    if (err != STACK_OK) {
        ChooseModification(RED_TEXT);

        printf("StackDump can't show you this stack (%p), because:\n", stack);
        StackShowError(err);
        return;
    }

    ChooseModification(PURPLE_TEXT);

    ON_DEBUG(printf("Init %s: file = %s, func = %s, line %d\n", stack->name, stack->file, stack->func, stack->line));

    printf("stack_t stack[%p]{\n", stack);
    
    {
        ChooseModification(YELLOW_TEXT);

        printf("\t.size = %zu,\n", stack->size);
        printf("\t.capacity = %zu,\n", stack->capacity);

        printf("\tleft_canary = %" PRIu64 " (correct canary = %" PRIu64 ")\n", 
                        *((uint64_t*)((size_t)stack->data - sizeof(size_t))), ((uint64_t)(stack) ^ BASE_CANARY));

        printf("\t.data[%p] = {\n", stack->data);
        for (size_t i = 0; i < stack->capacity; i++) {
            ChooseModification(CYAN_TEXT);
            if (i < stack->size)
                printf("\t\t*[%zu] = %" stack_elem_specifier ",\n",  i, stack->data[i]);

            else
                printf("\t\t [%zu] = %" stack_elem_specifier ",\n",  i, stack->data[i]);
        }

        printf("\tright_canary = %" PRIu64 " (correct canary = %" PRIu64 ")\n", 
                        *((uint64_t*)((size_t)stack->data + sizeof(stack_elem_t) * stack->capacity)), ((uint64_t)(stack) ^ BASE_CANARY));
 
        ChooseModification(YELLOW_TEXT);

        printf("\t}\n");
    }

    printf("};\n\n");

    ChooseModification(RESET_TEXT);
}

stack_err_t StackPush(stack_t* stack, stack_elem_t new_elem) {
    stack_err_t err = StackVerify(stack);
    if (err != STACK_OK)
        return err;

    if (stack->size == stack->capacity) {
        if (stack->capacity == 0)
            stack->capacity = 8;

        stack_elem_t* new_data = (stack_elem_t*)realloc((stack_elem_t*)((size_t)stack->data - sizeof(uint64_t)), 
                                                        2 * sizeof(uint64_t) + sizeof(stack_elem_t) * (stack->capacity * STACK_CAPACITY_COEF));
        if (new_data == NULL)
            return STACK_OOM;

        stack->capacity *= STACK_CAPACITY_COEF;
        stack->data = (stack_elem_t*)((size_t)new_data + sizeof(uint64_t));

        for (size_t i = stack->size; i < stack->capacity; i++)
            stack->data[i] = POISON;

        *((uint64_t*)((size_t)stack->data + sizeof(stack_elem_t) * stack->capacity)) = ((uint64_t)stack ^ BASE_CANARY); //˜˜˜˜˜˜ ˜˜˜˜˜˜˜˜˜
    }

    stack->data[stack->size++] = new_elem;
    stack->hash = StackCalcHash(stack);

    err = StackVerify(stack);
    if (err != STACK_OK)
        return err;

    return STACK_OK;
}

bool StackIsEmpty(const stack_t* stack) {
    return stack->size == 0;
}

stack_err_t StackPop(stack_t* stack, stack_elem_t* elem) {
    stack_err_t err = StackVerify(stack);
    if (err != STACK_OK)
        return err;

    if (elem == NULL)
        return STACK_BAD_ARG;

    if (StackIsEmpty(stack))
        return STACK_EMPTY;

    if (stack->capacity > 8 && STACK_CAPACITY_COEF * STACK_CAPACITY_COEF * stack->size < stack->capacity) {
        stack_elem_t* new_data = (stack_elem_t*)realloc((stack_elem_t*)((size_t)stack->data - sizeof(uint64_t)), 
                                                        2 * sizeof(uint64_t) + sizeof(stack_elem_t) * (stack->capacity / STACK_CAPACITY_COEF));

        if (new_data == NULL)
            return STACK_OOM;

        stack->capacity /= STACK_CAPACITY_COEF;
        stack->data = (stack_elem_t*)((size_t)new_data + sizeof(uint64_t));

        *((uint64_t*)((size_t)stack->data + sizeof(stack_elem_t) * stack->capacity)) = ((uint64_t)stack ^ BASE_CANARY); //˜˜˜˜˜˜ ˜˜˜˜˜˜˜˜˜
    }

    *elem = stack->data[--stack->size];
    stack->data[stack->size] = POISON;
    stack->hash = StackCalcHash(stack);

    err = StackVerify(stack);
    if (err != STACK_OK)
        return err;

    return STACK_OK;
}

stack_err_t  StackTop(const stack_t* stack, stack_elem_t* elem) {//Good
    stack_err_t err = StackVerify(stack);
    if (err != STACK_OK)
        return err;
    
    if (StackIsEmpty(stack))
        return STACK_EMPTY;

    if (elem == NULL)
        return STACK_BAD_ARG;

    *elem = stack->data[stack->size - 1];
    return STACK_OK;
}

stack_err_t StackFree(stack_t* stack) {
    stack_err_t err = StackVerify(stack);
    if (StackCheckError(err, STACK_BAD_ARG) || StackCheckError(err, STACK_NOT_INIT))
        return StackIntersectErrors(err, StackMergeErrors(STACK_BAD_ARG, STACK_NOT_INIT));

    free((void*)((size_t)stack->data - sizeof(uint64_t)));
    stack->data = NULL;
    stack->size = 0;
    stack->capacity = 0;
    stack->hash = 0;

    return STACK_OK;
}

void StackShowError(const stack_err_t err) {
    if (StackIntersectErrors(err, STACK_OOM)) {
        ChooseModification(PURPLE_TEXT);
        printf("warning: not enough memory\n");
    }

    if (StackIntersectErrors(err, STACK_NOT_INIT)) {
        ChooseModification(RED_TEXT);
        printf("error: Stack is not init\n");
    }

    if (StackIntersectErrors(err, STACK_EMPTY)) {
        ChooseModification(PURPLE_TEXT);
        printf("warning: stack is empty\n");
    }

    if (StackIntersectErrors(err, STACK_BAD_SIZE)) {
        ChooseModification(RED_TEXT);
        printf("Wrong size of stack\n");
    }
    
    if (StackIntersectErrors(err, STACK_BAD_ARG)) {
        ChooseModification(PURPLE_TEXT);
        printf("NULL pointer in argument\n");
    }

    if (StackIntersectErrors(err, STACK_DOUBLE_INIT)) {
        ChooseModification(PURPLE_TEXT);
        printf("Stack is init already. Create another stack or free that to new init\n");
    }

    if (StackIntersectErrors(err, STACK_DEAD_CANARY)) {
        ChooseModification(RED_TEXT);
        printf("error: stack is not valid (dead canary)\n");
    }

    if (StackIntersectErrors(err, STACK_DEAD_BY_POISON)) {
        ChooseModification(RED_TEXT);
        printf("error: stack is not valid (poison)\n");
    }

    if (StackIntersectErrors(err, STACK_INVALID_HASH)) {
        ChooseModification(RED_TEXT);
        printf("error: stack is not valid (invalid_hash)\n");
    }

    ChooseModification(RESET_TEXT);
}

stack_err_t     StackMergeErrors(stack_err_t err1, stack_err_t err2) {
    return (stack_err_t)((int)err1 | (int)err2);
}

stack_err_t StackIntersectErrors(stack_err_t err1, stack_err_t err2) {
    return (stack_err_t)((int)err1 & (int)err2);
}

bool StackCheckError(stack_err_t full_err, stack_err_t cur_err) {
    return StackIntersectErrors(full_err, cur_err) == cur_err;
}

hash_t CalcHash(const void* src, size_t size, hash_t start_hash) { // https://gist.github.com/MohamedTaha98/ccdf734f13299efb73ff0b12f7ce429f //Good
    if (src == NULL)
        return (hash_t)0;

    const char* ptr = (const char*)src;
    hash_t hash = start_hash;

    while (size--)
        hash = (hash * HASH_COEF) + *ptr++; /* hash * 33 + c */

    return hash;
}

hash_t StackCalcHash(const stack_t* stack) {
    if (stack == NULL)
        return (hash_t)0;

    return CalcHash(stack->data, stack->capacity * sizeof(stack_elem_t), CalcHash(stack, 3 * sizeof(size_t), START_HASH));
}

void StackUnitTesting() {
    StackUnitTestOOM();
    StackUnitTestNotInit();
    StackUnitTestEmpty();
    StackUnitTestBadSize();
    StackUnitTestBadArg();
    StackUnitTestDoubleInit();
    StackUnitTestDeadCanary();
    StackUnitTestDeadByPoison();
    StackUnitTestInvalidHash();
}

void StackUnitTestOOM() {
    stack_t stack = {0};
    stack_err_t err = STACK_OK;

    err = StackInit(&stack, 12938018230980 ON_DEBUG(,__FILE__, __func__, __LINE__, GetName(stack)));

    ChooseModification(CYAN_TEXT);
    printf("STACK_OOM: ");

    if (StackCheckError(err, STACK_OOM)) {
        ChooseModification(GREEN_TEXT);
        printf("Catched\n");
    }
    else {
        ChooseModification(RED_TEXT);
        printf("Missed\n");
    }

    ChooseModification(RESET_TEXT);
}

void StackUnitTestNotInit() {
    stack_t stack = {0};
    stack_err_t err = STACK_OK;

    err = StackVerify(&stack);

    ChooseModification(CYAN_TEXT);
    printf("STACK_NOT_INIT: ");

    if (StackCheckError(err, STACK_NOT_INIT)) {
        ChooseModification(GREEN_TEXT);
        printf("Catched\n");
    }
    else {
        ChooseModification(RED_TEXT);
        printf("Missed\n");
    }

    ChooseModification(RESET_TEXT);
}

void StackUnitTestEmpty() {
    stack_t stack = {0};
    stack_err_t err = STACK_OK;
    stack_elem_t el = 0;

    StackInit(&stack, 8 ON_DEBUG(,__FILE__, __func__, __LINE__, GetName(stack)));

    err = StackTop(&stack, &el);

    ChooseModification(CYAN_TEXT);
    printf("STACK_EMPTY: ");

    if (StackCheckError(err, STACK_EMPTY)) {
        ChooseModification(GREEN_TEXT);
        printf("Catched\n");
    }
    else {
        ChooseModification(RED_TEXT);
        printf("Missed\n");
    }

    ChooseModification(RESET_TEXT);
}

void StackUnitTestBadSize() {
    stack_t stack = {0};
    stack_err_t err = STACK_OK;

    StackInit(&stack, 8 ON_DEBUG(,__FILE__, __func__, __LINE__, GetName(stack)));
    stack.size = 200;

    err = StackVerify(&stack);

    ChooseModification(CYAN_TEXT);
    printf("STACK_BAD_SIZE: ");

    if (StackCheckError(err, STACK_BAD_SIZE)) {
        ChooseModification(GREEN_TEXT);
        printf("Catched\n");
    }
    else {
        ChooseModification(RED_TEXT);
        printf("Missed\n");
    }

    ChooseModification(RESET_TEXT);
}

void StackUnitTestBadArg() {
    stack_t stack = {0};
    stack_err_t err1 = STACK_OK;
    stack_err_t err2 = STACK_OK;
    stack_err_t err3 = STACK_OK;

    StackInit(&stack, 8 ON_DEBUG(,__FILE__, __func__, __LINE__, GetName(stack)));

    err1 = StackVerify(NULL);

    StackPush(&stack, 1);
    err2 = StackTop(&stack, NULL);
    err3 = StackTop(&stack, NULL);

    ChooseModification(CYAN_TEXT);
    printf("STACK_BAD_ARG: ");

    if  (StackCheckError(err1, STACK_BAD_ARG) &&
         StackCheckError(err2, STACK_BAD_ARG) &&
         StackCheckError(err3, STACK_BAD_ARG)) {

        ChooseModification(GREEN_TEXT);
        printf("Catched\n");
    }
    else {
        ChooseModification(RED_TEXT);
        printf("Missed\n");
    }

    ChooseModification(RESET_TEXT);
}

void StackUnitTestDoubleInit() {
    stack_t stack = {0};
    stack_err_t err = STACK_OK;

    StackInit(&stack, 8 ON_DEBUG(,__FILE__, __func__, __LINE__, GetName(stack)));
    err = StackInit(&stack, 8 ON_DEBUG(,__FILE__, __func__, __LINE__, GetName(stack)));

    ChooseModification(CYAN_TEXT);
    printf("STACK_DOUBLE_INIT: ");

    if (StackCheckError(err, STACK_DOUBLE_INIT)) {
        ChooseModification(GREEN_TEXT);
        printf("Catched\n");
    }
    else {
        ChooseModification(RED_TEXT);
        printf("Missed\n");
    }

    ChooseModification(RESET_TEXT);
}

void StackUnitTestDeadCanary() {
    stack_t stack1 = {0};
    stack_t stack2 = {0};
    stack_err_t err1 = STACK_OK;
    stack_err_t err2 = STACK_OK;

    StackInit(&stack1, 8 ON_DEBUG(,__FILE__, __func__, __LINE__, GetName(stack)));
    StackInit(&stack2, 8 ON_DEBUG(,__FILE__, __func__, __LINE__, GetName(stack)));

    *((uint64_t*)((size_t)stack1.data - sizeof(uint64_t))) = 0;
    err1 = StackVerify(&stack1);

    *((uint64_t*)((size_t)stack2.data + stack2.capacity * sizeof(stack_elem_t))) = 0;
    err2 = StackVerify(&stack2);

    ChooseModification(CYAN_TEXT);
    printf("STACK_DEAD_CANARY: ");

    if  (StackCheckError(err1, STACK_DEAD_CANARY) &&
         StackCheckError(err2, STACK_DEAD_CANARY)) {

        ChooseModification(GREEN_TEXT);
        printf("Catched\n");
    }
    else {
        ChooseModification(RED_TEXT);
        printf("Missed\n");
    }

    ChooseModification(RESET_TEXT);
}

void StackUnitTestDeadByPoison() {
    stack_t stack = {0};
    stack_err_t err = STACK_OK;

    StackInit(&stack, 8 ON_DEBUG(,__FILE__, __func__, __LINE__, GetName(stack)));

    *(stack.data) = 0;
    err = StackVerify(&stack);

    ChooseModification(CYAN_TEXT);
    printf("STACK_DEAD_BY_POISON: ");

    if (StackCheckError(err, STACK_DEAD_BY_POISON)) {
        ChooseModification(GREEN_TEXT);
        printf("Catched\n");
    }
    else {
        ChooseModification(RED_TEXT);
        printf("Missed\n");
    }

    ChooseModification(RESET_TEXT);
}

void StackUnitTestInvalidHash() {
    stack_t stack = {0};
    stack_err_t err = STACK_OK;

    StackInit(&stack, 8 ON_DEBUG(,__FILE__, __func__, __LINE__, GetName(stack)));
    StackPush(&stack, 1);

    *(stack.data) = 0;
    err = StackVerify(&stack);

    ChooseModification(CYAN_TEXT);
    printf("STACK_INVALID_HASH: ");

    if (StackCheckError(err, STACK_INVALID_HASH)) {
        ChooseModification(GREEN_TEXT);
        printf("Catched\n");
    }
    else {
        ChooseModification(RED_TEXT);
        printf("Missed\n");
    }

    ChooseModification(RESET_TEXT);
}

#endif