#include "nvs_flash.h"
#include "nvs.h"

void init_nvs();

char* get_nvs_value();

void set_nvs_value(char* place);

void close_nvs();