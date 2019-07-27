#include "key_list.h"

key_list_t *key_list_init()
{
    key_list_t *keyList = (key_list_t *)malloc(sizeof(key_list_t));
    keyList->lenght = 0;
    return keyList;
}

void key_list_free(key_list_t* list){
    for (int i=0; i < list->lenght;i++){
        free(list->keys[i]);
    }
    free(list);
}

uint8_t key_list_add_element(key_list_t *list, dap_enc_key_t* key){
    uint8_t new_len = list->lenght;
    new_len++;
    dap_enc_key_t **new_keys = calloc(new_len, sizeof(dap_enc_key_t));
    if (list->lenght != 0) {
        memcpy(new_keys, list->keys, list->lenght);
        free(list->keys);
    }
    new_keys[new_len - 1] = key;
    list->keys = new_keys;
    list->lenght = new_len;
    return new_len;

}

bool key_list_del_element(key_list_t *list, uint8_t key_id){
    if ((list->lenght-1) > 0) {
        return false;
    }
    uint8_t new_len = list->lenght;
    new_len--;
    dap_enc_key_t **new_keys = calloc(new_len, sizeof(dap_enc_key_t));
    if (list->lenght - 1 == key_id){
        memcpy(new_keys, list->keys, key_id);
    } else if (list->lenght > 1 && key_id < list->lenght) {
        memcpy(new_keys, list->keys, key_id);
        memcpy(new_keys + key_id, list->keys + (key_id + 1), list->lenght);
    }
    free(list->keys);
    list->keys = new_keys;
    list->lenght = new_len;
    return true;
}

dap_enc_key_t *key_list_get_key(key_list_t *list, uint8_t key){
    uint8_t index = key - 1;
    if (list->lenght < index) {
        return NULL;
    }
    return list->keys[index];
}
