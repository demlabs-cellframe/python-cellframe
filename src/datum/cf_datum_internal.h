#ifndef CF_DATUM_INTERNAL_H
#define CF_DATUM_INTERNAL_H

#include "cellframe.h"

void *cf_datum_get_capsule(PyObject *obj, const char *name, const char *error_msg);
const char *cf_datum_get_cstring(PyObject *obj, const char *error_msg);
int cf_datum_parse_hash_fast(PyObject *obj, dap_hash_fast_t *out);
int cf_datum_parse_uint256(PyObject *obj, uint256_t *out);
int cf_datum_parse_addr(PyObject *obj, dap_chain_addr_t *out);
int cf_datum_parse_node_addr(PyObject *obj, dap_chain_node_addr_t *out);

#endif
