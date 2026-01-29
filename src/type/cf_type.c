#include "cellframe.h"
#include "cf_common_utils.h"

#include "dap_chain_block.h"
#include "dap_chain_block_cache.h"
#include "dap_chain_block_tx.h"
#include "dap_chain_blocks_decree.h"
#include "dap_chain_type_blocks.h"
#include "dap_chain_type_dag.h"
#include "dap_chain_type_dag_event.h"
#include "dap_chain_type_none.h"
#include "dap_chain_datum.h"
#include "dap_chain_datum_tx_in.h"
#include "dap_hash.h"
#include "dap_list.h"
#include "dap_pkey.h"
#include "dap_sign.h"
#include "dap_enc_key.h"

#define LOG_TAG "python_cellframe_type"

int cf_chain_parse_cell_id(PyObject *obj, dap_chain_cell_id_t *out);

// =========================================
// HELPERS
// =========================================

static int cf_type_dict_set(PyObject *dict, const char *key, PyObject *value) {
    if (!dict || !value) {
        Py_XDECREF(value);
        return -1;
    }
    if (PyDict_SetItemString(dict, key, value) < 0) {
        Py_DECREF(value);
        return -1;
    }
    Py_DECREF(value);
    return 0;
}

static int cf_type_extract_bytes(PyObject *obj, const char **out, Py_ssize_t *out_size, const char *label) {
    if (!obj || obj == Py_None) {
        PyErr_Format(PyExc_TypeError, "%s is required", label);
        return -1;
    }
    if (!PyBytes_Check(obj)) {
        PyErr_Format(PyExc_TypeError, "%s must be bytes", label);
        return -1;
    }
    *out = PyBytes_AsString(obj);
    *out_size = PyBytes_Size(obj);
    if (!*out) {
        PyErr_Format(PyExc_ValueError, "Failed to read %s bytes", label);
        return -1;
    }
    return 0;
}

static const char *cf_type_get_optional_str(PyObject *obj, const char *label) {
    if (!obj || obj == Py_None) {
        return NULL;
    }
    if (PyUnicode_Check(obj)) {
        return PyUnicode_AsUTF8(obj);
    }
    if (PyBytes_Check(obj)) {
        return PyBytes_AsString(obj);
    }
    PyErr_Format(PyExc_TypeError, "%s must be str or bytes", label);
    return NULL;
}

static int cf_type_extract_block_bytes(PyObject *obj, const char **out, Py_ssize_t *out_size) {
    if (PyDict_Check(obj)) {
        PyObject *block_obj = PyDict_GetItemString(obj, "block");
        if (!block_obj) {
            PyErr_SetString(PyExc_TypeError, "block dict must contain 'block' bytes");
            return -1;
        }
        return cf_type_extract_bytes(block_obj, out, out_size, "block");
    }
    return cf_type_extract_bytes(obj, out, out_size, "block");
}

static int cf_type_extract_event_bytes(PyObject *obj, const char **out, Py_ssize_t *out_size) {
    if (PyDict_Check(obj)) {
        PyObject *event_obj = PyDict_GetItemString(obj, "event");
        if (!event_obj) {
            PyErr_SetString(PyExc_TypeError, "event dict must contain 'event' bytes");
            return -1;
        }
        return cf_type_extract_bytes(event_obj, out, out_size, "event");
    }
    return cf_type_extract_bytes(obj, out, out_size, "event");
}

static int cf_type_extract_round_item_bytes(PyObject *obj, const char **out, Py_ssize_t *out_size) {
    if (PyDict_Check(obj)) {
        PyObject *round_obj = PyDict_GetItemString(obj, "round_item");
        if (!round_obj) {
            PyErr_SetString(PyExc_TypeError, "round_item dict must contain 'round_item' bytes");
            return -1;
        }
        return cf_type_extract_bytes(round_obj, out, out_size, "round_item");
    }
    return cf_type_extract_bytes(obj, out, out_size, "round_item");
}

static int cf_type_block_size_checked(const char *bytes, size_t input_size, size_t *out_size) {
    if (!bytes || input_size < sizeof(dap_chain_block_hdr_t)) {
        PyErr_SetString(PyExc_ValueError, "block bytes are too small");
        return -1;
    }
    const dap_chain_block_t *block = (const dap_chain_block_t *)bytes;
    size_t size = dap_chain_block_get_size((dap_chain_block_t *)block);
    if (!size || size > input_size) {
        PyErr_SetString(PyExc_ValueError, "block size is invalid or truncated");
        return -1;
    }
    if (out_size) {
        *out_size = size;
    }
    return 0;
}

static int cf_type_event_size_checked(const char *bytes, size_t input_size, size_t *out_size) {
    if (!bytes || input_size < sizeof(dap_chain_class_dag_event_hdr_t)) {
        PyErr_SetString(PyExc_ValueError, "event bytes are too small");
        return -1;
    }
    dap_chain_type_dag_event_t *event = (dap_chain_type_dag_event_t *)bytes;
    uint64_t size = dap_chain_type_dag_event_calc_size(event, (uint64_t)input_size);
    if (!size && event->header.signs_count == 0) {
        size = dap_chain_type_dag_event_calc_size_excl_signs(event, (uint64_t)input_size);
    }
    if (!size || size > input_size) {
        PyErr_SetString(PyExc_ValueError, "event size is invalid or truncated");
        return -1;
    }
    if (out_size) {
        *out_size = (size_t)size;
    }
    return 0;
}

static int cf_type_round_item_size_checked(const char *bytes, size_t input_size, size_t *out_size) {
    if (!bytes || input_size < sizeof(dap_chain_type_dag_event_round_item_t)) {
        PyErr_SetString(PyExc_ValueError, "round_item bytes are too small");
        return -1;
    }
    dap_chain_type_dag_event_round_item_t *item = (dap_chain_type_dag_event_round_item_t *)bytes;
    size_t size = dap_chain_type_dag_event_round_item_get_size(item);
    if (!size || size > input_size) {
        PyErr_SetString(PyExc_ValueError, "round_item size is invalid or truncated");
        return -1;
    }
    if (out_size) {
        *out_size = size;
    }
    return 0;
}

static dap_chain_t *cf_type_get_chain(PyObject *obj) {
    return (dap_chain_t *)cf_common_capsule_get_pointer(obj, "dap_chain_t",
                                                        "Expected dap_chain_t capsule");
}

static dap_chain_net_t *cf_type_get_net(PyObject *obj) {
    return (dap_chain_net_t *)cf_common_capsule_get_pointer(obj, "dap_chain_net_t",
                                                            "Expected dap_chain_net_t capsule");
}

static dap_ledger_t *cf_type_get_ledger(PyObject *obj) {
    return (dap_ledger_t *)cf_common_capsule_get_pointer(obj, "dap_ledger_t",
                                                         "Expected dap_ledger_t capsule");
}

static dap_enc_key_t *cf_type_get_key(PyObject *obj) {
    return (dap_enc_key_t *)cf_common_capsule_get_pointer(obj, "dap_enc_key_t",
                                                          "Expected dap_enc_key_t capsule");
}

static dap_chain_type_blocks_t *cf_type_get_blocks(PyObject *obj) {
    dap_chain_t *chain = cf_type_get_chain(obj);
    if (!chain) {
        return NULL;
    }
    dap_chain_type_blocks_t *blocks = DAP_CHAIN_TYPE_BLOCKS(chain);
    if (!blocks) {
        PyErr_SetString(PyExc_ValueError, "Chain does not use blocks type");
        return NULL;
    }
    return blocks;
}

static dap_chain_type_dag_t *cf_type_get_dag(PyObject *obj) {
    dap_chain_t *chain = cf_type_get_chain(obj);
    if (!chain) {
        return NULL;
    }
    dap_chain_type_dag_t *dag = DAP_CHAIN_TYPE_DAG(chain);
    if (!dag) {
        PyErr_SetString(PyExc_ValueError, "Chain does not use dag type");
        return NULL;
    }
    return dag;
}

static int cf_type_parse_pkey(PyObject *obj, dap_pkey_t **out, bool *needs_free) {
    if (!out || !needs_free) {
        PyErr_SetString(PyExc_RuntimeError, "Invalid output pointers for pkey parser");
        return -1;
    }
    *out = NULL;
    *needs_free = false;
    if (!obj || obj == Py_None) {
        PyErr_SetString(PyExc_TypeError, "pkey is required");
        return -1;
    }
    if (PyCapsule_CheckExact(obj)) {
        dap_pkey_t *ptr = (dap_pkey_t *)PyCapsule_GetPointer(obj, "dap_pkey_t");
        if (!ptr) {
            PyErr_SetString(PyExc_ValueError, "Invalid dap_pkey_t capsule");
            return -1;
        }
        *out = ptr;
        return 0;
    }
    if (PyBytes_Check(obj)) {
        Py_ssize_t size = PyBytes_Size(obj);
        if (size < (Py_ssize_t)sizeof(dap_pkey_t)) {
            PyErr_SetString(PyExc_ValueError, "pkey bytes are too small");
            return -1;
        }
        dap_pkey_t *ptr = (dap_pkey_t *)PyBytes_AsString(obj);
        size_t expected = dap_pkey_get_size(ptr);
        if (!expected || expected != (size_t)size) {
            PyErr_SetString(PyExc_ValueError, "pkey bytes have invalid size");
            return -1;
        }
        *out = ptr;
        return 0;
    }
    if (PyUnicode_Check(obj)) {
        const char *pkey_str = PyUnicode_AsUTF8(obj);
        if (!pkey_str) {
            return -1;
        }
        dap_pkey_t *ptr = dap_pkey_get_from_str(pkey_str);
        if (!ptr) {
            PyErr_SetString(PyExc_ValueError, "Invalid pkey string");
            return -1;
        }
        *out = ptr;
        *needs_free = true;
        return 0;
    }
    PyErr_SetString(PyExc_TypeError, "pkey must be bytes, str, or dap_pkey_t capsule");
    return -1;
}

static void cf_type_free_list_item(void *item) {
    DAP_DELETE(item);
}

static void cf_type_free_fees_list(dap_ledger_hardfork_fees_t *fees) {
    while (fees) {
        dap_ledger_hardfork_fees_t *next = fees->next;
        DAP_DELETE(fees);
        fees = next;
    }
}

static dap_list_t *cf_type_build_hash_list(PyObject *obj) {
    if (!obj || obj == Py_None) {
        PyErr_SetString(PyExc_TypeError, "hash list is required");
        return NULL;
    }
    PyObject *seq = PySequence_Fast(obj, "hash list must be a sequence");
    if (!seq) {
        return NULL;
    }
    Py_ssize_t count = PySequence_Fast_GET_SIZE(seq);
    if (count <= 0) {
        Py_DECREF(seq);
        PyErr_SetString(PyExc_ValueError, "hash list must not be empty");
        return NULL;
    }
    dap_list_t *list = NULL;
    for (Py_ssize_t i = 0; i < count; i++) {
        PyObject *item = PySequence_Fast_GET_ITEM(seq, i);
        dap_hash_fast_t hash = {};
        if (cf_common_parse_hash_fast(item, &hash) != 0) {
            dap_list_free_full(list, cf_type_free_list_item);
            Py_DECREF(seq);
            return NULL;
        }
        dap_hash_fast_t *hash_ptr = DAP_NEW_Z(dap_hash_fast_t);
        if (!hash_ptr) {
            dap_list_free_full(list, cf_type_free_list_item);
            Py_DECREF(seq);
            PyErr_SetString(PyExc_MemoryError, "Failed to allocate hash item");
            return NULL;
        }
        *hash_ptr = hash;
        list = dap_list_append(list, hash_ptr);
    }
    Py_DECREF(seq);
    return list;
}

static int cf_type_hash_array_from_py(PyObject *obj, dap_chain_hash_fast_t **out, size_t *out_count) {
    if (!out || !out_count) {
        PyErr_SetString(PyExc_RuntimeError, "Invalid output pointers for hash array parser");
        return -1;
    }
    *out = NULL;
    *out_count = 0;
    if (!obj || obj == Py_None) {
        return 0;
    }
    PyObject *seq = PySequence_Fast(obj, "hashes must be a sequence");
    if (!seq) {
        return -1;
    }
    Py_ssize_t count = PySequence_Fast_GET_SIZE(seq);
    if (count <= 0) {
        Py_DECREF(seq);
        return 0;
    }
    dap_chain_hash_fast_t *hashes = DAP_NEW_Z_COUNT(dap_chain_hash_fast_t, (size_t)count);
    if (!hashes) {
        Py_DECREF(seq);
        PyErr_SetString(PyExc_MemoryError, "Failed to allocate hashes array");
        return -1;
    }
    for (Py_ssize_t i = 0; i < count; i++) {
        PyObject *item = PySequence_Fast_GET_ITEM(seq, i);
        if (cf_common_parse_hash_fast(item, &hashes[i]) != 0) {
            DAP_DELETE(hashes);
            Py_DECREF(seq);
            return -1;
        }
    }
    Py_DECREF(seq);
    *out = hashes;
    *out_count = (size_t)count;
    return 0;
}

static int cf_type_block_meta_find(const dap_chain_block_t *block, size_t block_size,
                                   uint8_t meta_type, const uint8_t **data, size_t *data_size) {
    if (!block || block_size < sizeof(block->hdr)) {
        PyErr_SetString(PyExc_ValueError, "block is corrupted");
        return -1;
    }
    if (block->hdr.meta_count == 0) {
        return 0;
    }
    size_t offset = 0;
    const uint8_t *found_data = NULL;
    size_t found_size = 0;
    bool found = false;
    for (size_t i = 0; i < block->hdr.meta_count; i++) {
        if (offset + sizeof(dap_chain_block_meta_t) > block_size - sizeof(block->hdr)) {
            PyErr_SetString(PyExc_ValueError, "block meta section is corrupted");
            return -1;
        }
        dap_chain_block_meta_t *meta = (dap_chain_block_meta_t *)(block->meta_n_datum_n_sign + offset);
        size_t meta_total = sizeof(meta->hdr) + meta->hdr.data_size;
        if (offset + meta_total > block_size - sizeof(block->hdr)) {
            PyErr_SetString(PyExc_ValueError, "block meta data is corrupted");
            return -1;
        }
        if (meta->hdr.type == meta_type) {
            found = true;
            found_data = meta->data;
            found_size = meta->hdr.data_size;
        }
        offset += meta_total;
    }
    if (found) {
        if (data) {
            *data = found_data;
        }
        if (data_size) {
            *data_size = found_size;
        }
        return 1;
    }
    return 0;
}

