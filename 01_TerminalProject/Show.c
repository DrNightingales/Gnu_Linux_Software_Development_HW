#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <ncurses.h>
#include <locale.h>
#define DX 3
#define TESTFILEPATH "Lorem_Ipsum"

#define WLINES (LINES - 2 * DX)
#define WCOLS (COLS - 2 * DX)
#define min(a, b) \
    ({ __typeof__ (a) _a = (a); \
       __typeof__ (b) _b = (b); \
     _a < _b ? _a : _b; })

typedef struct
{
    uint64_t capacity;
    uint64_t length;
    char *str;
} dynamic_string;

typedef struct
{
    uint64_t capacity;
    uint64_t length;
    dynamic_string **text;
} dynamic_string_array;

void appendchar(dynamic_string *dstr, char c);
void appendstr(dynamic_string_array *arr, dynamic_string *dstr);
void initstr(dynamic_string *dstr);
void inittext(dynamic_string_array *arr);
void printtext(dynamic_string_array *arr);

void readfile(FILE *filestream, dynamic_string_array *text);

void wprint_slice(WINDOW *window, char *str, int start, int stop);
int main(char *argc, char *argv[])
{
    setlocale(LC_ALL, "");
#ifdef NOARGC
    const char filename[] = TESTFILEPATH;
#else
    const char *filename = argv[1];
#endif
    printf("%s", argv[1]);
    FILE *textfile = fopen(filename, "r");
    dynamic_string_array text;
    readfile(textfile, &text);
    fclose(textfile);

    initscr();
    WINDOW *win = newwin(LINES - 2 * DX, COLS - 2 * DX, DX, DX);
    move(1, 1);
    printw(filename);
    refresh();
    noecho();
    cbreak();
    wmove(win, 1, 1);
    keypad(win, TRUE);
    erase();

    int c = 0;
    int offsetX = 0;
    int offsetY = 0;
    int max_length_on_screen = 0;
    box(win, 0, 0);

    do // ESCAPE Key Code
    {

        werase(win);

        wmove(win, 0, 0);
        wprintw(win, "%d %d %d", (int64_t)text.length - offsetY, WLINES, (int64_t)text.length);
        if (c == KEY_RIGHT && max_length_on_screen - offsetX > WCOLS)
            offsetX += 1;
        if (c == KEY_LEFT && offsetX > 0)
            offsetX -= 1;
        if (c == KEY_DOWN && (int64_t)text.length - offsetY > WLINES)
            offsetY += 1;
        if (c == KEY_UP && offsetY > 0)
            offsetY -= 1;

        max_length_on_screen = 0;
        for (int i = offsetY; i < min(WLINES + offsetY, (int64_t)text.length); i++)
        {
            if (text.text[i]->length > max_length_on_screen)
                max_length_on_screen = text.text[i]->length;

            wmove(win, i + 1 - offsetY, 1);

            wprint_slice(win, text.text[i]->str, offsetX, min((int64_t)text.text[i]->length, WCOLS + offsetX));
            // waddch(win, '\n');
            box(win, 0, 0);
            wrefresh(win);
        }
    } while ((c = wgetch(win)) != 27);

    for (int i = 0; i < text.length; ++i)
    {
        free(text.text[i]->str);
        free(text.text[i]);
    }
    free(text.text);
    endwin();
}
void wprint_slice(WINDOW *win, char *str, int start, int stop)
{

    if (start < stop - 2)
    {

        for (int i = start; i < stop - 2; i++)
            waddch(win, str[i]);
    }
    waddch(win, '\n');
}
void readfile(FILE *filestream, dynamic_string_array *ptext)
{
    char c;
    dynamic_string str;
    initstr(&str);
    inittext(ptext);
    while ((c = fgetc(filestream)) != EOF)
    {
        appendchar(&str, c);
        if (c == '\n')
        {

            appendchar(&str, '\0');
            appendstr(ptext, &str);
        }
    }
    appendchar(&str, '\n');
    appendchar(&str, '\0');
    appendstr(ptext, &str);
}
void inittext(dynamic_string_array *arr)
{
    arr->capacity = 1;
    arr->length = 0;
    arr->text = (dynamic_string **)malloc(sizeof(dynamic_string *));
}

void initstr(dynamic_string *dstr)
{
    dstr->capacity = 1;
    dstr->length = 0;
    dstr->str = (char *)malloc(sizeof(char));
    dstr->str[0] = '\0';
}
void appendstr(dynamic_string_array *arr, dynamic_string *dstr)
{
    if (arr->length == arr->capacity)
    {
        arr->text = (dynamic_string **)realloc(arr->text, 2u * arr->capacity * sizeof(dynamic_string *));
        arr->capacity *= 2;
    }
    if (dstr->length > 0)
        appendchar(dstr, '\0');

    arr->text[arr->length] = (dynamic_string *)malloc(sizeof(dynamic_string));
    arr->text[arr->length]->capacity = dstr->capacity;
    arr->text[arr->length]->length = dstr->length;
    arr->text[arr->length]->str = (char *)malloc(dstr->capacity * sizeof(char));

    memcpy(arr->text[arr->length]->str, dstr->str, (size_t)dstr->length);
    free(dstr->str);
    initstr(dstr);
    arr->length += 1;
}
void appendchar(dynamic_string *dstr, char c)
{
    if (dstr->length == dstr->capacity)
    {
        dstr->str = (char *)realloc(dstr->str, 2u * dstr->capacity * sizeof(char));
        dstr->capacity *= 2;
    }
    dstr->str[dstr->length++] = c;
}
void printtext(dynamic_string_array *arr)
{
    for (uint64_t i = 0; i < arr->length; i++)

        printf("%ld. %s", i + 1, arr->text[i]->str);
}
