#include <stdlib.h>
#include <stdio.h>
#include "strbuf.h"

#ifndef DEBUG
#define DEBUG 0
#endif

int sb_init(strbuf_t *L, size_t length)
{
    L->data = malloc(sizeof(char) * length);
    if (!L->data) return 1;

    L->data[0] = '\0';

    L->length = length;
    L->used   = 1;

    return 0;
}

int sb_insert(strbuf_t *list, int index, char item)
{
    if(index >= list->length - 1 || list->used == list->length){
        size_t size = list->length * 2;
        if(index >= size){
            size = index + 2;
        }
        char *p = realloc(list->data, sizeof(char) * size);
        if (!p) return 1;
        list->data = p;
        list->length = size;

        if (DEBUG) printf("Increased size to %lu\n", size);

    }

    if(index >= list->used)
        list->used = index + 1;

    char temp1 = list->data[index];
    char temp2;
    list->data[index] = item;
    
    for(int i = index + 1; i < list->length; i++){
        temp2 = list->data[i];
        list->data[i] = temp1;
        temp1 = temp2;
    }
    
    list->data[list->used] = '\0';
    ++list->used;

    return 0;
}

void sb_destroy(strbuf_t *L)
{
    free(L->data);
}

int sb_append(strbuf_t *L, char item)
{
    if (L->used == L->length) {
        size_t size = L->length * 2;
        char *p = realloc(L->data, sizeof(char) * size);
        if (!p) return 1;

        L->data = p;
        L->length = size;

        if (DEBUG) printf("Increased size to %lu\n", size);
    }

    L->data[L->used - 1] = item;
    L->data[L->used] = '\0';
    ++L->used;

    return 0;
}


int sb_remove(strbuf_t *L, char *item)
{
    if (L->used <= 1) return 1;

    --L->used;

    if (item) *item = L->data[L->used];

    L->data[L->used - 1] = '\0';
    return 0;
}

int sb_concat(strbuf_t *sb, char *str)
{
    int i = 0;
    while(str[i] != '\0'){
        if(sb_append(sb, str[i])){
            return 1;
        }
        i++;
    }
    return 0;
    /*
    int strLength = 1;
    char c = str[0];
    while(c != '\0'){
        strLength++;
        c = str[strLength - 1];
    }
    
    if (sb->used == sb->length || sb->length < strLength + (sb->used - 1)) {
        size_t size = sb->length * 2;
        if(strLength + (sb->used - 1) > size){
            size = strLength + (sb->used - 1);
        }
        char *p = realloc(sb->data, sizeof(char) * size);
        if (!p) return 1;

        sb->data = p;
        sb->length = size;

        if (DEBUG) printf("Increased size to %lu\n", size);
    }

    for(int i = 0; i < strLength; i++){
        sb->data[i + (sb->used - 1)] = str[i];
    }
    sb->used = sb->used + strLength - 1;
    return 0;
    */
}

void printList(strbuf_t *list)
{
    int i;
    printf("[%2lu / %2lu] ", list->used, list->length);
    for(i = 0; i < list->used; i++){
        printf("%02x ", (unsigned char) list->data[i]);
    }
    for(; i < list->length; ++i){
        printf("__ ");
    }
    putchar('\n');
    printf("String: %s\n", list->data);
}