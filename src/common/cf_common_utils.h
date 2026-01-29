#ifndef CF_COMMON_UTILS_H
#define CF_COMMON_UTILS_H

#include "cellframe.h"
#include "cf_callbacks_registry.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    PyObject *callback;
    PyObject *user_data;
} python_callback_ctx_t;

void cf_common_clear_ctx(python_callback_ctx_t **ctx_ptr);

int cf_common_parse_hash_fast(PyObject *obj, dap_hash_fast_t *out);
int cf_common_parse_hash_slow(PyObject *obj, dap_chain_hash_slow_t *out);
int cf_common_parse_addr(PyObject *obj, dap_chain_addr_t *out);
int cf_common_parse_uint256(PyObject *obj, uint256_t *out);
int cf_common_parse_uint128(PyObject *obj, uint128_t *out);

void *cf_common_capsule_get_pointer(PyObject *obj, const char *name, const char *error_msg);

PyObject *cf_common_capsule_new_addr(dap_chain_addr_t *addr);
void cf_common_capsule_addr_destructor(PyObject *capsule);

int cf_common_register_methods(PyObject *module, PyMethodDef *methods);

#ifdef __cplusplus
}
#endif

#endif /* CF_COMMON_UTILS_H */