static int cf_type_block_get_datum_offset(const dap_chain_block_t *block, size_t block_size, size_t *out_offset) {
    if (!block || block_size < sizeof(block->hdr)) {
        PyErr_SetString(PyExc_ValueError, "block is corrupted");
        return -1;
    }
    size_t offset = 0;
    for (size_t i = 0; i < block->hdr.meta_count; i++) {
        if (sizeof(block->hdr) + offset + sizeof(dap_chain_block_meta_t) > block_size) {
            PyErr_SetString(PyExc_ValueError, "block meta section is corrupted");
            return -1;
        }
        dap_chain_block_meta_t *meta = (dap_chain_block_meta_t *)(block->meta_n_datum_n_sign + offset);
        size_t meta_total = sizeof(meta->hdr) + meta->hdr.data_size;
        if (sizeof(block->hdr) + offset + meta_total > block_size) {
            PyErr_SetString(PyExc_ValueError, "block meta data is corrupted");
            return -1;
        }
        if (offset + meta_total < offset) {
            PyErr_SetString(PyExc_ValueError, "block meta section size overflow");
            return -1;
        }
        offset += meta_total;
    }
    if (sizeof(block->hdr) + offset > block_size) {
        PyErr_SetString(PyExc_ValueError, "block meta section is corrupted");
        return -1;
    }
    if (out_offset) {
        *out_offset = offset;
    }
    return 0;
}

static bool cf_type_sign_matches_pkey(const dap_pkey_t *pkey, const dap_sign_t *sign) {
    if (!pkey || !sign) {
        return false;
    }
    if (dap_pkey_compare_with_sign((dap_pkey_t *)pkey, (dap_sign_t *)sign)) {
        return true;
    }
    if (sign->header.sign_pkey_size == DAP_HASH_FAST_SIZE) {
        dap_chain_hash_fast_t pkey_hash = {};
        if (!dap_pkey_get_hash((dap_pkey_t *)pkey, &pkey_hash)) {
            return false;
        }
        return memcmp(sign->pkey_n_sign, &pkey_hash, DAP_HASH_FAST_SIZE) == 0;
    }
    return false;
}

static bool cf_type_block_sign_match_pkey(const dap_chain_block_t *block, size_t block_size, dap_pkey_t *pkey) {
    if (!block || !pkey || block_size < sizeof(block->hdr)) {
        return false;
    }
    size_t offset = dap_chain_block_get_sign_offset(block, block_size);
    if (sizeof(block->hdr) + offset > block_size) {
        return false;
    }
    while (sizeof(block->hdr) + offset + sizeof(dap_sign_t) <= block_size) {
        dap_sign_t *sign = (dap_sign_t *)(block->meta_n_datum_n_sign + offset);
        size_t sign_size = dap_sign_get_size(sign);
        if (!sign_size) {
            return false;
        }
        if (sizeof(block->hdr) + offset + sign_size > block_size) {
            return false;
        }
        if (cf_type_sign_matches_pkey(pkey, sign)) {
            return true;
        }
        if (offset + sign_size < offset) {
            return false;
        }
        offset += sign_size;
    }
    return false;
}

static PyObject *cf_type_hash_list_from_array(const dap_chain_hash_fast_t *hashes, size_t count) {
    PyObject *list = PyList_New((Py_ssize_t)count);
    if (!list) {
        return NULL;
    }
    for (size_t i = 0; i < count; i++) {
        PyObject *item = PyBytes_FromStringAndSize((const char *)&hashes[i], sizeof(dap_chain_hash_fast_t));
        if (!item) {
            Py_DECREF(list);
            return NULL;
        }
        PyList_SET_ITEM(list, (Py_ssize_t)i, item);
    }
    return list;
}

static PyObject *cf_type_block_cache_to_dict(const dap_chain_block_cache_t *cache) {
    if (!cache) {
        PyErr_SetString(PyExc_ValueError, "block cache is NULL");
        return NULL;
    }
    PyObject *dict = PyDict_New();
    if (!dict) {
        return NULL;
    }

    if (cf_type_dict_set(dict, "block_hash",
                         PyBytes_FromStringAndSize((const char *)&cache->block_hash, sizeof(dap_chain_hash_fast_t))) < 0) {
        Py_DECREF(dict);
        return NULL;
    }
    if (cache->block_hash_str[0] != '\0') {
        if (cf_type_dict_set(dict, "block_hash_str", PyUnicode_FromString(cache->block_hash_str)) < 0) {
            Py_DECREF(dict);
            return NULL;
        }
    } else {
        Py_INCREF(Py_None);
        if (PyDict_SetItemString(dict, "block_hash_str", Py_None) < 0) {
            Py_DECREF(Py_None);
            Py_DECREF(dict);
            return NULL;
        }
        Py_DECREF(Py_None);
    }

    if (cf_type_dict_set(dict, "block_size", PyLong_FromSize_t(cache->block_size)) < 0) {
        Py_DECREF(dict);
        return NULL;
    }
    if (cf_type_dict_set(dict, "block_number", PyLong_FromUnsignedLongLong(cache->block_number)) < 0) {
        Py_DECREF(dict);
        return NULL;
    }
    if (cf_type_dict_set(dict, "ts_created", PyLong_FromLongLong((long long)cache->ts_created)) < 0) {
        Py_DECREF(dict);
        return NULL;
    }

    PyObject *block_bytes = NULL;
    if (cache->block && cache->block_size) {
        block_bytes = PyBytes_FromStringAndSize((const char *)cache->block, (Py_ssize_t)cache->block_size);
        if (!block_bytes) {
            Py_DECREF(dict);
            return NULL;
        }
    } else {
        Py_INCREF(Py_None);
        block_bytes = Py_None;
    }
    if (cf_type_dict_set(dict, "block", block_bytes) < 0) {
        Py_DECREF(dict);
        return NULL;
    }

    if (cf_type_dict_set(dict, "datum_count", PyLong_FromSize_t(cache->datum_count)) < 0) {
        Py_DECREF(dict);
        return NULL;
    }

    PyObject *datums_list = PyList_New((Py_ssize_t)cache->datum_count);
    if (!datums_list) {
        Py_DECREF(dict);
        return NULL;
    }
    for (size_t i = 0; i < cache->datum_count; i++) {
        dap_chain_datum_t *datum = cache->datum ? cache->datum[i] : NULL;
        if (!datum) {
            Py_DECREF(datums_list);
            Py_DECREF(dict);
            PyErr_SetString(PyExc_ValueError, "block cache datum is NULL");
            return NULL;
        }
        size_t datum_size = dap_chain_datum_size(datum);
        if (!datum_size) {
            Py_DECREF(datums_list);
            Py_DECREF(dict);
            PyErr_SetString(PyExc_ValueError, "block cache datum size is invalid");
            return NULL;
        }
        PyObject *datum_bytes = PyBytes_FromStringAndSize((const char *)datum, (Py_ssize_t)datum_size);
        if (!datum_bytes) {
            Py_DECREF(datums_list);
            Py_DECREF(dict);
            return NULL;
        }
        PyList_SET_ITEM(datums_list, (Py_ssize_t)i, datum_bytes);
    }
    if (cf_type_dict_set(dict, "datums", datums_list) < 0) {
        Py_DECREF(dict);
        return NULL;
    }

    if (cache->datum_count > 0 && !cache->datum_hash) {
        Py_DECREF(dict);
        PyErr_SetString(PyExc_ValueError, "block cache datum hashes are missing");
        return NULL;
    }
    PyObject *datum_hashes = cf_type_hash_list_from_array(cache->datum_hash, cache->datum_count);
    if (!datum_hashes) {
        Py_DECREF(dict);
        return NULL;
    }
    if (cf_type_dict_set(dict, "datum_hashes", datum_hashes) < 0) {
        Py_DECREF(dict);
        return NULL;
    }

    if (cf_type_dict_set(dict, "prev_hash",
                         PyBytes_FromStringAndSize((const char *)&cache->prev_hash, sizeof(dap_chain_hash_fast_t))) < 0) {
        Py_DECREF(dict);
        return NULL;
    }
    if (cf_type_dict_set(dict, "anchor_hash",
                         PyBytes_FromStringAndSize((const char *)&cache->anchor_hash, sizeof(dap_chain_hash_fast_t))) < 0) {
        Py_DECREF(dict);
        return NULL;
    }
    if (cf_type_dict_set(dict, "merkle_root",
                         PyBytes_FromStringAndSize((const char *)&cache->merkle_root, sizeof(dap_chain_hash_fast_t))) < 0) {
        Py_DECREF(dict);
        return NULL;
    }

    if (cache->links_hash_count > 0 && !cache->links_hash) {
        Py_DECREF(dict);
        PyErr_SetString(PyExc_ValueError, "block cache links hashes are missing");
        return NULL;
    }
    PyObject *links_hashes = cf_type_hash_list_from_array(cache->links_hash, cache->links_hash_count);
    if (!links_hashes) {
        Py_DECREF(dict);
        return NULL;
    }
    if (cf_type_dict_set(dict, "links_hashes", links_hashes) < 0) {
        Py_DECREF(dict);
        return NULL;
    }
    if (cf_type_dict_set(dict, "links_hash_count", PyLong_FromSize_t(cache->links_hash_count)) < 0) {
        Py_DECREF(dict);
        return NULL;
    }

    if (cf_type_dict_set(dict, "nonce", PyLong_FromUnsignedLongLong(cache->nonce)) < 0) {
        Py_DECREF(dict);
        return NULL;
    }
    if (cf_type_dict_set(dict, "nonce2", PyLong_FromUnsignedLongLong(cache->nonce2)) < 0) {
        Py_DECREF(dict);
        return NULL;
    }
    if (cf_type_dict_set(dict, "is_genesis", PyBool_FromLong(cache->is_genesis ? 1 : 0)) < 0) {
        Py_DECREF(dict);
        return NULL;
    }
    if (cf_type_dict_set(dict, "is_blockgen", PyBool_FromLong(cache->is_blockgen ? 1 : 0)) < 0) {
        Py_DECREF(dict);
        return NULL;
    }
    if (cf_type_dict_set(dict, "generation", PyLong_FromUnsignedLong(cache->generation)) < 0) {
        Py_DECREF(dict);
        return NULL;
    }
    if (cf_type_dict_set(dict, "sign_count", PyLong_FromSize_t(cache->sign_count)) < 0) {
        Py_DECREF(dict);
        return NULL;
    }

    return dict;
}

static PyObject *cf_type_used_out_item_to_dict(const dap_chain_tx_used_out_item_t *item) {
    PyObject *dict = PyDict_New();
    if (!dict) {
        return NULL;
    }
    if (cf_type_dict_set(dict, "tx_hash",
                         PyBytes_FromStringAndSize((const char *)&item->tx_hash_fast, sizeof(dap_chain_hash_fast_t))) < 0) {
        Py_DECREF(dict);
        return NULL;
    }
    if (cf_type_dict_set(dict, "num_idx_out", PyLong_FromUnsignedLong(item->num_idx_out)) < 0) {
        Py_DECREF(dict);
        return NULL;
    }
    if (cf_type_dict_set(dict, "value",
                         PyBytes_FromStringAndSize((const char *)&item->value, sizeof(uint256_t))) < 0) {
        Py_DECREF(dict);
        return NULL;
    }
    return dict;
}

// =========================================
// BLOCK OPERATIONS
// =========================================

static PyObject *dap_chain_block_init_py(PyObject *self, PyObject *args) {
    (void)self;
    (void)args;
    int rc = dap_chain_block_init();
    return PyLong_FromLong(rc);
}

static PyObject *dap_chain_block_deinit_py(PyObject *self, PyObject *args) {
    (void)self;
    (void)args;
    dap_chain_block_deinit();
    Py_RETURN_NONE;
}

static PyObject *dap_chain_block_new_py(PyObject *self, PyObject *args) {
    (void)self;
    PyObject *prev_hash_obj = Py_None;
    if (!PyArg_ParseTuple(args, "|O", &prev_hash_obj)) {
        return NULL;
    }
    dap_hash_fast_t prev_hash = {};
    dap_hash_fast_t *prev_hash_ptr = NULL;
    if (prev_hash_obj && prev_hash_obj != Py_None) {
        if (cf_common_parse_hash_fast(prev_hash_obj, &prev_hash) != 0) {
            return NULL;
        }
        prev_hash_ptr = &prev_hash;
    }
    size_t block_size = 0;
    dap_chain_block_t *block = dap_chain_block_new(prev_hash_ptr, &block_size);
    if (!block || !block_size) {
        PyErr_SetString(PyExc_RuntimeError, "Failed to create block");
        return NULL;
    }
    PyObject *bytes = PyBytes_FromStringAndSize((const char *)block, (Py_ssize_t)block_size);
    DAP_DELETE(block);
    return bytes;
}

static PyObject *dap_chain_block_get_size_py(PyObject *self, PyObject *args) {
    (void)self;
    PyObject *block_obj = NULL;
    if (!PyArg_ParseTuple(args, "O", &block_obj)) {
        return NULL;
    }
    const char *block_bytes = NULL;
    Py_ssize_t block_size_in = 0;
    if (cf_type_extract_block_bytes(block_obj, &block_bytes, &block_size_in) != 0) {
        return NULL;
    }
    if ((size_t)block_size_in < sizeof(dap_chain_block_hdr_t)) {
        PyErr_SetString(PyExc_ValueError, "block bytes are too small");
        return NULL;
    }
    dap_chain_block_t *block = (dap_chain_block_t *)block_bytes;
    size_t size = dap_chain_block_get_size(block);
    if (!size || size > (size_t)block_size_in) {
        PyErr_SetString(PyExc_ValueError, "block size is invalid or truncated");
        return NULL;
    }
    return PyLong_FromSize_t(size);
}

static PyObject *dap_chain_block_meta_add_py(PyObject *self, PyObject *args) {
    (void)self;
    PyObject *block_obj = NULL;
    PyObject *data_obj = Py_None;
    unsigned int meta_type = 0;
    if (!PyArg_ParseTuple(args, "OI|O", &block_obj, &meta_type, &data_obj)) {
        return NULL;
    }
    const char *block_bytes = NULL;
    Py_ssize_t block_size_in = 0;
    if (cf_type_extract_block_bytes(block_obj, &block_bytes, &block_size_in) != 0) {
        return NULL;
    }
    size_t block_size = 0;
    if (cf_type_block_size_checked(block_bytes, (size_t)block_size_in, &block_size) != 0) {
        return NULL;
    }
    const char *data_bytes = NULL;
    Py_ssize_t data_size = 0;
    if (data_obj && data_obj != Py_None) {
        if (cf_type_extract_bytes(data_obj, &data_bytes, &data_size, "meta_data") != 0) {
            return NULL;
        }
    }
    dap_chain_block_t *block = DAP_DUP_SIZE(block_bytes, block_size);
    if (!block) {
        PyErr_SetString(PyExc_MemoryError, "Failed to allocate block copy");
        return NULL;
    }
    size_t new_size = dap_chain_block_meta_add(&block, block_size, (uint8_t)meta_type, data_bytes, (size_t)data_size);
    if (!new_size || new_size <= block_size) {
        DAP_DELETE(block);
        PyErr_SetString(PyExc_RuntimeError, "Failed to add block meta");
        return NULL;
    }
    PyObject *bytes = PyBytes_FromStringAndSize((const char *)block, (Py_ssize_t)new_size);
    DAP_DELETE(block);
    return bytes;
}

