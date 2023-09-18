#include <stdio.h>
#include <stdint.h>
#include <locale.h>
#include <string.h>
#include <stdlib.h>
#define NOARGC
#ifdef NOARGC
#define TESTFILEPATH "test"
#endif

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

int main(char *argc, char *argv[])
{
    setlocale(LC_ALL, "");
#ifdef NOARGC
    const char filename[] = TESTFILEPATH;
#else
    const char *filename = argv[1];
#endif
    FILE *textfile = fopen(filename, "r");
    char c;
    dynamic_string str;
    dynamic_string_array text;
    initstr(&str);
    inittext(&text);
    while ((c = fgetc(textfile)) != EOF)
    {
        appendchar(&str, c);
        if (c == '\n')
            appendstr(&text, &str);
    }
    appendstr(&text, &str);

    for (uint64_t i = 0; i < text.length; ++i)
    {
        printf("%ld. %s", i + 1, text.text[i]->str);
    }
    fclose(textfile);
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
}
void appendstr(dynamic_string_array *arr, dynamic_string *dstr)
{
    if (arr->length == arr->capacity)
    {
        arr->text = (dynamic_string **)realloc(arr->text, 2u * arr->capacity * sizeof(dynamic_string *));
        arr->capacity *= 2;
    }
    appendchar(dstr, '\0');
    arr->text[arr->length] = (dynamic_string *)malloc(sizeof(dynamic_string));
    arr->text[arr->length]->capacity = dstr->capacity;
    arr->text[arr->length]->length = dstr->length;
    arr->text[arr->length]->str = (char *)malloc(dstr->capacity * sizeof(char));

    memcpy(arr->text[arr->length]->str, dstr->str, (size_t)dstr->length);
    free(dstr->str);
    initstr(dstr);
    arr->length++;
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