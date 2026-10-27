#include <stdio.h>
#include <string.h>
#include <ctype.h>
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

//PUSH
//ADD
//SUB
//MUL
//DIV
//IN
//OUT
//...

const char* ASM_FILE = "asm.txt";
const char* BIN_FILE = "lst.bin";

struct text_line {
    char* start;
    char* end;
};

struct asm_text {
    char* buf;
    size_t size;
    size_t lines_count;
    text_line* lines;
};

cpu_err_t LoadAsmTextFromFile(asm_text* text, const char* name_file);

cpu_err_t SplitText(asm_text* text);

void FreeAsmText(asm_text* text);

cpu_err_t TranslateAsmToBin(asm_text* text, byte_t* code, size_t* code_size);

cpu_err_t PrintBinCodeToFile(byte_t* code, size_t code_size, const char* lst_file);

int main() {
    asm_text text = {0};
    cpu_err_t err = CPU_OK;

    err = LoadAsmTextFromFile(&text, ASM_FILE);
    if (err != CPU_OK){
        CpuShowError(err);
        return 1;
    }

    // for (size_t i = 0; i < text.lines_count; i++)
    //     printf("%s\n", text.lines[i].start);

    byte_t* byte_code = (byte_t*)calloc(text.lines_count, sizeof(byte_t) + sizeof(int));
    size_t code_size = 0;

    err = TranslateAsmToBin(&text, byte_code, &code_size);
    if (err != CPU_OK) {
        CpuShowError(err);
        return 1;
    }

    FreeAsmText(&text);

    err = PrintBinCodeToFile(byte_code, code_size, BIN_FILE);
    if (err != CPU_OK) {
        CpuShowError(err);
        return 1;
    }

    free(byte_code);

    return 0;
}

cpu_err_t LoadAsmTextFromFile(asm_text* text, const char* name_file) { //correct
    assert(text);
    assert(name_file);

    struct stat buf = {0};
    int status = stat(name_file, &buf);
    assert(status != -1);
    text->size = buf.st_size;

    text->buf = (char*)calloc(text->size + 1, sizeof(char));
    if (text->buf == NULL)
        return CPU_OOM;

    FILE* fp = fopen(name_file, "r");
    if (fp == NULL) {
        free(text->buf);
        return CPU_FOPEN;
    }

    text->size = fread(text->buf, sizeof(char), text->size, fp) + 1;
    text->buf[text->size - 1] = '\n';

    cpu_err_t err = SplitText(text);
    if (err != CPU_OK) {
        free(text->buf);
        return CPU_OOM;
    }

    return CPU_OK;
}

cpu_err_t SplitText(asm_text* text) { //correct
    assert(text);

    for (size_t cur_ch = 0; cur_ch < text->size; cur_ch++) {
        if (*(text->buf + cur_ch) == '\n'){
            ++text->lines_count;
            *(text->buf + cur_ch) = '\0';
        }
    }

    text->lines = (text_line*)calloc(text->lines_count, sizeof(text_line));
    if (text->lines == NULL)
        return CPU_OOM;

    size_t cur_ch = 0;
    size_t not_empty_lines = 0;

    //Добавлено убирание пустых строк //good
    for (size_t i = 0; i < text->lines_count; i++) {
        text->lines[not_empty_lines].start = text->buf + cur_ch;
        while (*(text->buf + cur_ch) != '\0')
            ++cur_ch;

        text->lines[not_empty_lines].end = text->buf + cur_ch;
        ++cur_ch;

        if (text->lines[not_empty_lines].start != text->lines[not_empty_lines].end)
            ++not_empty_lines;
    }

    text->lines_count = not_empty_lines;

    return CPU_OK;
}

void FreeAsmText(asm_text* text) {
    assert(text);

    free(text->buf);
    free(text->lines);
}

cpu_err_t TranslateAsmToBin(asm_text* text, byte_t* code, size_t* code_size) {
    assert(text);
    assert(code);
    assert(code_size);

    *code_size = 0;
    int temp_n = 0;
    int arg = 0;
    char temp_s[8] = "";
    cmd_t cmd = UNKNOWN_CMD;

    for (size_t i = 0; i < text->lines_count; i++) {
        cmd = UNKNOWN_CMD;
        sscanf(text->lines[i].start, "%7s", temp_s);

        for (int cmd_ind = 0; cmd_ind < (int)MAX_NUM_CMD; cmd_ind++)
            if (!strncmp(commands[cmd_ind], temp_s, MAX_CMD_LEN)) 
                cmd = (cmd_t)cmd_ind;

        if (cmd == UNKNOWN_CMD) 
            return CPU_UNKNOWN_CMD;

        switch ((int)cmd) {
            case PUSH: //TODO сделать PUSH AX
            {
                sscanf(text->lines[i].start, "%*s %n", &temp_n);

                if (isdigit(text->lines[i].start[temp_n])) { //done
                    sscanf(text->lines[i].start, "%*s %d %n", &arg, &temp_n);
                    if (text->lines[i].start + temp_n != text->lines[i].end) 
                        return CPU_BAD_ARG;

                } else if ('A' <= text->lines[i].start[temp_n] && text->lines[i].start[temp_n] <= 'D') { //done
                    arg = text->lines[i].start[temp_n] - 'A';

                    sscanf(text->lines[i].start, "%*s %*cX %n", &temp_n);
                    if (text->lines[i].start + temp_n != text->lines[i].end) 
                        return CPU_BAD_ARG;

                    cmd = (cmd_t)((int)cmd | (int)REGISTER_BIT);

                } else {
                    return CPU_BAD_ARG;
                } //позже добавить else if (text->lines[i].start[temp_n] == '[') для RAM

                break;
            }
            case POP:
            {
                sscanf(text->lines[i].start, "%*s %n", &temp_n);

                if ('A' <= text->lines[i].start[temp_n] && text->lines[i].start[temp_n] <= 'D') { //done
                    arg = text->lines[i].start[temp_n] - 'A';

                    sscanf(text->lines[i].start, "%*s %*cX %n", &temp_n);
                    if (text->lines[i].start + temp_n != text->lines[i].end) 
                        return CPU_BAD_ARG;

                    cmd = (cmd_t)((int)cmd | (int)REGISTER_BIT);

                } else {
                    return CPU_BAD_ARG;
                }

                break;
            }

            case ADD:
            case SUB:
            case MUL:
            case DIV:
            case IN:
            case OUT:
            case HLT:
            {
                sscanf(text->lines[i].start, "%*s %n", &temp_n); //TODO %n может нехило помочь с комментами

                if (text->lines[i].start + temp_n != text->lines[i].end)
                    return CPU_TOO_MUCH_ARGS;

                break;
            }

            default:
                break;
        }

        printf("%d", (int)cmd);
        code[(*code_size)++] = cmd;

        if ((cmd & OPCODE_BITS) == PUSH || (cmd & OPCODE_BITS) == POP) {
            printf(" %d", arg);
            memcpy(code + *code_size, &arg, sizeof(arg));
            *code_size += 4;
        }

        putchar('\n');
    }

    return CPU_OK;
}

cpu_err_t PrintBinCodeToFile(byte_t* code, size_t code_size, const char* lst_file) {
    assert(code);
    assert(lst_file);

    FILE* fp = fopen(lst_file, "wb");

    if (fp == NULL)
        return CPU_FOPEN;

    fwrite(code, sizeof(byte_t), code_size, fp);

    if (fclose(fp) < 0)
        return CPU_FCLOSE;

    return CPU_OK;
}