static PyObject *dap_chain_block_meta_get_py(PyObject *self, PyObject *args) {
    (void)self;
    PyObject *block_obj = NULL;
    unsigned int meta_type = 0;
    if (!PyArg_ParseTuple(args, "OI", &block_obj, &meta_type)) {
        return NULL;
    }
    const char *block_bytes = NULL;
    Py_ssize_t block_size_in = 0;
    if (cf_type_extract_block_bytes(block_obj, &block_bytes, &block_size_in) != 0) {
        return NULL;
    }
    size_t block_size = 0;
    if (cf_type_block_size_checked(block_bytes, (size_t)block_size_in, &block_size) != 0) {
        return NULL;
    }
    const uint8_t *data = NULL;
    size_t data_size = 0;
    int found = cf_type_block_meta_find((const dap_chain_block_t *)block_bytes, block_size,
                                        (uint8_t)meta_type, &data, &data_size);
    if (found < 0) {
        return NULL;
    }
    if (found == 0) {
        Py_RETURN_NONE;
    }

    switch (meta_type) {
    case DAP_CHAIN_BLOCK_META_GENESIS:
    case DAP_CHAIN_BLOCK_META_EMERGENCY:
    case DAP_CHAIN_BLOCK_META_BLOCKGEN:
        if (data_size == 0) {
            Py_RETURN_TRUE;
        }
        Py_RETURN_NONE;
    case DAP_CHAIN_BLOCK_META_PREV:
    case DAP_CHAIN_BLOCK_META_ANCHOR:
    case DAP_CHAIN_BLOCK_META_LINK:
    case DAP_CHAIN_BLOCK_META_MERKLE:
        if (data_size != sizeof(dap_chain_hash_fast_t)) {
            Py_RETURN_NONE;
        }
        return PyBytes_FromStringAndSize((const char *)data, sizeof(dap_chain_hash_fast_t));
    case DAP_CHAIN_BLOCK_META_NONCE:
    case DAP_CHAIN_BLOCK_META_NONCE2:
    case DAP_CHAIN_BLOCK_META_SYNC_ATTEMPT:
        if (data_size != sizeof(uint64_t)) {
            Py_RETURN_NONE;
        }
        return PyLong_FromUnsignedLongLong(*(uint64_t *)data);
    case DAP_CHAIN_BLOCK_META_ROUND_ATTEMPT:
        if (data_size != sizeof(uint8_t)) {
            Py_RETURN_NONE;
        }
        return PyLong_FromUnsignedLong((unsigned long)(*(uint8_t *)data));
    case DAP_CHAIN_BLOCK_META_GENERATION:
        if (data_size != sizeof(uint16_t)) {
            Py_RETURN_NONE;
        }
        return PyLong_FromUnsignedLong((unsigned long)(*(uint16_t *)data));
    case DAP_CHAIN_BLOCK_META_EXCLUDED_KEYS: {
        if (data_size < sizeof(uint16_t)) {
            Py_RETURN_NONE;
        }
        uint16_t count = *(uint16_t *)data;
        size_t expected = ((size_t)count + 1) * sizeof(uint16_t);
        if (expected != data_size) {
            Py_RETURN_NONE;
        }
        PyObject *list = PyList_New((Py_ssize_t)count);
        if (!list) {
            return NULL;
        }
        const uint16_t *vals = (const uint16_t *)data;
        for (uint16_t i = 0; i < count; i++) {
            PyObject *val = PyLong_FromUnsignedLong(vals[i + 1]);
            if (!val) {
                Py_DECREF(list);
                return NULL;
            }
            PyList_SET_ITEM(list, (Py_ssize_t)i, val);
        }
        return list;
    }
    case DAP_CHAIN_BLOCK_META_EVM_DATA:
        if (data_size == 0) {
            Py_RETURN_NONE;
        }
        return PyBytes_FromStringAndSize((const char *)data, (Py_ssize_t)data_size);
    default:
        Py_RETURN_NONE;
    }
}

static PyObject *dap_chain_block_meta_extract_py(PyObject *self, PyObject *args) {
    (void)self;
    PyObject *block_obj = NULL;
    if (!PyArg_ParseTuple(args, "O", &block_obj)) {
        return NULL;
    }
    const char *block_bytes = NULL;
    Py_ssize_t block_size_in = 0;
    if (cf_type_extract_block_bytes(block_obj, &block_bytes, &block_size_in) != 0) {
        return NULL;
    }
    size_t block_size = 0;
    if (cf_type_block_size_checked(block_bytes, (size_t)block_size_in, &block_size) != 0) {
        return NULL;
    }

    dap_chain_hash_fast_t prev_hash = {};
    dap_chain_hash_fast_t anchor_hash = {};
    dap_chain_hash_fast_t merkle = {};
    dap_chain_hash_fast_t *links = NULL;
    size_t links_count = 0;
    bool is_genesis = false;
    bool blockgen = false;
    uint64_t nonce = 0;
    uint64_t nonce2 = 0;
    uint16_t generation = 0;

    int rc = dap_chain_block_meta_extract((dap_chain_block_t *)block_bytes, block_size,
                                          &prev_hash, &anchor_hash, &merkle,
                                          &links, &links_count, &is_genesis,
                                          &nonce, &nonce2, &generation, &blockgen);

    PyObject *dict = PyDict_New();
    if (!dict) {
        DAP_DELETE(links);
        return NULL;
    }
    if (cf_type_dict_set(dict, "rc", PyLong_FromLong(rc)) < 0) {
        Py_DECREF(dict);
        DAP_DELETE(links);
        return NULL;
    }
    if (cf_type_dict_set(dict, "prev_hash",
                         PyBytes_FromStringAndSize((const char *)&prev_hash, sizeof(dap_chain_hash_fast_t))) < 0) {
        Py_DECREF(dict);
        DAP_DELETE(links);
        return NULL;
    }
    if (cf_type_dict_set(dict, "anchor_hash",
                         PyBytes_FromStringAndSize((const char *)&anchor_hash, sizeof(dap_chain_hash_fast_t))) < 0) {
        Py_DECREF(dict);
        DAP_DELETE(links);
        return NULL;
    }
    if (cf_type_dict_set(dict, "merkle_root",
                         PyBytes_FromStringAndSize((const char *)&merkle, sizeof(dap_chain_hash_fast_t))) < 0) {
        Py_DECREF(dict);
        DAP_DELETE(links);
        return NULL;
    }
    PyObject *links_list = cf_type_hash_list_from_array(links, links_count);
    if (!links_list) {
        Py_DECREF(dict);
        DAP_DELETE(links);
        return NULL;
    }
    if (cf_type_dict_set(dict, "links", links_list) < 0) {
        Py_DECREF(dict);
        DAP_DELETE(links);
        return NULL;
    }
    if (cf_type_dict_set(dict, "is_genesis", PyBool_FromLong(is_genesis ? 1 : 0)) < 0) {
        Py_DECREF(dict);
        DAP_DELETE(links);
        return NULL;
    }
    if (cf_type_dict_set(dict, "nonce", PyLong_FromUnsignedLongLong(nonce)) < 0) {
        Py_DECREF(dict);
        DAP_DELETE(links);
        return NULL;
    }
    if (cf_type_dict_set(dict, "nonce2", PyLong_FromUnsignedLongLong(nonce2)) < 0) {
        Py_DECREF(dict);
        DAP_DELETE(links);
        return NULL;
    }
    if (cf_type_dict_set(dict, "generation", PyLong_FromUnsignedLong(generation)) < 0) {
        Py_DECREF(dict);
        DAP_DELETE(links);
        return NULL;
    }
    if (cf_type_dict_set(dict, "blockgen", PyBool_FromLong(blockgen ? 1 : 0)) < 0) {
        Py_DECREF(dict);
        DAP_DELETE(links);
        return NULL;
    }

    DAP_DELETE(links);
    return dict;
}

static PyObject *dap_chain_block_datum_add_py(PyObject *self, PyObject *args) {
    (void)self;
    PyObject *block_obj = NULL;
    PyObject *datum_obj = NULL;
    if (!PyArg_ParseTuple(args, "OO", &block_obj, &datum_obj)) {
        return NULL;
    }
    const char *block_bytes = NULL;
    Py_ssize_t block_size_in = 0;
    if (cf_type_extract_block_bytes(block_obj, &block_bytes, &block_size_in) != 0) {
        return NULL;
    }
    size_t block_size = 0;
    if (cf_type_block_size_checked(block_bytes, (size_t)block_size_in, &block_size) != 0) {
        return NULL;
    }
    const char *datum_bytes = NULL;
    Py_ssize_t datum_size_in = 0;
    if (cf_type_extract_bytes(datum_obj, &datum_bytes, &datum_size_in, "datum") != 0) {
        return NULL;
    }
    if ((size_t)datum_size_in < sizeof(dap_chain_datum_t)) {
        PyErr_SetString(PyExc_ValueError, "datum bytes are too small");
        return NULL;
    }
    dap_chain_datum_t *datum = (dap_chain_datum_t *)datum_bytes;
    size_t datum_size = dap_chain_datum_size(datum);
    if (!datum_size || datum_size != (size_t)datum_size_in) {
        PyErr_SetString(PyExc_ValueError, "datum size is invalid");
        return NULL;
    }
    dap_chain_block_t *block = DAP_DUP_SIZE(block_bytes, block_size);
    if (!block) {
        PyErr_SetString(PyExc_MemoryError, "Failed to allocate block copy");
        return NULL;
    }
    size_t new_size = dap_chain_block_datum_add(&block, block_size, datum, datum_size);
    if (new_size <= block_size) {
        DAP_DELETE(block);
        PyErr_SetString(PyExc_RuntimeError, "Failed to add datum to block");
        return NULL;
    }
    PyObject *bytes = PyBytes_FromStringAndSize((const char *)block, (Py_ssize_t)new_size);
    DAP_DELETE(block);
    return bytes;
}

static PyObject *dap_chain_block_datum_del_by_hash_py(PyObject *self, PyObject *args) {
    (void)self;
    PyObject *block_obj = NULL;
    PyObject *hash_obj = NULL;
    if (!PyArg_ParseTuple(args, "OO", &block_obj, &hash_obj)) {
        return NULL;
    }
    const char *block_bytes = NULL;
    Py_ssize_t block_size_in = 0;
    if (cf_type_extract_block_bytes(block_obj, &block_bytes, &block_size_in) != 0) {
        return NULL;
    }
    size_t block_size = 0;
    if (cf_type_block_size_checked(block_bytes, (size_t)block_size_in, &block_size) != 0) {
        return NULL;
    }
    dap_hash_fast_t hash = {};
    if (cf_common_parse_hash_fast(hash_obj, &hash) != 0) {
        return NULL;
    }
    dap_chain_block_t *block = DAP_DUP_SIZE(block_bytes, block_size);
    if (!block) {
        PyErr_SetString(PyExc_MemoryError, "Failed to allocate block copy");
        return NULL;
    }
    size_t offset = 0;
    if (cf_type_block_get_datum_offset(block, block_size, &offset) != 0) {
        DAP_DELETE(block);
        return NULL;
    }
    size_t current_size = block_size;
    while (block->hdr.datum_count > 0 && sizeof(block->hdr) + offset < current_size) {
        if (sizeof(block->hdr) + offset + sizeof(dap_chain_datum_t) > current_size) {
            DAP_DELETE(block);
            PyErr_SetString(PyExc_ValueError, "block datum section is corrupted");
            return NULL;
        }
        dap_chain_datum_t *datum = (dap_chain_datum_t *)(block->meta_n_datum_n_sign + offset);
        size_t datum_size = dap_chain_datum_size(datum);
        if (!datum_size) {
            DAP_DELETE(block);
            PyErr_SetString(PyExc_ValueError, "datum size is invalid");
            return NULL;
        }
        if (sizeof(block->hdr) + offset + datum_size > current_size) {
            DAP_DELETE(block);
            PyErr_SetString(PyExc_ValueError, "block datum section is corrupted");
            return NULL;
        }
        dap_hash_fast_t datum_hash = {};
        dap_chain_datum_calc_hash(datum, &datum_hash);
        if (dap_hash_fast_compare(&datum_hash, &hash)) {
            size_t tail_size = current_size - sizeof(block->hdr) - offset - datum_size;
            memmove(datum, ((uint8_t *)datum) + datum_size, tail_size);
            current_size -= datum_size;
            if (block->hdr.meta_n_datum_n_signs_size < datum_size) {
                DAP_DELETE(block);
                PyErr_SetString(PyExc_ValueError, "block meta size underflow");
                return NULL;
            }
            block->hdr.meta_n_datum_n_signs_size -= (uint32_t)datum_size;
            block->hdr.datum_count--;
            continue;
        }
        offset += datum_size;
    }
    PyObject *bytes = PyBytes_FromStringAndSize((const char *)block, (Py_ssize_t)current_size);
    DAP_DELETE(block);
    return bytes;
}

static PyObject *dap_chain_block_get_datums_py(PyObject *self, PyObject *args) {
    (void)self;
    PyObject *block_obj = NULL;
    if (!PyArg_ParseTuple(args, "O", &block_obj)) {
        return NULL;
    }
    const char *block_bytes = NULL;
    Py_ssize_t block_size_in = 0;
    if (cf_type_extract_block_bytes(block_obj, &block_bytes, &block_size_in) != 0) {
        return NULL;
    }
    size_t block_size = 0;
    if (cf_type_block_size_checked(block_bytes, (size_t)block_size_in, &block_size) != 0) {
        return NULL;
    }
    size_t datums_count = 0;
    dap_chain_datum_t **datums = dap_chain_block_get_datums((dap_chain_block_t *)block_bytes, block_size, &datums_count);
    if (!datums || datums_count == 0) {
        if (datums) {
            DAP_DELETE(datums);
        }
        return PyList_New(0);
    }
    PyObject *list = PyList_New((Py_ssize_t)datums_count);
    if (!list) {
        DAP_DELETE(datums);
        return NULL;
    }
    for (size_t i = 0; i < datums_count; i++) {
        dap_chain_datum_t *datum = datums[i];
        size_t datum_size = dap_chain_datum_size(datum);
        if (!datum_size) {
            Py_DECREF(list);
            DAP_DELETE(datums);
            PyErr_SetString(PyExc_ValueError, "datum size is invalid");
            return NULL;
        }
        PyObject *datum_bytes = PyBytes_FromStringAndSize((const char *)datum, (Py_ssize_t)datum_size);
        if (!datum_bytes) {
            Py_DECREF(list);
            DAP_DELETE(datums);
            return NULL;
        }
        PyList_SET_ITEM(list, (Py_ssize_t)i, datum_bytes);
    }
    DAP_DELETE(datums);
    return list;
}

