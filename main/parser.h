#ifndef PARSER_H
#define PARSER_H

#include <cJSON.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

typedef struct {
    char *type;
    char *place;
    float value;
} JSONBody;

JSONBody parser_json(char *json);

#endif
