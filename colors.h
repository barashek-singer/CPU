#ifndef COLORS_H_INCLUDED

#define COLORS_H_INCLUDED

#include <stdio.h>

enum TextModification { //Модификации текста
    RESET_TEXT        = 0, //Сброс параметров
//---------------Стили текста------------------
    BOLD_TEXT         = 1,
    FAINT_TEXT        = 2,
    ITALIC_TEXT       = 3,
    UNDERLINE_TEXT    = 4,
    SLOW_BLINK_TEXT   = 5,
//---------------Цвета текста------------------
    BLACK_TEXT        = 30,
    RED_TEXT          = 31,
    GREEN_TEXT        = 32,
    YELLOW_TEXT       = 33,
    BLUE_TEXT         = 34,
    PURPLE_TEXT       = 35,
    CYAN_TEXT         = 36,
    WHITE_TEXT        = 37,
//-------------Цвета заднего фона--------------
    BLACK_BACKGROUND  = 40,
    RED_BACKGROUND    = 41,
    GREEN_BACKGROUND  = 42,
    YELLOW_BACKGROUND = 43,
    BLUE_BACKGROUND   = 44,
    PURPLE_BACKGROUND = 45,
    CYAN_BACKGROUND   = 46,
    WHITE_BACKGROUND  = 47
};

void ChooseModification(TextModification text_modification);

void ChooseModification(TextModification text_modification) {
    printf("\x1b[%dm", (int)text_modification);
}

#endif