static PyObject *dap_chain_block_sign_add_py(PyObject *self, PyObject *args) {
    (void)self;
    PyObject *block_obj = NULL;
    PyObject *key_obj = NULL;
    if (!PyArg_ParseTuple(args, "OO", &block_obj, &key_obj)) {
        return NULL;
    }
    const char *block_bytes = NULL;
    Py_ssize_t block_size_in = 0;
    if (cf_type_extract_block_bytes(block_obj, &block_bytes, &block_size_in) != 0) {
        return NULL;
    }
    size_t block_size = 0;
    if (cf_type_block_size_checked(block_bytes, (size_t)block_size_in, &block_size) != 0) {
        return NULL;
    }
    dap_enc_key_t *key = cf_type_get_key(key_obj);
    if (!key) {
        return NULL;
    }
    dap_chain_block_t *block = DAP_DUP_SIZE(block_bytes, block_size);
    if (!block) {
        PyErr_SetString(PyExc_MemoryError, "Failed to allocate block copy");
        return NULL;
    }
    size_t new_size = dap_chain_block_sign_add(&block, block_size, key);
    if (!new_size || new_size <= block_size) {
        DAP_DELETE(block);
        PyErr_SetString(PyExc_RuntimeError, "Failed to add block sign");
        return NULL;
    }
    size_t meta_size = new_size - sizeof(block->hdr);
    if (meta_size > UINT32_MAX) {
        DAP_DELETE(block);
        PyErr_SetString(PyExc_ValueError, "block size is too large");
        return NULL;
    }
    block->hdr.meta_n_datum_n_signs_size = (uint32_t)meta_size;
    PyObject *bytes = PyBytes_FromStringAndSize((const char *)block, (Py_ssize_t)new_size);
    DAP_DELETE(block);
    return bytes;
}

static PyObject *dap_chain_block_sign_get_py(PyObject *self, PyObject *args) {
    (void)self;
    PyObject *block_obj = NULL;
    unsigned int sign_num = 0;
    if (!PyArg_ParseTuple(args, "OI", &block_obj, &sign_num)) {
        return NULL;
    }
    const char *block_bytes = NULL;
    Py_ssize_t block_size_in = 0;
    if (cf_type_extract_block_bytes(block_obj, &block_bytes, &block_size_in) != 0) {
        return NULL;
    }
    size_t block_size = 0;
    if (cf_type_block_size_checked(block_bytes, (size_t)block_size_in, &block_size) != 0) {
        return NULL;
    }
    size_t sign_count = dap_chain_block_get_signs_count((const dap_chain_block_t *)block_bytes, block_size);
    if ((size_t)sign_num >= sign_count) {
        Py_RETURN_NONE;
    }
    dap_sign_t *sign = dap_chain_block_sign_get((const dap_chain_block_t *)block_bytes, block_size, (uint16_t)sign_num);
    if (!sign) {
        Py_RETURN_NONE;
    }
    size_t sign_size = dap_sign_get_size(sign);
    if (!sign_size) {
        PyErr_SetString(PyExc_ValueError, "sign size is invalid");
        return NULL;
    }
    const uint8_t *block_base = (const uint8_t *)block_bytes;
    const uint8_t *sign_base = (const uint8_t *)sign;
    if (sign_base < block_base || sign_base + sign_size > block_base + block_size) {
        PyErr_SetString(PyExc_ValueError, "sign is outside block bounds");
        return NULL;
    }
    return PyBytes_FromStringAndSize((const char *)sign, (Py_ssize_t)sign_size);
}

static PyObject *dap_chain_block_sign_match_pkey_py(PyObject *self, PyObject *args) {
    (void)self;
    PyObject *block_obj = NULL;
    PyObject *pkey_obj = NULL;
    if (!PyArg_ParseTuple(args, "OO", &block_obj, &pkey_obj)) {
        return NULL;
    }
    const char *block_bytes = NULL;
    Py_ssize_t block_size_in = 0;
    if (cf_type_extract_block_bytes(block_obj, &block_bytes, &block_size_in) != 0) {
        return NULL;
    }
    size_t block_size = 0;
    if (cf_type_block_size_checked(block_bytes, (size_t)block_size_in, &block_size) != 0) {
        return NULL;
    }
    dap_pkey_t *pkey = NULL;
    bool needs_free = false;
    if (cf_type_parse_pkey(pkey_obj, &pkey, &needs_free) != 0) {
        return NULL;
    }
    bool match = cf_type_block_sign_match_pkey((const dap_chain_block_t *)block_bytes, block_size, pkey);
    if (needs_free) {
        DAP_DELETE(pkey);
    }
    return PyBool_FromLong(match ? 1 : 0);
}

static PyObject *dap_chain_block_get_signs_count_py(PyObject *self, PyObject *args) {
    (void)self;
    PyObject *block_obj = NULL;
    if (!PyArg_ParseTuple(args, "O", &block_obj)) {
        return NULL;
    }
    const char *block_bytes = NULL;
    Py_ssize_t block_size_in = 0;
    if (cf_type_extract_block_bytes(block_obj, &block_bytes, &block_size_in) != 0) {
        return NULL;
    }
    size_t block_size = 0;
    if (cf_type_block_size_checked(block_bytes, (size_t)block_size_in, &block_size) != 0) {
        return NULL;
    }
    size_t count = dap_chain_block_get_signs_count((const dap_chain_block_t *)block_bytes, block_size);
    return PyLong_FromSize_t(count);
}

static PyObject *dap_chain_block_get_sign_offset_py(PyObject *self, PyObject *args) {
    (void)self;
    PyObject *block_obj = NULL;
    if (!PyArg_ParseTuple(args, "O", &block_obj)) {
        return NULL;
    }
    const char *block_bytes = NULL;
    Py_ssize_t block_size_in = 0;
    if (cf_type_extract_block_bytes(block_obj, &block_bytes, &block_size_in) != 0) {
        return NULL;
    }
    size_t block_size = 0;
    if (cf_type_block_size_checked(block_bytes, (size_t)block_size_in, &block_size) != 0) {
        return NULL;
    }
    size_t offset = dap_chain_block_get_sign_offset((const dap_chain_block_t *)block_bytes, block_size);
    return PyLong_FromSize_t(offset);
}

static PyObject *dap_chain_block_get_prev_hash_py(PyObject *self, PyObject *args) {
    (void)self;
    PyObject *block_obj = NULL;
    if (!PyArg_ParseTuple(args, "O", &block_obj)) {
        return NULL;
    }
    const char *block_bytes = NULL;
    Py_ssize_t block_size_in = 0;
    if (cf_type_extract_block_bytes(block_obj, &block_bytes, &block_size_in) != 0) {
        return NULL;
    }
    size_t block_size = 0;
    if (cf_type_block_size_checked(block_bytes, (size_t)block_size_in, &block_size) != 0) {
        return NULL;
    }
    const uint8_t *data = NULL;
    size_t data_size = 0;
    int found_prev = cf_type_block_meta_find((const dap_chain_block_t *)block_bytes, block_size,
                                             DAP_CHAIN_BLOCK_META_PREV, &data, &data_size);
    if (found_prev < 0) {
        return NULL;
    }
    if (found_prev == 1 && data_size == sizeof(dap_chain_hash_fast_t)) {
        return PyBytes_FromStringAndSize((const char *)data, sizeof(dap_chain_hash_fast_t));
    }
    int found_genesis = cf_type_block_meta_find((const dap_chain_block_t *)block_bytes, block_size,
                                                DAP_CHAIN_BLOCK_META_GENESIS, NULL, NULL);
    if (found_genesis < 0) {
        return NULL;
    }
    Py_RETURN_NONE;
}

// =========================================
// BLOCK CACHE OPERATIONS
// =========================================

static PyObject *dap_chain_block_cache_init_py(PyObject *self, PyObject *args) {
    (void)self;
    (void)args;
    int rc = dap_chain_block_cache_init();
    return PyLong_FromLong(rc);
}

static PyObject *dap_chain_block_cache_deinit_py(PyObject *self, PyObject *args) {
    (void)self;
    (void)args;
    dap_chain_block_cache_deinit();
    Py_RETURN_NONE;
}

static PyObject *dap_chain_block_cache_new_py(PyObject *self, PyObject *args) {
    (void)self;
    PyObject *block_obj = NULL;
    PyObject *hash_obj = Py_None;
    unsigned long long block_number = 0;
    if (!PyArg_ParseTuple(args, "O|OK", &block_obj, &hash_obj, &block_number)) {
        return NULL;
    }
    const char *block_bytes = NULL;
    Py_ssize_t block_size_in = 0;
    if (cf_type_extract_block_bytes(block_obj, &block_bytes, &block_size_in) != 0) {
        return NULL;
    }
    size_t block_size = 0;
    if (cf_type_block_size_checked(block_bytes, (size_t)block_size_in, &block_size) != 0) {
        return NULL;
    }
    dap_hash_fast_t hash = {};
    dap_hash_fast_t *hash_ptr = NULL;
    if (hash_obj && hash_obj != Py_None) {
        if (cf_common_parse_hash_fast(hash_obj, &hash) != 0) {
            return NULL;
        }
        hash_ptr = &hash;
    }
    dap_chain_block_t *block_copy = DAP_DUP_SIZE(block_bytes, block_size);
    if (!block_copy) {
        PyErr_SetString(PyExc_MemoryError, "Failed to allocate block copy");
        return NULL;
    }
    dap_chain_block_cache_t *cache = dap_chain_block_cache_new(hash_ptr, block_copy, block_size,
                                                               (uint64_t)block_number, false);
    if (!cache) {
        DAP_DELETE(block_copy);
        PyErr_SetString(PyExc_RuntimeError, "Failed to create block cache");
        return NULL;
    }
    PyObject *dict = cf_type_block_cache_to_dict(cache);
    dap_chain_block_cache_delete(cache);
    DAP_DELETE(block_copy);
    return dict;
}

static PyObject *dap_chain_block_cache_dup_py(PyObject *self, PyObject *args) {
    (void)self;
    PyObject *block_obj = NULL;
    PyObject *hash_obj = Py_None;
    unsigned long long block_number = 0;
    if (!PyArg_ParseTuple(args, "O|OK", &block_obj, &hash_obj, &block_number)) {
        return NULL;
    }
    const char *block_bytes = NULL;
    Py_ssize_t block_size_in = 0;
    if (cf_type_extract_block_bytes(block_obj, &block_bytes, &block_size_in) != 0) {
        return NULL;
    }
    size_t block_size = 0;
    if (cf_type_block_size_checked(block_bytes, (size_t)block_size_in, &block_size) != 0) {
        return NULL;
    }
    dap_hash_fast_t hash = {};
    dap_hash_fast_t *hash_ptr = NULL;
    if (hash_obj && hash_obj != Py_None) {
        if (cf_common_parse_hash_fast(hash_obj, &hash) != 0) {
            return NULL;
        }
        hash_ptr = &hash;
    }
    dap_chain_block_t *block_copy = DAP_DUP_SIZE(block_bytes, block_size);
    if (!block_copy) {
        PyErr_SetString(PyExc_MemoryError, "Failed to allocate block copy");
        return NULL;
    }
    dap_chain_block_cache_t *cache = dap_chain_block_cache_new(hash_ptr, block_copy, block_size,
                                                               (uint64_t)block_number, false);
    if (!cache) {
        DAP_DELETE(block_copy);
        PyErr_SetString(PyExc_RuntimeError, "Failed to create block cache");
        return NULL;
    }
    dap_chain_block_cache_t *dup_cache = dap_chain_block_cache_dup(cache);
    if (!dup_cache) {
        dap_chain_block_cache_delete(cache);
        DAP_DELETE(block_copy);
        PyErr_SetString(PyExc_RuntimeError, "Failed to duplicate block cache");
        return NULL;
    }
    PyObject *dict = cf_type_block_cache_to_dict(dup_cache);
    DAP_DELETE(dup_cache);
    dap_chain_block_cache_delete(cache);
    DAP_DELETE(block_copy);
    return dict;
}

static PyObject *dap_chain_block_cache_update_py(PyObject *self, PyObject *args) {
    (void)self;
    PyObject *block_obj = NULL;
    PyObject *hash_obj = Py_None;
    if (!PyArg_ParseTuple(args, "O|O", &block_obj, &hash_obj)) {
        return NULL;
    }
    const char *block_bytes = NULL;
    Py_ssize_t block_size_in = 0;
    if (cf_type_extract_block_bytes(block_obj, &block_bytes, &block_size_in) != 0) {
        return NULL;
    }
    size_t block_size = 0;
    if (cf_type_block_size_checked(block_bytes, (size_t)block_size_in, &block_size) != 0) {
        return NULL;
    }
    dap_hash_fast_t hash = {};
    dap_hash_fast_t *hash_ptr = NULL;
    if (hash_obj && hash_obj != Py_None) {
        if (cf_common_parse_hash_fast(hash_obj, &hash) != 0) {
            return NULL;
        }
        hash_ptr = &hash;
    }
    dap_chain_block_t *block_copy = DAP_DUP_SIZE(block_bytes, block_size);
    if (!block_copy) {
        PyErr_SetString(PyExc_MemoryError, "Failed to allocate block copy");
        return NULL;
    }
    dap_chain_block_cache_t *cache = DAP_NEW_Z(dap_chain_block_cache_t);
    if (!cache) {
        DAP_DELETE(block_copy);
        PyErr_SetString(PyExc_MemoryError, "Failed to allocate block cache");
        return NULL;
    }
    cache->block = block_copy;
    cache->block_size = block_size;
    cache->block_number = 0;
    cache->ts_created = block_copy->hdr.ts_created;
    cache->sign_count = dap_chain_block_get_signs_count(block_copy, block_size);

    int rc = dap_chain_block_cache_update(cache, hash_ptr);
    if (rc != 0) {
        PyObject *dict = PyDict_New();
        if (dict) {
            cf_type_dict_set(dict, "rc", PyLong_FromLong(rc));
        }
        cache->datum = NULL;
        dap_chain_block_cache_delete(cache);
        DAP_DELETE(block_copy);
        return dict;
    }

    PyObject *dict = cf_type_block_cache_to_dict(cache);
    if (dict && cf_type_dict_set(dict, "rc", PyLong_FromLong(rc)) < 0) {
        Py_DECREF(dict);
        dict = NULL;
    }
    dap_chain_block_cache_delete(cache);
    DAP_DELETE(block_copy);
    return dict;
}

