#include "dap_common.h"
#include "dap_enc_key.h"
#include <stdint.h>
#include <string.h>

#ifdef __cplusplus
extern "C" {
#endif

#undef LOG_TAG
#define LOG_TAG "key-list"

typedef struct key_list{
    dap_enc_key_t **keys;
    uint8_t lenght;
}key_list_t;

key_list_t *key_list_init(void);
void key_list_free(key_list_t* list);
uint8_t key_list_add_element(key_list_t *list, dap_enc_key_t* key);
bool key_list_del_element(key_list_t *list, uint8_t key);
dap_enc_key_t *key_list_get_key(key_list_t *list, uint8_t key);

#ifdef __cplusplus
}
#endif
