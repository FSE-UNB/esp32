#include "parser.h"

char* get_string(cJSON *json, char* attribute) {
    cJSON* content = NULL;
    content = cJSON_GetObjectItemCaseSensitive(json, attribute);
    if (cJSON_IsString(content) && (content->valuestring != NULL)) {
        return content->valuestring;
    }

    return "";
}

float get_float(cJSON *json, char* attribute) {
    cJSON* content = NULL;
    content = cJSON_GetObjectItemCaseSensitive(json, attribute);
    if (cJSON_IsNumber(content)) {
        return content->valuedouble;
    }

    return -1;
}

JSONBody parser_json(char *json) {
    cJSON* config_json;
    JSONBody jsonBody;

    config_json = cJSON_ParseWithLength(json, strlen(json));

    if (config_json == NULL) {
        const char* error_ptr = cJSON_GetErrorPtr();
        if (error_ptr != NULL) {
            fprintf(stderr, "Erro no JSON: %s\n", error_ptr);
        }
        cJSON_Delete(config_json);
    }

    jsonBody.type = get_string(config_json, "type");
    jsonBody.place = get_string(config_json, "place");
    jsonBody.value = get_float(config_json, "value");

    return jsonBody;
}