static PyObject *dap_chain_block_cache_delete_py(PyObject *self, PyObject *args) {
    (void)self;
    PyObject *block_obj = NULL;
    PyObject *hash_obj = Py_None;
    unsigned long long block_number = 0;
    if (!PyArg_ParseTuple(args, "O|OK", &block_obj, &hash_obj, &block_number)) {
        return NULL;
    }
    const char *block_bytes = NULL;
    Py_ssize_t block_size_in = 0;
    if (cf_type_extract_block_bytes(block_obj, &block_bytes, &block_size_in) != 0) {
        return NULL;
    }
    size_t block_size = 0;
    if (cf_type_block_size_checked(block_bytes, (size_t)block_size_in, &block_size) != 0) {
        return NULL;
    }
    dap_hash_fast_t hash = {};
    dap_hash_fast_t *hash_ptr = NULL;
    if (hash_obj && hash_obj != Py_None) {
        if (cf_common_parse_hash_fast(hash_obj, &hash) != 0) {
            return NULL;
        }
        hash_ptr = &hash;
    }
    dap_chain_block_t *block_copy = DAP_DUP_SIZE(block_bytes, block_size);
    if (!block_copy) {
        PyErr_SetString(PyExc_MemoryError, "Failed to allocate block copy");
        return NULL;
    }
    dap_chain_block_cache_t *cache = dap_chain_block_cache_new(hash_ptr, block_copy, block_size,
                                                               (uint64_t)block_number, false);
    if (!cache) {
        DAP_DELETE(block_copy);
        PyErr_SetString(PyExc_RuntimeError, "Failed to create block cache");
        return NULL;
    }
    dap_chain_block_cache_delete(cache);
    DAP_DELETE(block_copy);
    Py_RETURN_NONE;
}

static PyObject *dap_chain_block_get_list_tx_cond_outs_with_val_py(PyObject *self, PyObject *args) {
    (void)self;
    PyObject *ledger_obj = NULL;
    PyObject *block_obj = NULL;
    if (!PyArg_ParseTuple(args, "OO", &ledger_obj, &block_obj)) {
        return NULL;
    }
    dap_ledger_t *ledger = cf_type_get_ledger(ledger_obj);
    if (!ledger) {
        return NULL;
    }
    const char *block_bytes = NULL;
    Py_ssize_t block_size_in = 0;
    if (cf_type_extract_block_bytes(block_obj, &block_bytes, &block_size_in) != 0) {
        return NULL;
    }
    size_t block_size = 0;
    if (cf_type_block_size_checked(block_bytes, (size_t)block_size_in, &block_size) != 0) {
        return NULL;
    }
    dap_chain_block_t *block_copy = DAP_DUP_SIZE(block_bytes, block_size);
    if (!block_copy) {
        PyErr_SetString(PyExc_MemoryError, "Failed to allocate block copy");
        return NULL;
    }
    dap_chain_block_cache_t *cache = dap_chain_block_cache_new(NULL, block_copy, block_size, 0, false);
    if (!cache) {
        DAP_DELETE(block_copy);
        PyErr_SetString(PyExc_RuntimeError, "Failed to create block cache");
        return NULL;
    }
    uint256_t value_out = {};
    dap_list_t *list = dap_chain_block_get_list_tx_cond_outs_with_val(ledger, cache, &value_out);

    PyObject *py_list = PyList_New(0);
    if (!py_list) {
        dap_list_free_full(list, cf_type_free_list_item);
        dap_chain_block_cache_delete(cache);
        DAP_DELETE(block_copy);
        return NULL;
    }

    if (list) {
        for (dap_list_t *it = list; it; it = it->next) {
            dap_chain_tx_used_out_item_t *item = (dap_chain_tx_used_out_item_t *)it->data;
            PyObject *dict = cf_type_used_out_item_to_dict(item);
            if (!dict) {
                Py_DECREF(py_list);
                dap_list_free_full(list, cf_type_free_list_item);
                dap_chain_block_cache_delete(cache);
                DAP_DELETE(block_copy);
                return NULL;
            }
            if (PyList_Append(py_list, dict) < 0) {
                Py_DECREF(dict);
                Py_DECREF(py_list);
                dap_list_free_full(list, cf_type_free_list_item);
                dap_chain_block_cache_delete(cache);
                DAP_DELETE(block_copy);
                return NULL;
            }
            Py_DECREF(dict);
        }
    }

    dap_list_free_full(list, cf_type_free_list_item);
    dap_chain_block_cache_delete(cache);
    DAP_DELETE(block_copy);

    PyObject *value_bytes = PyBytes_FromStringAndSize((const char *)&value_out, sizeof(uint256_t));
    if (!value_bytes) {
        Py_DECREF(py_list);
        return NULL;
    }
    PyObject *tuple = PyTuple_New(2);
    if (!tuple) {
        Py_DECREF(py_list);
        Py_DECREF(value_bytes);
        return NULL;
    }
    PyTuple_SET_ITEM(tuple, 0, py_list);
    PyTuple_SET_ITEM(tuple, 1, value_bytes);
    return tuple;
}

// =========================================
// BLOCK TX OPERATIONS
// =========================================

static PyObject *dap_chain_block_tx_coll_fee_create_py(PyObject *self, PyObject *args) {
    (void)self;
    PyObject *chain_obj = NULL;
    PyObject *key_obj = NULL;
    PyObject *addr_obj = NULL;
    PyObject *block_list_obj = NULL;
    PyObject *ledger_obj = NULL;
    PyObject *fee_obj = NULL;
    PyObject *hash_out_type_obj = Py_None;
    const char *native_ticker = NULL;
    unsigned long long net_id = 0;

    if (!PyArg_ParseTuple(args, "OOOOOsKO|O", &chain_obj, &key_obj, &addr_obj, &block_list_obj,
                          &ledger_obj, &native_ticker, &net_id, &fee_obj, &hash_out_type_obj)) {
        return NULL;
    }

    dap_chain_type_blocks_t *blocks = cf_type_get_blocks(chain_obj);
    if (!blocks) {
        return NULL;
    }
    dap_enc_key_t *key = cf_type_get_key(key_obj);
    if (!key) {
        return NULL;
    }
    dap_chain_addr_t addr = {};
    if (cf_common_parse_addr(addr_obj, &addr) != 0) {
        return NULL;
    }
    dap_list_t *block_list = cf_type_build_hash_list(block_list_obj);
    if (!block_list) {
        return NULL;
    }
    dap_ledger_t *ledger = cf_type_get_ledger(ledger_obj);
    if (!ledger) {
        dap_list_free_full(block_list, cf_type_free_list_item);
        return NULL;
    }
    uint256_t fee = {};
    if (cf_common_parse_uint256(fee_obj, &fee) != 0) {
        dap_list_free_full(block_list, cf_type_free_list_item);
        return NULL;
    }
    const char *hash_out_type = cf_type_get_optional_str(hash_out_type_obj, "hash_out_type");
    if (!hash_out_type && PyErr_Occurred()) {
        dap_list_free_full(block_list, cf_type_free_list_item);
        return NULL;
    }
    dap_chain_net_id_t net_id_struct = {.uint64 = net_id};

    char *tx_hash = dap_chain_block_tx_coll_fee_create(blocks, key, &addr, block_list,
                                                       ledger, native_ticker, net_id_struct,
                                                       fee, hash_out_type);
    dap_list_free_full(block_list, cf_type_free_list_item);
    if (!tx_hash) {
        PyErr_SetString(PyExc_RuntimeError, "Failed to create fee collection transaction");
        return NULL;
    }
    PyObject *result = PyUnicode_FromString(tx_hash);
    DAP_DELETE(tx_hash);
    return result;
}

static PyObject *dap_chain_block_tx_reward_create_py(PyObject *self, PyObject *args) {
    (void)self;
    PyObject *chain_obj = NULL;
    PyObject *key_obj = NULL;
    PyObject *addr_obj = NULL;
    PyObject *block_list_obj = NULL;
    PyObject *ledger_obj = NULL;
    PyObject *fee_obj = NULL;
    PyObject *hash_out_type_obj = Py_None;
    const char *native_ticker = NULL;
    unsigned long long net_id = 0;

    if (!PyArg_ParseTuple(args, "OOOOOsKO|O", &chain_obj, &key_obj, &addr_obj, &block_list_obj,
                          &ledger_obj, &native_ticker, &net_id, &fee_obj, &hash_out_type_obj)) {
        return NULL;
    }

    dap_chain_type_blocks_t *blocks = cf_type_get_blocks(chain_obj);
    if (!blocks) {
        return NULL;
    }
    dap_enc_key_t *key = cf_type_get_key(key_obj);
    if (!key) {
        return NULL;
    }
    dap_chain_addr_t addr = {};
    if (cf_common_parse_addr(addr_obj, &addr) != 0) {
        return NULL;
    }
    dap_list_t *block_list = cf_type_build_hash_list(block_list_obj);
    if (!block_list) {
        return NULL;
    }
    dap_ledger_t *ledger = cf_type_get_ledger(ledger_obj);
    if (!ledger) {
        dap_list_free_full(block_list, cf_type_free_list_item);
        return NULL;
    }
    uint256_t fee = {};
    if (cf_common_parse_uint256(fee_obj, &fee) != 0) {
        dap_list_free_full(block_list, cf_type_free_list_item);
        return NULL;
    }
    const char *hash_out_type = cf_type_get_optional_str(hash_out_type_obj, "hash_out_type");
    if (!hash_out_type && PyErr_Occurred()) {
        dap_list_free_full(block_list, cf_type_free_list_item);
        return NULL;
    }
    dap_chain_net_id_t net_id_struct = {.uint64 = net_id};

    char *tx_hash = dap_chain_block_tx_reward_create(blocks, key, &addr, block_list,
                                                     ledger, native_ticker, net_id_struct,
                                                     fee, hash_out_type);
    dap_list_free_full(block_list, cf_type_free_list_item);
    if (!tx_hash) {
        PyErr_SetString(PyExc_RuntimeError, "Failed to create reward collection transaction");
        return NULL;
    }
    PyObject *result = PyUnicode_FromString(tx_hash);
    DAP_DELETE(tx_hash);
    return result;
}

static PyObject *dap_chain_block_tx_coll_fee_stack_create_py(PyObject *self, PyObject *args) {
    (void)self;
    PyObject *chain_obj = NULL;
    PyObject *key_obj = NULL;
    PyObject *addr_obj = NULL;
    PyObject *ledger_obj = NULL;
    PyObject *fee_obj = NULL;
    PyObject *hash_out_type_obj = Py_None;
    const char *native_ticker = NULL;
    unsigned long long net_id = 0;

    if (!PyArg_ParseTuple(args, "OOOOsKO|O", &chain_obj, &key_obj, &addr_obj,
                          &ledger_obj, &native_ticker, &net_id, &fee_obj, &hash_out_type_obj)) {
        return NULL;
    }

    dap_chain_type_blocks_t *blocks = cf_type_get_blocks(chain_obj);
    if (!blocks) {
        return NULL;
    }
    dap_enc_key_t *key = cf_type_get_key(key_obj);
    if (!key) {
        return NULL;
    }
    dap_chain_addr_t addr = {};
    if (cf_common_parse_addr(addr_obj, &addr) != 0) {
        return NULL;
    }
    dap_ledger_t *ledger = cf_type_get_ledger(ledger_obj);
    if (!ledger) {
        return NULL;
    }
    uint256_t fee = {};
    if (cf_common_parse_uint256(fee_obj, &fee) != 0) {
        return NULL;
    }
    const char *hash_out_type = cf_type_get_optional_str(hash_out_type_obj, "hash_out_type");
    if (!hash_out_type && PyErr_Occurred()) {
        return NULL;
    }
    dap_chain_net_id_t net_id_struct = {.uint64 = net_id};

    char *tx_hash = dap_chain_block_tx_coll_fee_stack_create(blocks, key, &addr,
                                                             ledger, native_ticker, net_id_struct,
                                                             fee, hash_out_type);
    if (!tx_hash) {
        PyErr_SetString(PyExc_RuntimeError, "Failed to create stacked fee collection transaction");
        return NULL;
    }
    PyObject *result = PyUnicode_FromString(tx_hash);
    DAP_DELETE(tx_hash);
    return result;
}

// =========================================
// BLOCKS DECREE OPERATIONS
// =========================================

static PyObject *dap_chain_blocks_decree_init_py(PyObject *self, PyObject *args) {
    (void)self;
    (void)args;
    dap_chain_blocks_decree_init();
    Py_RETURN_NONE;
}

static PyObject *dap_chain_blocks_decree_deinit_py(PyObject *self, PyObject *args) {
    (void)self;
    (void)args;
    dap_chain_blocks_decree_deinit();
    Py_RETURN_NONE;
}

// =========================================
// TYPE BLOCKS OPERATIONS
// =========================================

typedef struct {
    PyObject *callback;
    PyObject *user_data;
} cf_type_fork_ctx_t;

static void cf_type_fork_resolved_cb(dap_chain_t *chain, dap_hash_fast_t block_before_fork_hash,
                                    dap_list_t *reverted_blocks, uint64_t reverted_blocks_cnt,
                                    uint64_t main_blocks_cnt, void *arg) {
    cf_type_fork_ctx_t *ctx = (cf_type_fork_ctx_t *)arg;
    if (!ctx || !ctx->callback) {
        return;
    }

    PyGILState_STATE gstate = PyGILState_Ensure();

    PyObject *py_chain = PyCapsule_New(chain, "dap_chain_t", NULL);
    PyObject *py_before_hash = PyBytes_FromStringAndSize((const char *)&block_before_fork_hash,
                                                         sizeof(dap_hash_fast_t));
    PyObject *py_reverted = PyList_New(0);
    PyObject *py_reverted_cnt = PyLong_FromUnsignedLongLong(reverted_blocks_cnt);
    PyObject *py_main_cnt = PyLong_FromUnsignedLongLong(main_blocks_cnt);

    if (!py_chain || !py_before_hash || !py_reverted || !py_reverted_cnt || !py_main_cnt) {
        Py_XDECREF(py_chain);
        Py_XDECREF(py_before_hash);
        Py_XDECREF(py_reverted);
        Py_XDECREF(py_reverted_cnt);
        Py_XDECREF(py_main_cnt);
        PyGILState_Release(gstate);
        return;
    }

    bool list_ok = true;
    for (dap_list_t *it = reverted_blocks; it; it = it->next) {
        dap_hash_fast_t *hash = (dap_hash_fast_t *)it->data;
        if (!hash) {
            continue;
        }
        PyObject *hash_bytes = PyBytes_FromStringAndSize((const char *)hash, sizeof(dap_hash_fast_t));
        if (!hash_bytes) {
            list_ok = false;
            break;
        }
        if (PyList_Append(py_reverted, hash_bytes) < 0) {
            Py_DECREF(hash_bytes);
            list_ok = false;
            break;
        }
        Py_DECREF(hash_bytes);
    }

    if (!list_ok) {
        Py_DECREF(py_chain);
        Py_DECREF(py_before_hash);
        Py_DECREF(py_reverted);
        Py_DECREF(py_reverted_cnt);
        Py_DECREF(py_main_cnt);
        PyGILState_Release(gstate);
        return;
    }

    PyObject *result = PyObject_CallFunctionObjArgs(ctx->callback, py_chain, py_before_hash,
                                                    py_reverted, py_reverted_cnt, py_main_cnt,
                                                    ctx->user_data, NULL);
    if (!result) {
        PyErr_Print();
    } else {
        Py_DECREF(result);
    }

    Py_DECREF(py_chain);
    Py_DECREF(py_before_hash);
    Py_DECREF(py_reverted);
    Py_DECREF(py_reverted_cnt);
    Py_DECREF(py_main_cnt);
    PyGILState_Release(gstate);
}

