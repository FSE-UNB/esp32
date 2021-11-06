#include "my_nvs.h"

nvs_handle_t my_handle;
esp_err_t err;

void init_nvs() {
    printf("iniciando nvs\n");
    ESP_ERROR_CHECK( nvs_flash_init() );
    err = nvs_open("storage", NVS_READWRITE, &my_handle);
}

char* get_nvs_value() {
    size_t place_size;
    printf("pegando place size\n");
    err = nvs_get_str(my_handle, "place", NULL, &place_size);
    if (err == ESP_ERR_NVS_NOT_FOUND) {
        printf("nvs not found\n");
        return "";
    }
    printf("pegou place size %d\n", place_size);
    char* place = malloc(place_size);
    printf("pegando place\n");
    err = nvs_get_str(my_handle, "place", place, &place_size);
    printf("pegou place\n");
    return place;
}

void set_nvs_value(char* place) {
    printf("salvando place\n");
    err = nvs_set_str(my_handle, "place", place);
    nvs_commit(my_handle);
}

void erase_nvs() {
    nvs_flash_erase();
}

void close_nvs() {
    printf("fechando nvs\n");
    nvs_close(my_handle);
}