static PyObject *dap_chain_block_add_fork_notificator_py(PyObject *self, PyObject *args) {
    (void)self;
    PyObject *callback = NULL;
    PyObject *user_data = Py_None;
    if (!PyArg_ParseTuple(args, "O|O", &callback, &user_data)) {
        return NULL;
    }
    if (!PyCallable_Check(callback)) {
        PyErr_SetString(PyExc_TypeError, "callback must be callable");
        return NULL;
    }
    cf_type_fork_ctx_t *ctx = DAP_NEW_Z(cf_type_fork_ctx_t);
    if (!ctx) {
        PyErr_SetString(PyExc_MemoryError, "Failed to allocate fork callback context");
        return NULL;
    }
    ctx->callback = callback;
    ctx->user_data = user_data;
    Py_INCREF(callback);
    Py_INCREF(user_data);

    int rc = dap_chain_block_add_fork_notificator(cf_type_fork_resolved_cb, ctx);
    if (rc != 0) {
        Py_DECREF(callback);
        Py_DECREF(user_data);
        DAP_DELETE(ctx);
        PyErr_SetString(PyExc_RuntimeError, "Failed to register fork callback");
        return NULL;
    }

    char callback_id[128];
    snprintf(callback_id, sizeof(callback_id), "block_fork_resolved_%p", (void *)ctx);
    cf_callbacks_registry_add(CF_CALLBACK_TYPE_BLOCK_FORK_RESOLVED, callback, user_data, ctx, callback_id);

    return PyLong_FromLong(rc);
}

static PyObject *dap_chain_block_cache_get_by_hash_py(PyObject *self, PyObject *args) {
    (void)self;
    PyObject *chain_obj = NULL;
    PyObject *hash_obj = NULL;
    if (!PyArg_ParseTuple(args, "OO", &chain_obj, &hash_obj)) {
        return NULL;
    }
    dap_chain_type_blocks_t *blocks = cf_type_get_blocks(chain_obj);
    if (!blocks) {
        return NULL;
    }
    dap_hash_fast_t hash = {};
    if (cf_common_parse_hash_fast(hash_obj, &hash) != 0) {
        return NULL;
    }
    dap_chain_block_cache_t *cache = dap_chain_block_cache_get_by_hash(blocks, &hash);
    if (!cache) {
        Py_RETURN_NONE;
    }
    return cf_type_block_cache_to_dict(cache);
}

static PyObject *dap_chain_block_cache_get_by_number_py(PyObject *self, PyObject *args) {
    (void)self;
    PyObject *chain_obj = NULL;
    unsigned long long block_number = 0;
    if (!PyArg_ParseTuple(args, "OK", &chain_obj, &block_number)) {
        return NULL;
    }
    dap_chain_type_blocks_t *blocks = cf_type_get_blocks(chain_obj);
    if (!blocks) {
        return NULL;
    }
    dap_chain_block_cache_t *cache = dap_chain_block_cache_get_by_number(blocks, (uint64_t)block_number);
    if (!cache) {
        Py_RETURN_NONE;
    }
    return cf_type_block_cache_to_dict(cache);
}

static PyObject *dap_chain_cs_blocks_get_fee_group_py(PyObject *self, PyObject *args) {
    (void)self;
    const char *net_name = NULL;
    if (!PyArg_ParseTuple(args, "s", &net_name)) {
        return NULL;
    }
    char *group = dap_chain_cs_blocks_get_fee_group(net_name);
    if (!group) {
        Py_RETURN_NONE;
    }
    PyObject *result = PyUnicode_FromString(group);
    DAP_DELETE(group);
    return result;
}

static PyObject *dap_chain_cs_blocks_get_reward_group_py(PyObject *self, PyObject *args) {
    (void)self;
    const char *net_name = NULL;
    if (!PyArg_ParseTuple(args, "s", &net_name)) {
        return NULL;
    }
    char *group = dap_chain_cs_blocks_get_reward_group(net_name);
    if (!group) {
        Py_RETURN_NONE;
    }
    PyObject *result = PyUnicode_FromString(group);
    DAP_DELETE(group);
    return result;
}

static PyObject *dap_chain_type_blocks_init_py(PyObject *self, PyObject *args) {
    (void)self;
    (void)args;
    int rc = dap_chain_type_blocks_init();
    return PyLong_FromLong(rc);
}

static PyObject *dap_chain_type_blocks_deinit_py(PyObject *self, PyObject *args) {
    (void)self;
    (void)args;
    dap_chain_type_blocks_deinit();
    Py_RETURN_NONE;
}

static PyObject *dap_chain_type_blocks_fees_aggregate_py(PyObject *self, PyObject *args) {
    (void)self;
    PyObject *chain_obj = NULL;
    if (!PyArg_ParseTuple(args, "O", &chain_obj)) {
        return NULL;
    }
    dap_chain_type_blocks_t *blocks = cf_type_get_blocks(chain_obj);
    if (!blocks) {
        return NULL;
    }
    dap_ledger_hardfork_fees_t *fees = dap_chain_type_blocks_fees_aggregate(blocks->chain);
    PyObject *list = PyList_New(0);
    if (!list) {
        cf_type_free_fees_list(fees);
        return NULL;
    }
    for (dap_ledger_hardfork_fees_t *it = fees; it; ) {
        dap_ledger_hardfork_fees_t *next = it->next;
        PyObject *dict = PyDict_New();
        if (!dict) {
            Py_DECREF(list);
            cf_type_free_fees_list(it);
            return NULL;
        }
        if (cf_type_dict_set(dict, "owner_addr",
                             PyBytes_FromStringAndSize((const char *)&it->owner_addr, sizeof(dap_chain_addr_t))) < 0 ||
            cf_type_dict_set(dict, "fees_n_rewards_sum",
                             PyBytes_FromStringAndSize((const char *)&it->fees_n_rewards_sum, sizeof(uint256_t))) < 0) {
            Py_DECREF(dict);
            Py_DECREF(list);
            cf_type_free_fees_list(it);
            return NULL;
        }
        if (PyList_Append(list, dict) < 0) {
            Py_DECREF(dict);
            Py_DECREF(list);
            cf_type_free_fees_list(it);
            return NULL;
        }
        Py_DECREF(dict);
        DAP_DELETE(it);
        it = next;
    }
    return list;
}

static PyObject *dap_chain_type_blocks_get_block_signers_rewards_py(PyObject *self, PyObject *args) {
    (void)self;
    PyObject *chain_obj = NULL;
    PyObject *hash_obj = NULL;
    if (!PyArg_ParseTuple(args, "OO", &chain_obj, &hash_obj)) {
        return NULL;
    }
    dap_chain_type_blocks_t *blocks = cf_type_get_blocks(chain_obj);
    if (!blocks) {
        return NULL;
    }
    dap_hash_fast_t hash = {};
    if (cf_common_parse_hash_fast(hash_obj, &hash) != 0) {
        return NULL;
    }
    dap_list_t *list = dap_chain_type_blocks_get_block_signers_rewards(blocks->chain, &hash);
    if (!list) {
        Py_RETURN_NONE;
    }
    PyObject *py_list = PyList_New(0);
    if (!py_list) {
        dap_list_free_full(list, cf_type_free_list_item);
        return NULL;
    }
    for (dap_list_t *it = list; it; it = it->next) {
        dap_chain_cs_block_rewards_t *reward = (dap_chain_cs_block_rewards_t *)it->data;
        if (!reward) {
            continue;
        }
        PyObject *dict = PyDict_New();
        if (!dict) {
            Py_DECREF(py_list);
            dap_list_free_full(list, cf_type_free_list_item);
            return NULL;
        }
        if (cf_type_dict_set(dict, "pkey_hash",
                             PyBytes_FromStringAndSize((const char *)&reward->pkey_hash, sizeof(dap_chain_hash_fast_t))) < 0 ||
            cf_type_dict_set(dict, "reward",
                             PyBytes_FromStringAndSize((const char *)&reward->reward, sizeof(uint256_t))) < 0) {
            Py_DECREF(dict);
            Py_DECREF(py_list);
            dap_list_free_full(list, cf_type_free_list_item);
            return NULL;
        }
        if (PyList_Append(py_list, dict) < 0) {
            Py_DECREF(dict);
            Py_DECREF(py_list);
            dap_list_free_full(list, cf_type_free_list_item);
            return NULL;
        }
        Py_DECREF(dict);
    }
    dap_list_free_full(list, cf_type_free_list_item);
    return py_list;
}

static PyObject *dap_chain_type_blocks_get_pkey_by_hash_py(PyObject *self, PyObject *args) {
    (void)self;
    PyObject *net_obj = NULL;
    PyObject *hash_obj = NULL;
    if (!PyArg_ParseTuple(args, "OO", &net_obj, &hash_obj)) {
        return NULL;
    }
    dap_chain_net_t *net = cf_type_get_net(net_obj);
    if (!net) {
        return NULL;
    }
    dap_hash_fast_t hash = {};
    if (cf_common_parse_hash_fast(hash_obj, &hash) != 0) {
        return NULL;
    }
    dap_pkey_t *pkey = dap_chain_type_blocks_get_pkey_by_hash(net, &hash);
    if (!pkey) {
        Py_RETURN_NONE;
    }
    size_t pkey_size = dap_pkey_get_size(pkey);
    PyObject *bytes = PyBytes_FromStringAndSize((const char *)pkey, (Py_ssize_t)pkey_size);
    DAP_DELETE(pkey);
    return bytes;
}

// =========================================
// DAG CORE OPERATIONS
// =========================================

static PyObject *dap_chain_type_dag_init_py(PyObject *self, PyObject *args) {
    (void)self;
    (void)args;
    int rc = dap_chain_type_dag_init();
    return PyLong_FromLong(rc);
}

static PyObject *dap_chain_type_dag_deinit_py(PyObject *self, PyObject *args) {
    (void)self;
    (void)args;
    dap_chain_type_dag_deinit();
    Py_RETURN_NONE;
}

static PyObject *dap_chain_type_dag_start_py(PyObject *self, PyObject *args) {
    (void)self;
    PyObject *chain_obj = NULL;
    if (!PyArg_ParseTuple(args, "O", &chain_obj)) {
        return NULL;
    }
    dap_chain_type_dag_t *dag = cf_type_get_dag(chain_obj);
    if (!dag) {
        return NULL;
    }
    dap_chain_type_dag_start(dag);
    Py_RETURN_NONE;
}

static PyObject *dap_chain_type_dag_proc_event_round_new_py(PyObject *self, PyObject *args) {
    (void)self;
    PyObject *chain_obj = NULL;
    if (!PyArg_ParseTuple(args, "O", &chain_obj)) {
        return NULL;
    }
    dap_chain_type_dag_t *dag = cf_type_get_dag(chain_obj);
    if (!dag) {
        return NULL;
    }
    dap_chain_type_dag_proc_event_round_new(dag);
    Py_RETURN_NONE;
}

static PyObject *dap_chain_type_dag_find_event_by_hash_py(PyObject *self, PyObject *args) {
    (void)self;
    PyObject *chain_obj = NULL;
    PyObject *hash_obj = NULL;
    if (!PyArg_ParseTuple(args, "OO", &chain_obj, &hash_obj)) {
        return NULL;
    }
    dap_chain_type_dag_t *dag = cf_type_get_dag(chain_obj);
    if (!dag) {
        return NULL;
    }
    dap_hash_fast_t hash = {};
    if (cf_common_parse_hash_fast(hash_obj, &hash) != 0) {
        return NULL;
    }
    dap_chain_type_dag_event_t *event = dap_chain_type_dag_find_event_by_hash(dag, &hash);
    if (!event) {
        Py_RETURN_NONE;
    }
    uint64_t size = dap_chain_type_dag_event_calc_size(event, 0);
    if (!size) {
        PyErr_SetString(PyExc_ValueError, "event size is invalid");
        return NULL;
    }
    PyObject *dict = PyDict_New();
    if (!dict) {
        return NULL;
    }
    if (cf_type_dict_set(dict, "event", PyBytes_FromStringAndSize((const char *)event, (Py_ssize_t)size)) < 0 ||
        cf_type_dict_set(dict, "size", PyLong_FromUnsignedLongLong(size)) < 0) {
        Py_DECREF(dict);
        return NULL;
    }
    return dict;
}

// =========================================
// DAG EVENT OPERATIONS
// =========================================

static PyObject *dap_chain_type_dag_event_new_py(PyObject *self, PyObject *args) {
    (void)self;
    unsigned long long chain_id = 0;
    PyObject *cell_id_obj = NULL;
    PyObject *datum_obj = NULL;
    PyObject *key_obj = Py_None;
    PyObject *hashes_obj = Py_None;

    if (!PyArg_ParseTuple(args, "KOO|OO", &chain_id, &cell_id_obj, &datum_obj, &key_obj, &hashes_obj)) {
        return NULL;
    }

    dap_chain_id_t chain_id_struct = {.uint64 = chain_id};
    dap_chain_cell_id_t cell_id = {0};
    if (cf_chain_parse_cell_id(cell_id_obj, &cell_id) != 0) {
        return NULL;
    }

    const char *datum_bytes = NULL;
    Py_ssize_t datum_size_in = 0;
    if (cf_type_extract_bytes(datum_obj, &datum_bytes, &datum_size_in, "datum") != 0) {
        return NULL;
    }
    if ((size_t)datum_size_in < sizeof(dap_chain_datum_t)) {
        PyErr_SetString(PyExc_ValueError, "datum bytes are too small");
        return NULL;
    }
    dap_chain_datum_t *datum = (dap_chain_datum_t *)datum_bytes;
    size_t datum_size = dap_chain_datum_size(datum);
    if (!datum_size || datum_size != (size_t)datum_size_in) {
        PyErr_SetString(PyExc_ValueError, "datum size is invalid");
        return NULL;
    }

    dap_enc_key_t *key = NULL;
    if (key_obj && key_obj != Py_None) {
        key = cf_type_get_key(key_obj);
        if (!key) {
            return NULL;
        }
    }

    dap_chain_hash_fast_t *hashes = NULL;
    size_t hashes_count = 0;
    if (cf_type_hash_array_from_py(hashes_obj, &hashes, &hashes_count) != 0) {
        return NULL;
    }

    size_t event_size = 0;
    dap_chain_type_dag_event_t *event = dap_chain_type_dag_event_new(chain_id_struct, cell_id,
                                                                     datum, key, hashes,
                                                                     hashes_count, &event_size);
    DAP_DELETE(hashes);
    if (!event || !event_size) {
        PyErr_SetString(PyExc_RuntimeError, "Failed to create dag event");
        return NULL;
    }

    PyObject *dict = PyDict_New();
    if (!dict) {
        DAP_DELETE(event);
        return NULL;
    }
    if (cf_type_dict_set(dict, "event", PyBytes_FromStringAndSize((const char *)event, (Py_ssize_t)event_size)) < 0 ||
        cf_type_dict_set(dict, "size", PyLong_FromSize_t(event_size)) < 0) {
        Py_DECREF(dict);
        DAP_DELETE(event);
        return NULL;
    }
    DAP_DELETE(event);
    return dict;
}

static PyObject *dap_chain_type_dag_event_calc_hash_py(PyObject *self, PyObject *args) {
    (void)self;
    PyObject *event_obj = NULL;
    if (!PyArg_ParseTuple(args, "O", &event_obj)) {
        return NULL;
    }
    const char *event_bytes = NULL;
    Py_ssize_t event_size_in = 0;
    if (cf_type_extract_event_bytes(event_obj, &event_bytes, &event_size_in) != 0) {
        return NULL;
    }
    size_t event_size = 0;
    if (cf_type_event_size_checked(event_bytes, (size_t)event_size_in, &event_size) != 0) {
        return NULL;
    }
    dap_hash_fast_t hash = {};
    dap_chain_type_dag_event_calc_hash((dap_chain_type_dag_event_t *)event_bytes, event_size, &hash);
    return PyBytes_FromStringAndSize((const char *)&hash, sizeof(dap_hash_fast_t));
}

static PyObject *dap_chain_type_dag_event_calc_size_py(PyObject *self, PyObject *args) {
    (void)self;
    PyObject *event_obj = NULL;
    unsigned long long limit = 0;
    if (!PyArg_ParseTuple(args, "O|K", &event_obj, &limit)) {
        return NULL;
    }
    const char *event_bytes = NULL;
    Py_ssize_t event_size_in = 0;
    if (cf_type_extract_event_bytes(event_obj, &event_bytes, &event_size_in) != 0) {
        return NULL;
    }
    uint64_t limit_size = limit ? (uint64_t)limit : (uint64_t)event_size_in;
    dap_chain_type_dag_event_t *event = (dap_chain_type_dag_event_t *)event_bytes;
    uint64_t size = dap_chain_type_dag_event_calc_size(event, limit_size);
    if (!size && event->header.signs_count == 0) {
        size = dap_chain_type_dag_event_calc_size_excl_signs(event, limit_size);
    }
    if (!size) {
        PyErr_SetString(PyExc_ValueError, "event size is invalid");
        return NULL;
    }
    return PyLong_FromUnsignedLongLong(size);
}

static PyObject *dap_chain_type_dag_event_calc_size_excl_signs_py(PyObject *self, PyObject *args) {
    (void)self;
    PyObject *event_obj = NULL;
    unsigned long long limit = 0;
    if (!PyArg_ParseTuple(args, "O|K", &event_obj, &limit)) {
        return NULL;
    }
    const char *event_bytes = NULL;
    Py_ssize_t event_size_in = 0;
    if (cf_type_extract_event_bytes(event_obj, &event_bytes, &event_size_in) != 0) {
        return NULL;
    }
    uint64_t limit_size = limit ? (uint64_t)limit : (uint64_t)event_size_in;
    uint64_t size = dap_chain_type_dag_event_calc_size_excl_signs((dap_chain_type_dag_event_t *)event_bytes, limit_size);
    if (!size) {
        PyErr_SetString(PyExc_ValueError, "event size is invalid");
        return NULL;
    }
    return PyLong_FromUnsignedLongLong(size);
}

static PyObject *dap_chain_type_dag_event_get_datum_py(PyObject *self, PyObject *args) {
    (void)self;
    PyObject *event_obj = NULL;
    if (!PyArg_ParseTuple(args, "O", &event_obj)) {
        return NULL;
    }
    const char *event_bytes = NULL;
    Py_ssize_t event_size_in = 0;
    if (cf_type_extract_event_bytes(event_obj, &event_bytes, &event_size_in) != 0) {
        return NULL;
    }
    size_t event_size = 0;
    if (cf_type_event_size_checked(event_bytes, (size_t)event_size_in, &event_size) != 0) {
        return NULL;
    }
    dap_chain_type_dag_event_t *event = (dap_chain_type_dag_event_t *)event_bytes;
    dap_chain_datum_t *datum = dap_chain_type_dag_event_get_datum(event, event_size);
    if (!datum) {
        Py_RETURN_NONE;
    }
    size_t max_size = dap_chain_type_dag_event_get_datum_size_maximum(event, event_size);
    size_t datum_size = dap_chain_datum_size(datum);
    if (!datum_size || datum_size > max_size) {
        PyErr_SetString(PyExc_ValueError, "datum size is invalid");
        return NULL;
    }
    return PyBytes_FromStringAndSize((const char *)datum, (Py_ssize_t)datum_size);
}

static PyObject *dap_chain_type_dag_event_get_datum_size_maximum_py(PyObject *self, PyObject *args) {
    (void)self;
    PyObject *event_obj = NULL;
    if (!PyArg_ParseTuple(args, "O", &event_obj)) {
        return NULL;
    }
    const char *event_bytes = NULL;
    Py_ssize_t event_size_in = 0;
    if (cf_type_extract_event_bytes(event_obj, &event_bytes, &event_size_in) != 0) {
        return NULL;
    }
    size_t event_size = 0;
    if (cf_type_event_size_checked(event_bytes, (size_t)event_size_in, &event_size) != 0) {
        return NULL;
    }
    size_t max_size = dap_chain_type_dag_event_get_datum_size_maximum((dap_chain_type_dag_event_t *)event_bytes, event_size);
    return PyLong_FromSize_t(max_size);
}

static PyObject *dap_chain_type_dag_event_get_sign_py(PyObject *self, PyObject *args) {
    (void)self;
    PyObject *event_obj = NULL;
    unsigned int sign_num = 0;
    if (!PyArg_ParseTuple(args, "OI", &event_obj, &sign_num)) {
        return NULL;
    }
    const char *event_bytes = NULL;
    Py_ssize_t event_size_in = 0;
    if (cf_type_extract_event_bytes(event_obj, &event_bytes, &event_size_in) != 0) {
        return NULL;
    }
    size_t event_size = 0;
    if (cf_type_event_size_checked(event_bytes, (size_t)event_size_in, &event_size) != 0) {
        return NULL;
    }
    dap_sign_t *sign = dap_chain_type_dag_event_get_sign((dap_chain_type_dag_event_t *)event_bytes, event_size,
                                                         (uint16_t)sign_num);
    if (!sign) {
        Py_RETURN_NONE;
    }
    size_t sign_size = dap_sign_get_size(sign);
    if (!sign_size) {
        PyErr_SetString(PyExc_ValueError, "sign size is invalid");
        return NULL;
    }
    const uint8_t *event_base = (const uint8_t *)event_bytes;
    const uint8_t *sign_base = (const uint8_t *)sign;
    if (sign_base < event_base || sign_base + sign_size > event_base + event_size) {
        PyErr_SetString(PyExc_ValueError, "sign is outside event bounds");
        return NULL;
    }
    return PyBytes_FromStringAndSize((const char *)sign, (Py_ssize_t)sign_size);
}

static PyObject *dap_chain_type_dag_event_sign_add_py(PyObject *self, PyObject *args) {
    (void)self;
    PyObject *event_obj = NULL;
    PyObject *key_obj = NULL;
    if (!PyArg_ParseTuple(args, "OO", &event_obj, &key_obj)) {
        return NULL;
    }
    const char *event_bytes = NULL;
    Py_ssize_t event_size_in = 0;
    if (cf_type_extract_event_bytes(event_obj, &event_bytes, &event_size_in) != 0) {
        return NULL;
    }
    size_t event_size = 0;
    if (cf_type_event_size_checked(event_bytes, (size_t)event_size_in, &event_size) != 0) {
        return NULL;
    }
    dap_enc_key_t *key = cf_type_get_key(key_obj);
    if (!key) {
        return NULL;
    }
    dap_chain_type_dag_event_t *event = DAP_DUP_SIZE(event_bytes, event_size);
    if (!event) {
        PyErr_SetString(PyExc_MemoryError, "Failed to allocate event copy");
        return NULL;
    }
    size_t new_size = dap_chain_type_dag_event_sign_add(&event, event_size, key);
    if (new_size == 0 || new_size <= event_size) {
        new_size = event_size;
    }
    PyObject *bytes = PyBytes_FromStringAndSize((const char *)event, (Py_ssize_t)new_size);
    DAP_DELETE(event);
    return bytes;
}

static PyObject *dap_chain_type_dag_event_sign_exists_py(PyObject *self, PyObject *args) {
    (void)self;
    PyObject *event_obj = NULL;
    PyObject *key_obj = NULL;
    if (!PyArg_ParseTuple(args, "OO", &event_obj, &key_obj)) {
        return NULL;
    }
    const char *event_bytes = NULL;
    Py_ssize_t event_size_in = 0;
    if (cf_type_extract_event_bytes(event_obj, &event_bytes, &event_size_in) != 0) {
        return NULL;
    }
    size_t event_size = 0;
    if (cf_type_event_size_checked(event_bytes, (size_t)event_size_in, &event_size) != 0) {
        return NULL;
    }
    dap_enc_key_t *key = cf_type_get_key(key_obj);
    if (!key) {
        return NULL;
    }
    bool exists = dap_chain_type_dag_event_sign_exists((dap_chain_type_dag_event_t *)event_bytes, event_size, key);
    return PyBool_FromLong(exists ? 1 : 0);
}

static PyObject *dap_chain_type_dag_event_round_sign_add_py(PyObject *self, PyObject *args) {
    (void)self;
    PyObject *round_obj = NULL;
    PyObject *key_obj = NULL;
    if (!PyArg_ParseTuple(args, "OO", &round_obj, &key_obj)) {
        return NULL;
    }
    const char *round_bytes = NULL;
    Py_ssize_t round_size_in = 0;
    if (cf_type_extract_round_item_bytes(round_obj, &round_bytes, &round_size_in) != 0) {
        return NULL;
    }
    size_t round_size = 0;
    if (cf_type_round_item_size_checked(round_bytes, (size_t)round_size_in, &round_size) != 0) {
        return NULL;
    }
    dap_enc_key_t *key = cf_type_get_key(key_obj);
    if (!key) {
        return NULL;
    }
    dap_chain_type_dag_event_round_item_t *round_item = DAP_DUP_SIZE(round_bytes, round_size);
    if (!round_item) {
        PyErr_SetString(PyExc_MemoryError, "Failed to allocate round_item copy");
        return NULL;
    }
    dap_chain_type_dag_event_round_sign_add(&round_item, round_size, key);
    size_t new_size = dap_chain_type_dag_event_round_item_get_size(round_item);
    PyObject *bytes = PyBytes_FromStringAndSize((const char *)round_item, (Py_ssize_t)new_size);
    DAP_DELETE(round_item);
    return bytes;
}

static PyObject *dap_chain_type_dag_event_round_sign_exists_py(PyObject *self, PyObject *args) {
    (void)self;
    PyObject *round_obj = NULL;
    PyObject *key_obj = NULL;
    if (!PyArg_ParseTuple(args, "OO", &round_obj, &key_obj)) {
        return NULL;
    }
    const char *round_bytes = NULL;
    Py_ssize_t round_size_in = 0;
    if (cf_type_extract_round_item_bytes(round_obj, &round_bytes, &round_size_in) != 0) {
        return NULL;
    }
    size_t round_size = 0;
    if (cf_type_round_item_size_checked(round_bytes, (size_t)round_size_in, &round_size) != 0) {
        return NULL;
    }
    dap_enc_key_t *key = cf_type_get_key(key_obj);
    if (!key) {
        return NULL;
    }
    bool exists = dap_chain_type_dag_event_round_sign_exists((dap_chain_type_dag_event_round_item_t *)round_bytes, key);
    return PyBool_FromLong(exists ? 1 : 0);
}

static PyObject *dap_chain_type_dag_event_round_item_get_size_py(PyObject *self, PyObject *args) {
    (void)self;
    PyObject *round_obj = NULL;
    if (!PyArg_ParseTuple(args, "O", &round_obj)) {
        return NULL;
    }
    const char *round_bytes = NULL;
    Py_ssize_t round_size_in = 0;
    if (cf_type_extract_round_item_bytes(round_obj, &round_bytes, &round_size_in) != 0) {
        return NULL;
    }
    size_t round_size = 0;
    if (cf_type_round_item_size_checked(round_bytes, (size_t)round_size_in, &round_size) != 0) {
        return NULL;
    }
    return PyLong_FromSize_t(round_size);
}

static PyObject *dap_chain_type_dag_event_gdb_set_py(PyObject *self, PyObject *args) {
    (void)self;
    PyObject *chain_obj = NULL;
    const char *event_hash_str = NULL;
    PyObject *event_obj = NULL;
    PyObject *round_obj = Py_None;
    if (!PyArg_ParseTuple(args, "OsO|O", &chain_obj, &event_hash_str, &event_obj, &round_obj)) {
        return NULL;
    }
    dap_chain_type_dag_t *dag = cf_type_get_dag(chain_obj);
    if (!dag) {
        return NULL;
    }
    const char *event_bytes = NULL;
    Py_ssize_t event_size_in = 0;
    if (cf_type_extract_event_bytes(event_obj, &event_bytes, &event_size_in) != 0) {
        return NULL;
    }
    size_t event_size = 0;
    if (cf_type_event_size_checked(event_bytes, (size_t)event_size_in, &event_size) != 0) {
        return NULL;
    }

    bool result = false;
    if (round_obj && round_obj != Py_None) {
        const char *round_bytes = NULL;
        Py_ssize_t round_size_in = 0;
        if (cf_type_extract_round_item_bytes(round_obj, &round_bytes, &round_size_in) != 0) {
            return NULL;
        }
        size_t round_size = 0;
        if (cf_type_round_item_size_checked(round_bytes, (size_t)round_size_in, &round_size) != 0) {
            return NULL;
        }
        result = dap_chain_type_dag_event_gdb_set(dag, event_hash_str,
                                                  (dap_chain_type_dag_event_t *)event_bytes, event_size,
                                                  (dap_chain_type_dag_event_round_item_t *)round_bytes);
    } else {
        dap_chain_type_dag_event_round_item_t round_item = {0};
        dap_chain_type_dag_event_t *event = (dap_chain_type_dag_event_t *)event_bytes;
        dap_chain_datum_t *datum = dap_chain_type_dag_event_get_datum(event, event_size);
        if (!datum) {
            PyErr_SetString(PyExc_ValueError, "Failed to extract datum from event");
            return NULL;
        }
        dap_chain_datum_calc_hash(datum, &round_item.round_info.datum_hash);
        result = dap_chain_type_dag_event_gdb_set(dag, event_hash_str,
                                                  (dap_chain_type_dag_event_t *)event_bytes, event_size,
                                                  &round_item);
    }

    return PyBool_FromLong(result ? 1 : 0);
}

static PyObject *dap_chain_type_dag_event_gdb_get_py(PyObject *self, PyObject *args) {
    (void)self;
    PyObject *chain_obj = NULL;
    const char *event_hash_str = NULL;
    PyObject *group_obj = Py_None;
    if (!PyArg_ParseTuple(args, "Os|O", &chain_obj, &event_hash_str, &group_obj)) {
        return NULL;
    }
    dap_chain_type_dag_t *dag = cf_type_get_dag(chain_obj);
    if (!dag) {
        return NULL;
    }
    const char *group = NULL;
    group = cf_type_get_optional_str(group_obj, "group");
    if (!group && PyErr_Occurred()) {
        return NULL;
    }
    if (!group) {
        group = dag->gdb_group_events_round_new;
    }
    if (!group) {
        PyErr_SetString(PyExc_ValueError, "DAG group is not set");
        return NULL;
    }

    size_t event_size = 0;
    dap_chain_type_dag_event_round_info_t round_info = {0};
    dap_chain_type_dag_event_t *event = dap_chain_type_dag_event_gdb_get(event_hash_str, &event_size, group, &round_info);
    if (!event) {
        Py_RETURN_NONE;
    }

    PyObject *dict = PyDict_New();
    if (!dict) {
        DAP_DELETE(event);
        return NULL;
    }
    if (cf_type_dict_set(dict, "event", PyBytes_FromStringAndSize((const char *)event, (Py_ssize_t)event_size)) < 0 ||
        cf_type_dict_set(dict, "size", PyLong_FromSize_t(event_size)) < 0) {
        Py_DECREF(dict);
        DAP_DELETE(event);
        return NULL;
    }

    PyObject *round_dict = PyDict_New();
    if (!round_dict) {
        Py_DECREF(dict);
        DAP_DELETE(event);
        return NULL;
    }
    if (cf_type_dict_set(round_dict, "reject_count", PyLong_FromUnsignedLong(round_info.reject_count)) < 0 ||
        cf_type_dict_set(round_dict, "ts_update", PyLong_FromUnsignedLongLong(round_info.ts_update)) < 0 ||
        cf_type_dict_set(round_dict, "datum_hash",
                         PyBytes_FromStringAndSize((const char *)&round_info.datum_hash, sizeof(dap_chain_hash_fast_t))) < 0) {
        Py_DECREF(round_dict);
        Py_DECREF(dict);
        DAP_DELETE(event);
        return NULL;
    }
    if (cf_type_dict_set(dict, "round_info", round_dict) < 0) {
        Py_DECREF(dict);
        DAP_DELETE(event);
        return NULL;
    }

    DAP_DELETE(event);
    return dict;
}

static PyObject *dap_chain_type_dag_event_broadcast_py(PyObject *self, PyObject *args) {
    (void)self;
    PyObject *chain_obj = NULL;
    PyObject *store_obj = NULL;
    PyObject *dbi_obj = NULL;
    if (!PyArg_ParseTuple(args, "OOO", &chain_obj, &store_obj, &dbi_obj)) {
        return NULL;
    }
    (void)chain_obj;
    (void)store_obj;
    (void)dbi_obj;
    PyErr_SetString(PyExc_NotImplementedError,
                    "dap_chain_type_dag_event_broadcast is unavailable in this SDK build");
    return NULL;
}

// =========================================
// NONE TYPE OPERATIONS
// =========================================

static PyObject *dap_nonconsensus_init_py(PyObject *self, PyObject *args) {
    (void)self;
    (void)args;
    int rc = dap_nonconsensus_init();
    return PyLong_FromLong(rc);
}

static PyObject *dap_nonconsensus_get_group_py(PyObject *self, PyObject *args) {
    (void)self;
    PyObject *chain_obj = NULL;
    if (!PyArg_ParseTuple(args, "O", &chain_obj)) {
        return NULL;
    }
    dap_chain_t *chain = cf_type_get_chain(chain_obj);
    if (!chain) {
        return NULL;
    }
    const char *group = dap_nonconsensus_get_group(chain);
    if (!group) {
        Py_RETURN_NONE;
    }
    return PyUnicode_FromString(group);
}

// =========================================
// MODULE INITIALIZATION
// =========================================

static int cf_type_register_methods(PyObject *module, PyMethodDef *methods) {
    for (PyMethodDef *method = methods; method->ml_name != NULL; method++) {
        PyObject *func = PyCFunction_New(method, NULL);
        if (!func) {
            return -1;
        }
        if (PyObject_SetAttrString(module, method->ml_name, func) < 0) {
            Py_DECREF(func);
            return -1;
        }
        Py_DECREF(func);
    }
    return 0;
}

int cellframe_type_init(PyObject *module) {
    static PyMethodDef type_methods[] = {
        {"dap_chain_block_init", dap_chain_block_init_py, METH_NOARGS, "Initialize block module"},
        {"dap_chain_block_deinit", dap_chain_block_deinit_py, METH_NOARGS, "Deinitialize block module"},
        {"dap_chain_block_new_bytes", dap_chain_block_new_py, METH_VARARGS, "Create new block bytes"},
        {"dap_chain_block_get_size_bytes", dap_chain_block_get_size_py, METH_VARARGS, "Get block size from bytes"},
        {"dap_chain_block_meta_add_bytes", dap_chain_block_meta_add_py, METH_VARARGS, "Add block metadata to bytes"},
        {"dap_chain_block_meta_get", dap_chain_block_meta_get_py, METH_VARARGS, "Get block metadata"},
        {"dap_chain_block_meta_extract", dap_chain_block_meta_extract_py, METH_VARARGS, "Extract block metadata"},
        {"dap_chain_block_datum_add_bytes", dap_chain_block_datum_add_py, METH_VARARGS, "Add datum to block bytes"},
        {"dap_chain_block_datum_del_by_hash", dap_chain_block_datum_del_by_hash_py, METH_VARARGS, "Delete datum from block by hash"},
        {"dap_chain_block_get_datums_bytes", dap_chain_block_get_datums_py, METH_VARARGS, "Get block datums from bytes"},
        {"dap_chain_block_sign_add_bytes", dap_chain_block_sign_add_py, METH_VARARGS, "Add block sign to bytes"},
        {"dap_chain_block_sign_get", dap_chain_block_sign_get_py, METH_VARARGS, "Get block sign"},
        {"dap_chain_block_sign_match_pkey", dap_chain_block_sign_match_pkey_py, METH_VARARGS, "Match block sign by pkey"},
        {"dap_chain_block_get_signs_count", dap_chain_block_get_signs_count_py, METH_VARARGS, "Get block sign count"},
        {"dap_chain_block_get_sign_offset", dap_chain_block_get_sign_offset_py, METH_VARARGS, "Get block sign offset"},
        {"dap_chain_block_get_prev_hash", dap_chain_block_get_prev_hash_py, METH_VARARGS, "Get block prev hash"},

        {"dap_chain_block_cache_init", dap_chain_block_cache_init_py, METH_NOARGS, "Initialize block cache"},
        {"dap_chain_block_cache_deinit", dap_chain_block_cache_deinit_py, METH_NOARGS, "Deinitialize block cache"},
        {"dap_chain_block_cache_new", dap_chain_block_cache_new_py, METH_VARARGS, "Create block cache"},
        {"dap_chain_block_cache_dup", dap_chain_block_cache_dup_py, METH_VARARGS, "Duplicate block cache"},
        {"dap_chain_block_cache_update", dap_chain_block_cache_update_py, METH_VARARGS, "Update block cache"},
        {"dap_chain_block_cache_delete", dap_chain_block_cache_delete_py, METH_VARARGS, "Delete block cache"},
        {"dap_chain_block_get_list_tx_cond_outs_with_val", dap_chain_block_get_list_tx_cond_outs_with_val_py, METH_VARARGS,
         "Get list of conditional outs with total value"},

        {"dap_chain_block_tx_coll_fee_create", dap_chain_block_tx_coll_fee_create_py, METH_VARARGS, "Create fee collection tx"},
        {"dap_chain_block_tx_reward_create", dap_chain_block_tx_reward_create_py, METH_VARARGS, "Create reward collection tx"},
        {"dap_chain_block_tx_coll_fee_stack_create", dap_chain_block_tx_coll_fee_stack_create_py, METH_VARARGS, "Create stacked fee collection tx"},

        {"dap_chain_blocks_decree_init", dap_chain_blocks_decree_init_py, METH_NOARGS, "Init blocks decree"},
        {"dap_chain_blocks_decree_deinit", dap_chain_blocks_decree_deinit_py, METH_NOARGS, "Deinit blocks decree"},

        {"dap_chain_block_add_fork_notificator", dap_chain_block_add_fork_notificator_py, METH_VARARGS, "Add fork notificator"},
        {"dap_chain_block_cache_get_by_hash", dap_chain_block_cache_get_by_hash_py, METH_VARARGS, "Get block cache by hash"},
        {"dap_chain_block_cache_get_by_number", dap_chain_block_cache_get_by_number_py, METH_VARARGS, "Get block cache by number"},
        {"dap_chain_cs_blocks_get_fee_group", dap_chain_cs_blocks_get_fee_group_py, METH_VARARGS, "Get fee group name"},
        {"dap_chain_cs_blocks_get_reward_group", dap_chain_cs_blocks_get_reward_group_py, METH_VARARGS, "Get reward group name"},
        {"dap_chain_type_blocks_init", dap_chain_type_blocks_init_py, METH_NOARGS, "Init blocks type"},
        {"dap_chain_type_blocks_deinit", dap_chain_type_blocks_deinit_py, METH_NOARGS, "Deinit blocks type"},
        {"dap_chain_type_blocks_fees_aggregate", dap_chain_type_blocks_fees_aggregate_py, METH_VARARGS, "Aggregate block fees"},
        {"dap_chain_type_blocks_get_block_signers_rewards", dap_chain_type_blocks_get_block_signers_rewards_py, METH_VARARGS,
         "Get block signers rewards"},
        {"dap_chain_type_blocks_get_pkey_by_hash", dap_chain_type_blocks_get_pkey_by_hash_py, METH_VARARGS, "Get pkey by hash"},

        {"dap_chain_type_dag_init", dap_chain_type_dag_init_py, METH_NOARGS, "Init dag type"},
        {"dap_chain_type_dag_deinit", dap_chain_type_dag_deinit_py, METH_NOARGS, "Deinit dag type"},
        {"dap_chain_type_dag_start", dap_chain_type_dag_start_py, METH_VARARGS, "Start dag type"},
        {"dap_chain_type_dag_proc_event_round_new", dap_chain_type_dag_proc_event_round_new_py, METH_VARARGS, "Process dag round"},
        {"dap_chain_type_dag_find_event_by_hash", dap_chain_type_dag_find_event_by_hash_py, METH_VARARGS, "Find dag event by hash"},

        {"dap_chain_type_dag_event_new", dap_chain_type_dag_event_new_py, METH_VARARGS, "Create dag event"},
        {"dap_chain_type_dag_event_calc_hash", dap_chain_type_dag_event_calc_hash_py, METH_VARARGS, "Calculate dag event hash"},
        {"dap_chain_type_dag_event_calc_size", dap_chain_type_dag_event_calc_size_py, METH_VARARGS, "Calculate dag event size"},
        {"dap_chain_type_dag_event_calc_size_excl_signs", dap_chain_type_dag_event_calc_size_excl_signs_py, METH_VARARGS,
         "Calculate dag event size without signs"},
        {"dap_chain_type_dag_event_get_datum", dap_chain_type_dag_event_get_datum_py, METH_VARARGS, "Get datum from dag event"},
        {"dap_chain_type_dag_event_get_datum_size_maximum", dap_chain_type_dag_event_get_datum_size_maximum_py, METH_VARARGS,
         "Get dag event datum maximum size"},
        {"dap_chain_type_dag_event_get_sign", dap_chain_type_dag_event_get_sign_py, METH_VARARGS, "Get dag event sign"},
        {"dap_chain_type_dag_event_sign_add", dap_chain_type_dag_event_sign_add_py, METH_VARARGS, "Add dag event sign"},
        {"dap_chain_type_dag_event_sign_exists", dap_chain_type_dag_event_sign_exists_py, METH_VARARGS, "Check dag event sign"},
        {"dap_chain_type_dag_event_round_sign_add", dap_chain_type_dag_event_round_sign_add_py, METH_VARARGS, "Add dag round sign"},
        {"dap_chain_type_dag_event_round_sign_exists", dap_chain_type_dag_event_round_sign_exists_py, METH_VARARGS,
         "Check dag round sign"},
        {"dap_chain_type_dag_event_round_item_get_size", dap_chain_type_dag_event_round_item_get_size_py, METH_VARARGS,
         "Get dag round item size"},
        {"dap_chain_type_dag_event_gdb_set", dap_chain_type_dag_event_gdb_set_py, METH_VARARGS, "Set dag event in gdb"},
        {"dap_chain_type_dag_event_gdb_get", dap_chain_type_dag_event_gdb_get_py, METH_VARARGS, "Get dag event from gdb"},
        {"dap_chain_type_dag_event_broadcast", dap_chain_type_dag_event_broadcast_py, METH_VARARGS, "Broadcast dag event"},

        {"dap_nonconsensus_init", dap_nonconsensus_init_py, METH_NOARGS, "Init nonconsensus"},
        {"dap_nonconsensus_get_group", dap_nonconsensus_get_group_py, METH_VARARGS, "Get nonconsensus group"},

        {NULL, NULL, 0, NULL}
    };

    if (cf_type_register_methods(module, type_methods) < 0) {
        return -1;
    }

    return 0;
}
