#include "cellframe.h"
#include "cf_common.h"
#include "cf_callbacks_registry.h"

#include "dap_chain_atom.h"
#include "dap_chain_block_callbacks.h"
#include "dap_chain_rpc_callbacks.h"
#include "dap_chain_net_api.h"
#include "dap_chain_types.h"
#include "dap_cli_error_codes.h"
#include "dap_json.h"
#include "dap_hash.h"

#define LOG_TAG "python_cellframe_common"

typedef struct {
    PyObject *callback;
    PyObject *user_data;
} python_callback_ctx_t;

static python_callback_ctx_t *s_block_tax_ctx = NULL;
static python_callback_ctx_t *s_rpc_consensus_ctx = NULL;
static python_callback_ctx_t *s_rpc_storage_ctx = NULL;
static python_callback_ctx_t *s_rpc_service_ctx = NULL;
static python_callback_ctx_t *s_rpc_wallet_ctx = NULL;
static python_callback_ctx_t *s_rpc_tx_notify_ctx = NULL;

static void cf_common_clear_ctx(python_callback_ctx_t **ctx_ptr) {
    if (!ctx_ptr || !*ctx_ptr) {
        return;
    }
    cf_callbacks_registry_remove(*ctx_ptr);
    DAP_DELETE(*ctx_ptr);
    *ctx_ptr = NULL;
}

static int cf_common_parse_hash_fast(PyObject *obj, dap_hash_fast_t *out) {
    if (!obj || obj == Py_None) {
        PyErr_SetString(PyExc_TypeError, "hash value is required");
        return -1;
    }
    if (PyCapsule_CheckExact(obj)) {
        dap_hash_fast_t *ptr = (dap_hash_fast_t *)PyCapsule_GetPointer(obj, "dap_hash_fast_t");
        if (!ptr) {
            PyErr_SetString(PyExc_ValueError, "Invalid dap_hash_fast_t capsule");
            return -1;
        }
        *out = *ptr;
        return 0;
    }
    if (PyBytes_Check(obj)) {
        Py_ssize_t size = PyBytes_Size(obj);
        if ((size_t)size != sizeof(dap_hash_fast_t)) {
            PyErr_Format(PyExc_ValueError, "hash must be exactly %zu bytes", sizeof(dap_hash_fast_t));
            return -1;
        }
        memcpy(out, PyBytes_AsString(obj), sizeof(dap_hash_fast_t));
        return 0;
    }
    if (PyUnicode_Check(obj)) {
        const char *hash_str = PyUnicode_AsUTF8(obj);
        if (!hash_str) {
            return -1;
        }
        if (dap_chain_hash_fast_from_str(hash_str, out) != 0) {
            PyErr_SetString(PyExc_ValueError, "Invalid hash string");
            return -1;
        }
        return 0;
    }
    PyErr_SetString(PyExc_TypeError, "hash must be bytes, str, or dap_hash_fast_t capsule");
    return -1;
}

static int cf_common_parse_hash_slow(PyObject *obj, dap_chain_hash_slow_t *out) {
    if (!obj || obj == Py_None) {
        PyErr_SetString(PyExc_TypeError, "hash value is required");
        return -1;
    }
    if (PyCapsule_CheckExact(obj)) {
        dap_chain_hash_slow_t *ptr = (dap_chain_hash_slow_t *)PyCapsule_GetPointer(obj, "dap_chain_hash_slow_t");
        if (!ptr) {
            PyErr_SetString(PyExc_ValueError, "Invalid dap_chain_hash_slow_t capsule");
            return -1;
        }
        *out = *ptr;
        return 0;
    }
    if (PyBytes_Check(obj)) {
        Py_ssize_t size = PyBytes_Size(obj);
        if ((size_t)size != sizeof(dap_chain_hash_slow_t)) {
            PyErr_Format(PyExc_ValueError, "hash must be exactly %zu bytes", sizeof(dap_chain_hash_slow_t));
            return -1;
        }
        memcpy(out, PyBytes_AsString(obj), sizeof(dap_chain_hash_slow_t));
        return 0;
    }
    PyErr_SetString(PyExc_TypeError, "hash must be bytes or dap_chain_hash_slow_t capsule");
    return -1;
}

static int cf_common_parse_addr(PyObject *obj, dap_chain_addr_t *out) {
    if (!obj || obj == Py_None) {
        PyErr_SetString(PyExc_TypeError, "address value is required");
        return -1;
    }
    if (PyCapsule_CheckExact(obj)) {
        dap_chain_addr_t *ptr = (dap_chain_addr_t *)PyCapsule_GetPointer(obj, "dap_chain_addr_t");
        if (!ptr) {
            PyErr_SetString(PyExc_ValueError, "Invalid dap_chain_addr_t capsule");
            return -1;
        }
        *out = *ptr;
        return 0;
    }
    if (PyBytes_Check(obj)) {
        Py_ssize_t size = PyBytes_Size(obj);
        if ((size_t)size != sizeof(dap_chain_addr_t)) {
            PyErr_Format(PyExc_ValueError, "address must be exactly %zu bytes", sizeof(dap_chain_addr_t));
            return -1;
        }
        memcpy(out, PyBytes_AsString(obj), sizeof(dap_chain_addr_t));
        return 0;
    }
    if (PyUnicode_Check(obj)) {
        const char *addr_str = PyUnicode_AsUTF8(obj);
        if (!addr_str) {
            return -1;
        }
        dap_chain_addr_t *addr = dap_chain_addr_from_str(addr_str);
        if (!addr) {
            PyErr_SetString(PyExc_ValueError, "Invalid address string");
            return -1;
        }
        *out = *addr;
        DAP_DELETE(addr);
        return 0;
    }
    PyErr_SetString(PyExc_TypeError, "address must be bytes, str, or dap_chain_addr_t capsule");
    return -1;
}

static int cf_common_parse_uint256(PyObject *obj, uint256_t *out) {
    if (!obj || obj == Py_None) {
        PyErr_SetString(PyExc_TypeError, "uint256 value is required");
        return -1;
    }
    if (PyCapsule_CheckExact(obj)) {
        uint256_t *ptr = (uint256_t *)PyCapsule_GetPointer(obj, "uint256_t");
        if (!ptr) {
            PyErr_SetString(PyExc_ValueError, "Invalid uint256_t capsule");
            return -1;
        }
        *out = *ptr;
        return 0;
    }
    if (PyBytes_Check(obj)) {
        Py_ssize_t size = PyBytes_Size(obj);
        if ((size_t)size != sizeof(uint256_t)) {
            PyErr_Format(PyExc_ValueError, "uint256 value must be exactly %zu bytes", sizeof(uint256_t));
            return -1;
        }
        memcpy(out, PyBytes_AsString(obj), sizeof(uint256_t));
        return 0;
    }
    PyObject *str_obj = PyObject_Str(obj);
    if (!str_obj) {
        return -1;
    }
    const char *str_val = PyUnicode_AsUTF8(str_obj);
    if (!str_val) {
        Py_DECREF(str_obj);
        return -1;
    }
    *out = dap_chain_balance_scan(str_val);
    Py_DECREF(str_obj);
    return 0;
}

static int cf_common_parse_uint128(PyObject *obj, uint128_t *out) {
    if (!obj || obj == Py_None) {
        PyErr_SetString(PyExc_TypeError, "uint128 value is required");
        return -1;
    }
    if (PyCapsule_CheckExact(obj)) {
        uint128_t *ptr = (uint128_t *)PyCapsule_GetPointer(obj, "uint128_t");
        if (!ptr) {
            PyErr_SetString(PyExc_ValueError, "Invalid uint128_t capsule");
            return -1;
        }
        *out = *ptr;
        return 0;
    }
    if (PyBytes_Check(obj)) {
        Py_ssize_t size = PyBytes_Size(obj);
        if ((size_t)size != sizeof(uint128_t)) {
            PyErr_Format(PyExc_ValueError, "uint128 value must be exactly %zu bytes", sizeof(uint128_t));
            return -1;
        }
        memcpy(out, PyBytes_AsString(obj), sizeof(uint128_t));
        return 0;
    }
    if (PyLong_Check(obj)) {
        unsigned long long val = PyLong_AsUnsignedLongLong(obj);
        if (PyErr_Occurred()) {
            return -1;
        }
        *out = dap_chain_uint128_from(val);
        return 0;
    }
    PyErr_SetString(PyExc_TypeError, "uint128 value must be bytes, int, or uint128_t capsule");
    return -1;
}

static void *cf_common_capsule_get_pointer(PyObject *obj, const char *name, const char *error_msg) {
    if (!PyCapsule_CheckExact(obj)) {
        PyErr_SetString(PyExc_TypeError, error_msg);
        return NULL;
    }
    void *ptr = PyCapsule_GetPointer(obj, name);
    if (!ptr) {
        PyErr_SetString(PyExc_ValueError, error_msg);
        return NULL;
    }
    return ptr;
}

// =============================================================================
// BLOCK CALLBACKS
// =============================================================================

static dap_chain_sovereign_tax_info_t *s_block_sovereign_tax_wrapper(dap_chain_net_id_t a_net_id,
                                                                     dap_hash_fast_t *a_pkey_hash) {
    if (!s_block_tax_ctx || !s_block_tax_ctx->callback || !a_pkey_hash) {
        return NULL;
    }

    PyGILState_STATE l_gstate = PyGILState_Ensure();

    PyObject *l_net_id = PyLong_FromUnsignedLongLong(a_net_id.uint64);
    PyObject *l_pkey_hash = PyBytes_FromStringAndSize((const char *)a_pkey_hash, sizeof(dap_hash_fast_t));

    if (!l_net_id || !l_pkey_hash) {
        Py_XDECREF(l_net_id);
        Py_XDECREF(l_pkey_hash);
        PyGILState_Release(l_gstate);
        return NULL;
    }

    PyObject *l_result = PyObject_CallFunctionObjArgs(
        s_block_tax_ctx->callback, l_net_id, l_pkey_hash, s_block_tax_ctx->user_data, NULL
    );

    Py_DECREF(l_net_id);
    Py_DECREF(l_pkey_hash);

    dap_chain_sovereign_tax_info_t *l_tax_info = NULL;

    if (!l_result) {
        log_it(L_ERROR, "Python sovereign tax callback raised an exception");
        PyErr_Print();
        PyGILState_Release(l_gstate);
        return NULL;
    }

    if (l_result == Py_None || !PyObject_IsTrue(l_result)) {
        Py_DECREF(l_result);
        PyGILState_Release(l_gstate);
        return NULL;
    }

    bool l_has_tax = true;
    PyObject *l_addr_obj = NULL;
    PyObject *l_tax_obj = NULL;
    bool l_addr_new_ref = false;
    bool l_tax_new_ref = false;

    if (PyDict_Check(l_result)) {
        PyObject *l_has_tax_obj = PyDict_GetItemString(l_result, "has_tax");
        if (l_has_tax_obj) {
            l_has_tax = PyObject_IsTrue(l_has_tax_obj);
        }
        l_addr_obj = PyDict_GetItemString(l_result, "sovereign_addr");
        l_tax_obj = PyDict_GetItemString(l_result, "sovereign_tax");
    } else if (PyTuple_Check(l_result) || PyList_Check(l_result)) {
        Py_ssize_t l_len = PySequence_Length(l_result);
        if (l_len < 3) {
            log_it(L_ERROR, "Sovereign tax callback returned insufficient tuple size");
            Py_DECREF(l_result);
            PyGILState_Release(l_gstate);
            return NULL;
        }
        PyObject *l_has_tax_obj = PySequence_GetItem(l_result, 0);
        l_addr_obj = PySequence_GetItem(l_result, 1);
        l_tax_obj = PySequence_GetItem(l_result, 2);
        l_addr_new_ref = true;
        l_tax_new_ref = true;
        if (!l_has_tax_obj || !l_addr_obj || !l_tax_obj) {
            Py_XDECREF(l_has_tax_obj);
            Py_XDECREF(l_addr_obj);
            Py_XDECREF(l_tax_obj);
            Py_DECREF(l_result);
            PyGILState_Release(l_gstate);
            return NULL;
        }
        l_has_tax = PyObject_IsTrue(l_has_tax_obj);
        Py_DECREF(l_has_tax_obj);
    } else {
        log_it(L_ERROR, "Sovereign tax callback returned unsupported type");
        Py_DECREF(l_result);
        PyGILState_Release(l_gstate);
        return NULL;
    }

    if (!l_has_tax) {
        if (l_addr_new_ref) {
            Py_XDECREF(l_addr_obj);
        }
        if (l_tax_new_ref) {
            Py_XDECREF(l_tax_obj);
        }
        Py_DECREF(l_result);
        PyGILState_Release(l_gstate);
        return NULL;
    }

    if (!l_addr_obj || !l_tax_obj) {
        log_it(L_ERROR, "Sovereign tax callback missing address or tax");
        if (l_addr_new_ref) {
            Py_XDECREF(l_addr_obj);
        }
        if (l_tax_new_ref) {
            Py_XDECREF(l_tax_obj);
        }
        Py_DECREF(l_result);
        PyGILState_Release(l_gstate);
        return NULL;
    }

    l_tax_info = DAP_NEW_Z(dap_chain_sovereign_tax_info_t);
    if (!l_tax_info) {
        PyErr_SetString(PyExc_MemoryError, "Failed to allocate tax info");
    } else if (cf_common_parse_addr(l_addr_obj, &l_tax_info->sovereign_addr) != 0 ||
               cf_common_parse_uint256(l_tax_obj, &l_tax_info->sovereign_tax) != 0) {
        DAP_DELETE(l_tax_info);
        l_tax_info = NULL;
    } else {
        l_tax_info->has_tax = true;
    }

    if (l_addr_new_ref) {
        Py_XDECREF(l_addr_obj);
    }
    if (l_tax_new_ref) {
        Py_XDECREF(l_tax_obj);
    }

    Py_DECREF(l_result);

    if (PyErr_Occurred()) {
        PyErr_Print();
        PyErr_Clear();
    }

    PyGILState_Release(l_gstate);
    return l_tax_info;
}

// =============================================================================
// RPC CALLBACKS
// =============================================================================

static int s_rpc_consensus_wrapper(dap_chain_rpc_consensus_params_t *params, void *user_data) {
    python_callback_ctx_t *l_ctx = (python_callback_ctx_t *)user_data;
    if (!params || !l_ctx || !l_ctx->callback) {
        return -1;
    }

    PyGILState_STATE l_gstate = PyGILState_Ensure();

    PyObject *l_chain = PyCapsule_New(params->chain, "dap_chain_t", NULL);
    PyObject *l_net = PyCapsule_New(params->net, "dap_chain_net_t", NULL);
    PyObject *l_custom = params->custom_data ? PyCapsule_New(params->custom_data, "void_ptr", NULL) : Py_None;

    if (!l_chain || !l_net) {
        Py_XDECREF(l_chain);
        Py_XDECREF(l_net);
        if (l_custom != Py_None) {
            Py_XDECREF(l_custom);
        }
        PyGILState_Release(l_gstate);
        return -1;
    }

    PyObject *l_result = PyObject_CallFunctionObjArgs(
        l_ctx->callback, l_chain, l_net, l_custom, l_ctx->user_data, NULL
    );

    Py_DECREF(l_chain);
    Py_DECREF(l_net);
    if (l_custom != Py_None) {
        Py_DECREF(l_custom);
    }

    int l_ret = 0;
    if (!l_result) {
        log_it(L_ERROR, "Python RPC consensus callback raised an exception");
        PyErr_Print();
        l_ret = -1;
    } else {
        l_ret = (int)PyLong_AsLong(l_result);
        Py_DECREF(l_result);
        if (PyErr_Occurred()) {
            PyErr_Print();
            PyErr_Clear();
            l_ret = -1;
        }
    }

    PyGILState_Release(l_gstate);
    return l_ret;
}

static int s_rpc_storage_wrapper(dap_chain_rpc_storage_params_t *params, void *user_data) {
    python_callback_ctx_t *l_ctx = (python_callback_ctx_t *)user_data;
    if (!params || !l_ctx || !l_ctx->callback) {
        return -1;
    }

    PyGILState_STATE l_gstate = PyGILState_Ensure();

    PyObject *l_chain = PyCapsule_New(params->chain, "dap_chain_t", NULL);
    PyObject *l_net = PyCapsule_New(params->net, "dap_chain_net_t", NULL);
    PyObject *l_hash = params->block_hash
        ? PyBytes_FromStringAndSize((const char *)params->block_hash, sizeof(dap_hash_fast_t))
        : Py_None;
    PyObject *l_custom = params->custom_data ? PyCapsule_New(params->custom_data, "void_ptr", NULL) : Py_None;

    if (!l_chain || !l_net || !l_hash) {
        Py_XDECREF(l_chain);
        Py_XDECREF(l_net);
        Py_XDECREF(l_hash);
        if (l_custom != Py_None) {
            Py_XDECREF(l_custom);
        }
        PyGILState_Release(l_gstate);
        return -1;
    }

    PyObject *l_result = PyObject_CallFunctionObjArgs(
        l_ctx->callback, l_chain, l_net, l_hash, l_custom, l_ctx->user_data, NULL
    );

    Py_DECREF(l_chain);
    Py_DECREF(l_net);
    Py_XDECREF(l_hash);
    if (l_custom != Py_None) {
        Py_DECREF(l_custom);
    }

    int l_ret = 0;
    if (!l_result) {
        log_it(L_ERROR, "Python RPC storage callback raised an exception");
        PyErr_Print();
        l_ret = -1;
    } else {
        l_ret = (int)PyLong_AsLong(l_result);
        Py_DECREF(l_result);
        if (PyErr_Occurred()) {
            PyErr_Print();
            PyErr_Clear();
            l_ret = -1;
        }
    }

    PyGILState_Release(l_gstate);
    return l_ret;
}

static int s_rpc_service_wrapper(dap_chain_rpc_service_params_t *params, void *user_data) {
    python_callback_ctx_t *l_ctx = (python_callback_ctx_t *)user_data;
    if (!params || !l_ctx || !l_ctx->callback) {
        return -1;
    }

    PyGILState_STATE l_gstate = PyGILState_Ensure();

    PyObject *l_chain = PyCapsule_New(params->chain, "dap_chain_t", NULL);
    PyObject *l_net = PyCapsule_New(params->net, "dap_chain_net_t", NULL);
    PyObject *l_service = params->service_name ? PyUnicode_FromString(params->service_name) : Py_None;
    PyObject *l_custom = params->custom_data ? PyCapsule_New(params->custom_data, "void_ptr", NULL) : Py_None;

    if (!l_chain || !l_net || !l_service) {
        Py_XDECREF(l_chain);
        Py_XDECREF(l_net);
        Py_XDECREF(l_service);
        if (l_custom != Py_None) {
            Py_XDECREF(l_custom);
        }
        PyGILState_Release(l_gstate);
        return -1;
    }

    PyObject *l_result = PyObject_CallFunctionObjArgs(
        l_ctx->callback, l_chain, l_net, l_service, l_custom, l_ctx->user_data, NULL
    );

    Py_DECREF(l_chain);
    Py_DECREF(l_net);
    Py_DECREF(l_service);
    if (l_custom != Py_None) {
        Py_DECREF(l_custom);
    }

    int l_ret = 0;
    if (!l_result) {
        log_it(L_ERROR, "Python RPC service callback raised an exception");
        PyErr_Print();
        l_ret = -1;
    } else {
        l_ret = (int)PyLong_AsLong(l_result);
        Py_DECREF(l_result);
        if (PyErr_Occurred()) {
            PyErr_Print();
            PyErr_Clear();
            l_ret = -1;
        }
    }

    PyGILState_Release(l_gstate);
    return l_ret;
}

static int s_rpc_wallet_wrapper(dap_chain_rpc_wallet_params_t *params, void *user_data) {
    python_callback_ctx_t *l_ctx = (python_callback_ctx_t *)user_data;
    if (!params || !l_ctx || !l_ctx->callback) {
        return -1;
    }

    PyGILState_STATE l_gstate = PyGILState_Ensure();

    PyObject *l_net = PyCapsule_New(params->net, "dap_chain_net_t", NULL);
    PyObject *l_wallet = params->wallet_name ? PyUnicode_FromString(params->wallet_name) : Py_None;
    PyObject *l_value = PyBytes_FromStringAndSize((const char *)&params->value, sizeof(uint256_t));
    PyObject *l_custom = params->custom_data ? PyCapsule_New(params->custom_data, "void_ptr", NULL) : Py_None;

    if (!l_net || !l_wallet || !l_value) {
        Py_XDECREF(l_net);
        Py_XDECREF(l_wallet);
        Py_XDECREF(l_value);
        if (l_custom != Py_None) {
            Py_XDECREF(l_custom);
        }
        PyGILState_Release(l_gstate);
        return -1;
    }

    PyObject *l_result = PyObject_CallFunctionObjArgs(
        l_ctx->callback, l_net, l_wallet, l_value, l_custom, l_ctx->user_data, NULL
    );

    Py_DECREF(l_net);
    Py_DECREF(l_wallet);
    Py_DECREF(l_value);
    if (l_custom != Py_None) {
        Py_DECREF(l_custom);
    }

    int l_ret = 0;
    if (!l_result) {
        log_it(L_ERROR, "Python RPC wallet callback raised an exception");
        PyErr_Print();
        l_ret = -1;
    } else {
        l_ret = (int)PyLong_AsLong(l_result);
        Py_DECREF(l_result);
        if (PyErr_Occurred()) {
            PyErr_Print();
            PyErr_Clear();
            l_ret = -1;
        }
    }

    PyGILState_Release(l_gstate);
    return l_ret;
}

static void s_rpc_tx_notify_wrapper(dap_chain_rpc_tx_notify_params_t *params, void *user_data) {
    python_callback_ctx_t *l_ctx = (python_callback_ctx_t *)user_data;
    if (!params || !l_ctx || !l_ctx->callback) {
        return;
    }

    PyGILState_STATE l_gstate = PyGILState_Ensure();

    PyObject *l_net = PyCapsule_New(params->net, "dap_chain_net_t", NULL);
    PyObject *l_tx_datum = PyCapsule_New(params->tx_datum, "dap_chain_datum_t", NULL);
    PyObject *l_tx_hash = params->tx_hash
        ? PyBytes_FromStringAndSize((const char *)params->tx_hash, sizeof(dap_hash_fast_t))
        : Py_None;
    PyObject *l_ledger = PyCapsule_New(params->ledger, "dap_ledger_t", NULL);
    PyObject *l_custom = params->custom_data ? PyCapsule_New(params->custom_data, "void_ptr", NULL) : Py_None;

    if (!l_net || !l_tx_datum || !l_tx_hash || !l_ledger) {
        Py_XDECREF(l_net);
        Py_XDECREF(l_tx_datum);
        Py_XDECREF(l_tx_hash);
        Py_XDECREF(l_ledger);
        if (l_custom != Py_None) {
            Py_XDECREF(l_custom);
        }
        PyGILState_Release(l_gstate);
        return;
    }

    PyObject *l_result = PyObject_CallFunctionObjArgs(
        l_ctx->callback,
        l_net, l_tx_datum, l_tx_hash, l_ledger, l_custom, l_ctx->user_data, NULL
    );

    Py_DECREF(l_net);
    Py_DECREF(l_tx_datum);
    Py_XDECREF(l_tx_hash);
    Py_DECREF(l_ledger);
    if (l_custom != Py_None) {
        Py_DECREF(l_custom);
    }

    if (!l_result) {
        log_it(L_ERROR, "Python RPC tx notify callback raised an exception");
        PyErr_Print();
    } else {
        Py_DECREF(l_result);
    }

    PyGILState_Release(l_gstate);
}

// =============================================================================
// PYTHON WRAPPERS
// =============================================================================

PyObject *dap_chain_atom_verify_res_to_str_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    int l_res = 0;
    if (!PyArg_ParseTuple(a_args, "i", &l_res)) {
        return NULL;
    }
    const char *l_str = dap_chain_atom_verify_res_to_str((dap_chain_atom_verify_res_t)l_res);
    return PyUnicode_FromString(l_str ? l_str : "unknown");
}

PyObject *dap_chain_block_callbacks_init_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    (void)a_args;
    int l_rc = dap_chain_block_callbacks_init();
    return PyLong_FromLong(l_rc);
}

PyObject *dap_chain_block_callbacks_deinit_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    (void)a_args;
    dap_chain_block_callbacks_deinit();
    cf_common_clear_ctx(&s_block_tax_ctx);
    Py_RETURN_NONE;
}

PyObject *dap_chain_block_callbacks_register_sovereign_tax_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    PyObject *l_callback = NULL;
    PyObject *l_user_data = Py_None;

    if (!PyArg_ParseTuple(a_args, "O|O", &l_callback, &l_user_data)) {
        return NULL;
    }
    if (!PyCallable_Check(l_callback)) {
        PyErr_SetString(PyExc_TypeError, "Callback must be callable");
        return NULL;
    }

    cf_common_clear_ctx(&s_block_tax_ctx);

    python_callback_ctx_t *l_ctx = DAP_NEW_Z(python_callback_ctx_t);
    if (!l_ctx) {
        PyErr_SetString(PyExc_MemoryError, "Failed to allocate callback context");
        return NULL;
    }

    l_ctx->callback = l_callback;
    l_ctx->user_data = l_user_data;
    s_block_tax_ctx = l_ctx;

    dap_chain_block_callbacks_register_sovereign_tax(s_block_sovereign_tax_wrapper);

    if (cf_callbacks_registry_add(CF_CALLBACK_TYPE_BLOCK_SOVEREIGN_TAX, l_callback, l_user_data,
                                  l_ctx, "block_sovereign_tax") != 0) {
        log_it(L_WARNING, "Failed to register block sovereign tax callback in registry");
    }

    Py_RETURN_NONE;
}

PyObject *dap_chain_block_callbacks_get_sovereign_tax_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    unsigned long long l_net_id = 0;
    PyObject *l_hash_obj = NULL;

    if (!PyArg_ParseTuple(a_args, "KO", &l_net_id, &l_hash_obj)) {
        return NULL;
    }

    dap_hash_fast_t l_hash = {0};
    if (cf_common_parse_hash_fast(l_hash_obj, &l_hash) != 0) {
        return NULL;
    }

    dap_chain_net_id_t l_net_id_struct = {.uint64 = l_net_id};
    dap_chain_sovereign_tax_info_t *l_info = dap_chain_block_callbacks_get_sovereign_tax(l_net_id_struct, &l_hash);
    if (!l_info) {
        Py_RETURN_NONE;
    }

    PyObject *l_dict = PyDict_New();
    if (!l_dict) {
        DAP_DELETE(l_info);
        return NULL;
    }

    PyObject *l_has_tax = PyBool_FromLong(l_info->has_tax ? 1 : 0);
    PyObject *l_tax_bytes = PyBytes_FromStringAndSize((const char *)&l_info->sovereign_tax, sizeof(uint256_t));

    dap_chain_addr_t *l_addr_copy = DAP_NEW_Z(dap_chain_addr_t);
    if (!l_addr_copy || !l_has_tax || !l_tax_bytes) {
        Py_XDECREF(l_has_tax);
        Py_XDECREF(l_tax_bytes);
        if (l_addr_copy) {
            DAP_DELETE(l_addr_copy);
        }
        Py_DECREF(l_dict);
        DAP_DELETE(l_info);
        return NULL;
    }

    *l_addr_copy = l_info->sovereign_addr;
    PyObject *l_addr_capsule = PyCapsule_New(l_addr_copy, "dap_chain_addr_t", NULL);
    if (!l_addr_capsule) {
        DAP_DELETE(l_addr_copy);
        Py_DECREF(l_has_tax);
        Py_DECREF(l_tax_bytes);
        Py_DECREF(l_dict);
        DAP_DELETE(l_info);
        return NULL;
    }

    PyDict_SetItemString(l_dict, "has_tax", l_has_tax);
    PyDict_SetItemString(l_dict, "sovereign_addr", l_addr_capsule);
    PyDict_SetItemString(l_dict, "sovereign_tax", l_tax_bytes);

    Py_DECREF(l_has_tax);
    Py_DECREF(l_addr_capsule);
    Py_DECREF(l_tax_bytes);

    DAP_DELETE(l_info);
    return l_dict;
}

PyObject *dap_chain_addr_compare_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    PyObject *l_addr1_obj = NULL;
    PyObject *l_addr2_obj = NULL;

    if (!PyArg_ParseTuple(a_args, "OO", &l_addr1_obj, &l_addr2_obj)) {
        return NULL;
    }

    dap_chain_addr_t l_addr1 = {0};
    dap_chain_addr_t l_addr2 = {0};
    if (cf_common_parse_addr(l_addr1_obj, &l_addr1) != 0 ||
        cf_common_parse_addr(l_addr2_obj, &l_addr2) != 0) {
        return NULL;
    }

    bool l_equal = dap_chain_addr_compare(&l_addr1, &l_addr2);
    return PyBool_FromLong(l_equal ? 1 : 0);
}

PyObject *dap_chain_addr_fill_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    unsigned int l_sign_type = 0;
    PyObject *l_hash_obj = NULL;
    unsigned long long l_net_id = 0;

    if (!PyArg_ParseTuple(a_args, "IOK", &l_sign_type, &l_hash_obj, &l_net_id)) {
        return NULL;
    }

    dap_chain_addr_t *l_addr = DAP_NEW_Z(dap_chain_addr_t);
    if (!l_addr) {
        PyErr_SetString(PyExc_MemoryError, "Failed to allocate address");
        return NULL;
    }

    dap_hash_fast_t l_hash = {0};
    if (cf_common_parse_hash_fast(l_hash_obj, &l_hash) != 0) {
        DAP_DELETE(l_addr);
        return NULL;
    }

    dap_chain_net_id_t l_net_id_struct = {.uint64 = l_net_id};
    dap_sign_type_t l_type = {.raw = l_sign_type};

    dap_chain_addr_fill(l_addr, l_type, &l_hash, l_net_id_struct);

    return PyCapsule_New(l_addr, "dap_chain_addr_t", NULL);
}

PyObject *dap_chain_addr_fill_from_key_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    PyObject *l_key_obj = NULL;
    unsigned long long l_net_id = 0;

    if (!PyArg_ParseTuple(a_args, "OK", &l_key_obj, &l_net_id)) {
        return NULL;
    }

    dap_enc_key_t *l_key = (dap_enc_key_t *)cf_common_capsule_get_pointer(
        l_key_obj, "dap_enc_key_t", "First argument must be a dap_enc_key_t capsule"
    );
    if (!l_key) {
        return NULL;
    }

    dap_chain_addr_t *l_addr = DAP_NEW_Z(dap_chain_addr_t);
    if (!l_addr) {
        PyErr_SetString(PyExc_MemoryError, "Failed to allocate address");
        return NULL;
    }

    dap_chain_net_id_t l_net_id_struct = {.uint64 = l_net_id};
    int l_rc = dap_chain_addr_fill_from_key(l_addr, l_key, l_net_id_struct);

    PyObject *l_tuple = PyTuple_New(2);
    PyTuple_SetItem(l_tuple, 0, PyLong_FromLong(l_rc));
    if (l_rc != 0) {
        DAP_DELETE(l_addr);
        Py_INCREF(Py_None);
        PyTuple_SetItem(l_tuple, 1, Py_None);
        return l_tuple;
    }

    PyObject *l_capsule = PyCapsule_New(l_addr, "dap_chain_addr_t", NULL);
    PyTuple_SetItem(l_tuple, 1, l_capsule);
    return l_tuple;
}

PyObject *dap_chain_addr_fill_from_sign_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    PyObject *l_sign_obj = NULL;
    unsigned long long l_net_id = 0;

    if (!PyArg_ParseTuple(a_args, "OK", &l_sign_obj, &l_net_id)) {
        return NULL;
    }

    dap_sign_t *l_sign = (dap_sign_t *)cf_common_capsule_get_pointer(
        l_sign_obj, "dap_sign_t", "First argument must be a dap_sign_t capsule"
    );
    if (!l_sign) {
        return NULL;
    }

    dap_chain_addr_t *l_addr = DAP_NEW_Z(dap_chain_addr_t);
    if (!l_addr) {
        PyErr_SetString(PyExc_MemoryError, "Failed to allocate address");
        return NULL;
    }

    dap_chain_net_id_t l_net_id_struct = {.uint64 = l_net_id};
    int l_rc = dap_chain_addr_fill_from_sign(l_addr, l_sign, l_net_id_struct);

    PyObject *l_tuple = PyTuple_New(2);
    PyTuple_SetItem(l_tuple, 0, PyLong_FromLong(l_rc));
    if (l_rc != 0) {
        DAP_DELETE(l_addr);
        Py_INCREF(Py_None);
        PyTuple_SetItem(l_tuple, 1, Py_None);
        return l_tuple;
    }

    PyObject *l_capsule = PyCapsule_New(l_addr, "dap_chain_addr_t", NULL);
    PyTuple_SetItem(l_tuple, 1, l_capsule);
    return l_tuple;
}

PyObject *dap_chain_addr_from_str_array_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    const char *l_addr_str = NULL;

    if (!PyArg_ParseTuple(a_args, "s", &l_addr_str)) {
        return NULL;
    }

    dap_chain_addr_t *l_addr_array = NULL;
    size_t l_count = dap_chain_addr_from_str_array(l_addr_str, &l_addr_array);
    if (!l_count || !l_addr_array) {
        PyErr_SetString(PyExc_ValueError, "Failed to parse address list");
        return NULL;
    }

    PyObject *l_list = PyList_New((Py_ssize_t)l_count);
    if (!l_list) {
        DAP_DELETE(l_addr_array);
        return NULL;
    }

    for (size_t i = 0; i < l_count; i++) {
        dap_chain_addr_t *l_copy = DAP_NEW_Z(dap_chain_addr_t);
        if (!l_copy) {
            for (size_t j = 0; j < i; j++) {
                PyObject *l_item = PyList_GetItem(l_list, (Py_ssize_t)j);
                if (l_item && PyCapsule_CheckExact(l_item)) {
                    dap_chain_addr_t *l_addr = PyCapsule_GetPointer(l_item, "dap_chain_addr_t");
                    if (l_addr) {
                        DAP_DELETE(l_addr);
                    }
                }
            }
            Py_DECREF(l_list);
            DAP_DELETE(l_addr_array);
            return NULL;
        }
        *l_copy = l_addr_array[i];
        PyObject *l_capsule = PyCapsule_New(l_copy, "dap_chain_addr_t", NULL);
        if (!l_capsule) {
            DAP_DELETE(l_copy);
            for (size_t j = 0; j < i; j++) {
                PyObject *l_item = PyList_GetItem(l_list, (Py_ssize_t)j);
                if (l_item && PyCapsule_CheckExact(l_item)) {
                    dap_chain_addr_t *l_addr = PyCapsule_GetPointer(l_item, "dap_chain_addr_t");
                    if (l_addr) {
                        DAP_DELETE(l_addr);
                    }
                }
            }
            Py_DECREF(l_list);
            DAP_DELETE(l_addr_array);
            return NULL;
        }
        PyList_SetItem(l_list, (Py_ssize_t)i, l_capsule);
    }

    DAP_DELETE(l_addr_array);
    return l_list;
}

PyObject *dap_chain_addr_to_str_static_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    PyObject *l_addr_obj = NULL;

    if (!PyArg_ParseTuple(a_args, "O", &l_addr_obj)) {
        return NULL;
    }

    dap_chain_addr_t l_addr = {0};
    if (cf_common_parse_addr(l_addr_obj, &l_addr) != 0) {
        return NULL;
    }

    dap_chain_addr_str_t l_addr_str = dap_chain_addr_to_str_static_(&l_addr);
    return PyUnicode_FromString(l_addr_str.s);
}

PyObject *dap_chain_balance_scan_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    PyObject *l_obj = NULL;

    if (!PyArg_ParseTuple(a_args, "O", &l_obj)) {
        return NULL;
    }

    PyObject *l_str_obj = PyObject_Str(l_obj);
    if (!l_str_obj) {
        return NULL;
    }
    const char *l_str = PyUnicode_AsUTF8(l_str_obj);
    if (!l_str) {
        Py_DECREF(l_str_obj);
        return NULL;
    }

    uint256_t l_val = dap_chain_balance_scan(l_str);
    Py_DECREF(l_str_obj);
    return PyBytes_FromStringAndSize((const char *)&l_val, sizeof(uint256_t));
}

PyObject *dap_chain_balance_to_coins_uint64_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    PyObject *l_val_obj = NULL;

    if (!PyArg_ParseTuple(a_args, "O", &l_val_obj)) {
        return NULL;
    }

    uint256_t l_val = {0};
    if (cf_common_parse_uint256(l_val_obj, &l_val) != 0) {
        return NULL;
    }

    uint64_t l_out = dap_chain_balance_to_coins_uint64(l_val);
    return PyLong_FromUnsignedLongLong(l_out);
}

PyObject *dap_chain_hash_slow_kind_check_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    PyObject *l_hash_obj = NULL;
    int l_head = 0;

    if (!PyArg_ParseTuple(a_args, "Oi", &l_hash_obj, &l_head)) {
        return NULL;
    }

    dap_chain_hash_slow_t l_hash = {0};
    if (cf_common_parse_hash_slow(l_hash_obj, &l_hash) != 0) {
        return NULL;
    }

    dap_chain_hash_slow_kind_t l_kind = dap_chain_hash_slow_kind_check(&l_hash, (uint8_t)l_head);
    return PyLong_FromLong((long)l_kind);
}

PyObject *dap_chain_hash_slow_to_str_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    PyObject *l_hash_obj = NULL;

    if (!PyArg_ParseTuple(a_args, "O", &l_hash_obj)) {
        return NULL;
    }

    dap_chain_hash_slow_t l_hash = {0};
    if (cf_common_parse_hash_slow(l_hash_obj, &l_hash) != 0) {
        return NULL;
    }

    size_t l_buf_size = sizeof(dap_chain_hash_slow_t) * 2 + 3;
    char *l_buf = DAP_NEW_Z_SIZE(char, l_buf_size);
    if (!l_buf) {
        PyErr_SetString(PyExc_MemoryError, "Failed to allocate hash buffer");
        return NULL;
    }

    if (!dap_chain_hash_slow_to_str(&l_hash, l_buf, l_buf_size)) {
        DAP_DELETE(l_buf);
        PyErr_SetString(PyExc_RuntimeError, "Failed to convert hash to string");
        return NULL;
    }

    PyObject *l_str = PyUnicode_FromString(l_buf);
    DAP_DELETE(l_buf);
    return l_str;
}

PyObject *dap_chain_hash_slow_to_str_new_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    PyObject *l_hash_obj = NULL;

    if (!PyArg_ParseTuple(a_args, "O", &l_hash_obj)) {
        return NULL;
    }

    dap_chain_hash_slow_t l_hash = {0};
    if (cf_common_parse_hash_slow(l_hash_obj, &l_hash) != 0) {
        return NULL;
    }

    char *l_str = dap_chain_hash_slow_to_str_new(&l_hash);
    if (!l_str) {
        Py_RETURN_NONE;
    }

    PyObject *l_py_str = PyUnicode_FromString(l_str);
    DAP_DELETE(l_str);
    return l_py_str;
}

PyObject *dap_chain_mempool_group_name_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    const char *l_prefix = NULL;
    const char *l_chain_name = NULL;

    if (!PyArg_ParseTuple(a_args, "ss", &l_prefix, &l_chain_name)) {
        return NULL;
    }

    char *l_name = dap_chain_mempool_group_name(l_prefix, l_chain_name);
    if (!l_name) {
        Py_RETURN_NONE;
    }

    PyObject *l_py_name = PyUnicode_FromString(l_name);
    DAP_DELETE(l_name);
    return l_py_name;
}

PyObject *dap_chain_node_role_to_str_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    unsigned int l_role = 0;

    if (!PyArg_ParseTuple(a_args, "I", &l_role)) {
        return NULL;
    }

    dap_chain_node_role_t l_role_union = {.enums = l_role};
    const char *l_str = dap_chain_node_role_to_str(l_role_union);
    return PyUnicode_FromString(l_str ? l_str : "none");
}

PyObject *dap_chain_set_offset_limit_json_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    PyObject *l_json_obj = NULL;
    unsigned long long l_start_in = 0;
    unsigned long long l_end_in = 0;
    unsigned long long l_limit = 0;
    unsigned long long l_offset = 0;
    unsigned long long l_end_count = 0;
    int l_last = 0;

    if (!PyArg_ParseTuple(a_args, "OKKKKKp", &l_json_obj, &l_start_in, &l_end_in,
                          &l_limit, &l_offset, &l_end_count, &l_last)) {
        return NULL;
    }

    dap_json_t *l_json = (dap_json_t *)cf_common_capsule_get_pointer(
        l_json_obj, "dap_json_t", "First argument must be a JSON capsule"
    );
    if (!l_json) {
        return NULL;
    }

    size_t l_start = (size_t)l_start_in;
    size_t l_end = (size_t)l_end_in;

    dap_chain_set_offset_limit_json(l_json, &l_start, &l_end, (size_t)l_limit,
                                    (size_t)l_offset, (size_t)l_end_count, l_last != 0);

    return Py_BuildValue("(KK)", (unsigned long long)l_start, (unsigned long long)l_end);
}

PyObject *dap_chain_srv_str_to_unit_enum_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    const char *l_str = NULL;
    if (!PyArg_ParseTuple(a_args, "s", &l_str)) {
        return NULL;
    }
    dap_chain_srv_unit_enum_t l_unit = dap_chain_srv_str_to_unit_enum(l_str);
    return PyLong_FromUnsignedLong((unsigned long)l_unit);
}

PyObject *dap_chain_srv_uid_from_str_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    const char *l_str = NULL;
    if (!PyArg_ParseTuple(a_args, "s", &l_str)) {
        return NULL;
    }
    dap_chain_srv_uid_t l_uid = dap_chain_srv_uid_from_str(l_str);
    return PyLong_FromUnsignedLongLong(l_uid.uint64);
}

PyObject *dap_chain_srv_unit_enum_to_str_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    unsigned int l_unit = 0;
    if (!PyArg_ParseTuple(a_args, "I", &l_unit)) {
        return NULL;
    }
    const char *l_str = dap_chain_srv_unit_enum_to_str((dap_chain_srv_unit_enum_t)l_unit);
    return PyUnicode_FromString(l_str ? l_str : "UNDEFINED");
}

PyObject *dap_chain_uint128_from_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    unsigned long long l_val = 0;
    if (!PyArg_ParseTuple(a_args, "K", &l_val)) {
        return NULL;
    }
    uint128_t l_out = dap_chain_uint128_from((uint64_t)l_val);
    return PyBytes_FromStringAndSize((const char *)&l_out, sizeof(uint128_t));
}

PyObject *dap_chain_uint256_from_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    unsigned long long l_val = 0;
    if (!PyArg_ParseTuple(a_args, "K", &l_val)) {
        return NULL;
    }
    uint256_t l_out = dap_chain_uint256_from((uint64_t)l_val);
    return PyBytes_FromStringAndSize((const char *)&l_out, sizeof(uint256_t));
}

PyObject *dap_chain_uint256_from_uint128_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    PyObject *l_val_obj = NULL;
    if (!PyArg_ParseTuple(a_args, "O", &l_val_obj)) {
        return NULL;
    }
    uint128_t l_val = {0};
    if (cf_common_parse_uint128(l_val_obj, &l_val) != 0) {
        return NULL;
    }
    uint256_t l_out = dap_chain_uint256_from_uint128(l_val);
    return PyBytes_FromStringAndSize((const char *)&l_out, sizeof(uint256_t));
}

PyObject *dap_chain_net_api_add_reward_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    PyObject *l_net_obj = NULL;
    PyObject *l_reward_obj = NULL;
    unsigned long long l_block_num = 0;

    if (!PyArg_ParseTuple(a_args, "OOK", &l_net_obj, &l_reward_obj, &l_block_num)) {
        return NULL;
    }

    dap_chain_net_t *l_net = (dap_chain_net_t *)cf_common_capsule_get_pointer(
        l_net_obj, "dap_chain_net_t", "First argument must be a network capsule"
    );
    if (!l_net) {
        return NULL;
    }

    uint256_t l_reward = {0};
    if (cf_common_parse_uint256(l_reward_obj, &l_reward) != 0) {
        return NULL;
    }

    int l_rc = dap_chain_net_api_add_reward(l_net, l_reward, (uint64_t)l_block_num);
    return PyLong_FromLong(l_rc);
}

PyObject *dap_chain_net_api_by_id_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    unsigned long long l_net_id = 0;

    if (!PyArg_ParseTuple(a_args, "K", &l_net_id)) {
        return NULL;
    }

    dap_chain_net_id_t l_net_id_struct = {.uint64 = l_net_id};
    dap_chain_net_t *l_net = dap_chain_net_api_by_id(l_net_id_struct);
    if (!l_net) {
        Py_RETURN_NONE;
    }
    return PyCapsule_New(l_net, "dap_chain_net_t", NULL);
}

PyObject *dap_chain_net_api_by_name_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    const char *l_name = NULL;
    if (!PyArg_ParseTuple(a_args, "s", &l_name)) {
        return NULL;
    }
    dap_chain_net_t *l_net = dap_chain_net_api_by_name(l_name);
    if (!l_net) {
        Py_RETURN_NONE;
    }
    return PyCapsule_New(l_net, "dap_chain_net_t", NULL);
}

PyObject *dap_chain_net_api_datum_add_to_mempool_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    PyObject *l_datum_obj = NULL;
    PyObject *l_chain_obj = NULL;
    const char *l_hash_type = NULL;

    if (!PyArg_ParseTuple(a_args, "OO|s", &l_datum_obj, &l_chain_obj, &l_hash_type)) {
        return NULL;
    }

    dap_chain_datum_t *l_datum = (dap_chain_datum_t *)cf_common_capsule_get_pointer(
        l_datum_obj, "dap_chain_datum_t", "First argument must be a datum capsule"
    );
    if (!l_datum) {
        return NULL;
    }

    dap_chain_t *l_chain = (dap_chain_t *)cf_common_capsule_get_pointer(
        l_chain_obj, "dap_chain_t", "Second argument must be a chain capsule"
    );
    if (!l_chain) {
        return NULL;
    }

    char *l_hash = dap_chain_net_api_datum_add_to_mempool(l_datum, l_chain, l_hash_type);
    if (!l_hash) {
        Py_RETURN_NONE;
    }
    return PyUnicode_FromString(l_hash);
}

PyObject *dap_chain_net_api_deinit_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    (void)a_args;
    dap_chain_net_api_deinit();
    Py_RETURN_NONE;
}

PyObject *dap_chain_net_api_get_chain_by_name_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    PyObject *l_net_obj = NULL;
    const char *l_name = NULL;

    if (!PyArg_ParseTuple(a_args, "Os", &l_net_obj, &l_name)) {
        return NULL;
    }

    dap_chain_net_t *l_net = (dap_chain_net_t *)cf_common_capsule_get_pointer(
        l_net_obj, "dap_chain_net_t", "First argument must be a network capsule"
    );
    if (!l_net) {
        return NULL;
    }

    dap_chain_t *l_chain = dap_chain_net_api_get_chain_by_name(l_net, l_name);
    if (!l_chain) {
        Py_RETURN_NONE;
    }
    return PyCapsule_New(l_chain, "dap_chain_t", NULL);
}

PyObject *dap_chain_net_api_get_chain_by_type_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    PyObject *l_net_obj = NULL;
    int l_type = 0;

    if (!PyArg_ParseTuple(a_args, "Oi", &l_net_obj, &l_type)) {
        return NULL;
    }

    dap_chain_net_t *l_net = (dap_chain_net_t *)cf_common_capsule_get_pointer(
        l_net_obj, "dap_chain_net_t", "First argument must be a network capsule"
    );
    if (!l_net) {
        return NULL;
    }

    dap_chain_t *l_chain = dap_chain_net_api_get_chain_by_type(l_net, (dap_chain_type_t)l_type);
    if (!l_chain) {
        Py_RETURN_NONE;
    }
    return PyCapsule_New(l_chain, "dap_chain_t", NULL);
}

PyObject *dap_chain_net_api_get_cur_cell_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    PyObject *l_net_obj = NULL;

    if (!PyArg_ParseTuple(a_args, "O", &l_net_obj)) {
        return NULL;
    }

    dap_chain_net_t *l_net = (dap_chain_net_t *)cf_common_capsule_get_pointer(
        l_net_obj, "dap_chain_net_t", "First argument must be a network capsule"
    );
    if (!l_net) {
        return NULL;
    }

    dap_chain_cell_id_t *l_cell_id = dap_chain_net_api_get_cur_cell(l_net);
    if (!l_cell_id) {
        Py_RETURN_NONE;
    }

    PyObject *l_raw = PyBytes_FromStringAndSize((const char *)l_cell_id->raw, DAP_CHAIN_SHARD_ID_SIZE);
    if (!l_raw) {
        Py_RETURN_NONE;
    }

    PyObject *l_result = Py_BuildValue("{s:K,s:O}", "uint64", l_cell_id->uint64, "raw", l_raw);
    Py_DECREF(l_raw);
    return l_result;
}

PyObject *dap_chain_net_api_get_default_chain_by_type_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    PyObject *l_net_obj = NULL;
    int l_type = 0;

    if (!PyArg_ParseTuple(a_args, "Oi", &l_net_obj, &l_type)) {
        return NULL;
    }

    dap_chain_net_t *l_net = (dap_chain_net_t *)cf_common_capsule_get_pointer(
        l_net_obj, "dap_chain_net_t", "First argument must be a network capsule"
    );
    if (!l_net) {
        return NULL;
    }

    dap_chain_t *l_chain = dap_chain_net_api_get_default_chain_by_type(l_net, (dap_chain_type_t)l_type);
    if (!l_chain) {
        Py_RETURN_NONE;
    }
    return PyCapsule_New(l_chain, "dap_chain_t", NULL);
}

PyObject *dap_chain_net_api_get_load_mode_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    PyObject *l_net_obj = NULL;

    if (!PyArg_ParseTuple(a_args, "O", &l_net_obj)) {
        return NULL;
    }

    dap_chain_net_t *l_net = (dap_chain_net_t *)cf_common_capsule_get_pointer(
        l_net_obj, "dap_chain_net_t", "First argument must be a network capsule"
    );
    if (!l_net) {
        return NULL;
    }

    bool l_mode = dap_chain_net_api_get_load_mode(l_net);
    return PyBool_FromLong(l_mode ? 1 : 0);
}

PyObject *dap_chain_net_api_get_reward_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    PyObject *l_net_obj = NULL;
    unsigned long long l_block_num = 0;

    if (!PyArg_ParseTuple(a_args, "OK", &l_net_obj, &l_block_num)) {
        return NULL;
    }

    dap_chain_net_t *l_net = (dap_chain_net_t *)cf_common_capsule_get_pointer(
        l_net_obj, "dap_chain_net_t", "First argument must be a network capsule"
    );
    if (!l_net) {
        return NULL;
    }

    uint256_t l_reward = dap_chain_net_api_get_reward(l_net, (uint64_t)l_block_num);
    return PyBytes_FromStringAndSize((const char *)&l_reward, sizeof(uint256_t));
}

PyObject *dap_chain_net_api_init_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    (void)a_args;
    int l_rc = dap_chain_net_api_init();
    return PyLong_FromLong(l_rc);
}

PyObject *dap_chain_net_api_register_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    PyObject *l_registry_obj = NULL;
    if (!PyArg_ParseTuple(a_args, "O", &l_registry_obj)) {
        return NULL;
    }

    dap_chain_net_api_registry_t *l_registry = (dap_chain_net_api_registry_t *)cf_common_capsule_get_pointer(
        l_registry_obj, "dap_chain_net_api_registry_t", "Expected dap_chain_net_api_registry_t capsule"
    );
    if (!l_registry) {
        return NULL;
    }

    int l_rc = dap_chain_net_api_register(l_registry);
    return PyLong_FromLong(l_rc);
}

PyObject *dap_chain_rpc_callbacks_deinit_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    (void)a_args;
    dap_chain_rpc_callbacks_deinit();
    cf_common_clear_ctx(&s_rpc_consensus_ctx);
    cf_common_clear_ctx(&s_rpc_storage_ctx);
    cf_common_clear_ctx(&s_rpc_service_ctx);
    cf_common_clear_ctx(&s_rpc_wallet_ctx);
    cf_common_clear_ctx(&s_rpc_tx_notify_ctx);
    Py_RETURN_NONE;
}

PyObject *dap_chain_rpc_callbacks_init_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    (void)a_args;
    int l_rc = dap_chain_rpc_callbacks_init();
    return PyLong_FromLong(l_rc);
}

PyObject *dap_chain_rpc_callbacks_notify_consensus_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    PyObject *l_chain_obj = NULL;
    PyObject *l_net_obj = NULL;
    PyObject *l_custom_obj = Py_None;

    if (!PyArg_ParseTuple(a_args, "OO|O", &l_chain_obj, &l_net_obj, &l_custom_obj)) {
        return NULL;
    }

    dap_chain_t *l_chain = (dap_chain_t *)cf_common_capsule_get_pointer(
        l_chain_obj, "dap_chain_t", "First argument must be a chain capsule"
    );
    if (!l_chain) {
        return NULL;
    }

    dap_chain_net_t *l_net = (dap_chain_net_t *)cf_common_capsule_get_pointer(
        l_net_obj, "dap_chain_net_t", "Second argument must be a network capsule"
    );
    if (!l_net) {
        return NULL;
    }

    void *l_custom = NULL;
    if (l_custom_obj != Py_None) {
        if (!PyCapsule_CheckExact(l_custom_obj)) {
            PyErr_SetString(PyExc_TypeError, "custom_data must be a capsule or None");
            return NULL;
        }
        l_custom = PyCapsule_GetPointer(l_custom_obj, NULL);
    }

    dap_chain_rpc_consensus_params_t l_params = {.chain = l_chain, .net = l_net, .custom_data = l_custom};
    int l_rc = dap_chain_rpc_callbacks_notify_consensus(&l_params);
    return PyLong_FromLong(l_rc);
}

PyObject *dap_chain_rpc_callbacks_notify_service_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    PyObject *l_chain_obj = NULL;
    PyObject *l_net_obj = NULL;
    const char *l_service = NULL;
    PyObject *l_custom_obj = Py_None;

    if (!PyArg_ParseTuple(a_args, "OOs|O", &l_chain_obj, &l_net_obj, &l_service, &l_custom_obj)) {
        return NULL;
    }

    dap_chain_t *l_chain = (dap_chain_t *)cf_common_capsule_get_pointer(
        l_chain_obj, "dap_chain_t", "First argument must be a chain capsule"
    );
    if (!l_chain) {
        return NULL;
    }

    dap_chain_net_t *l_net = (dap_chain_net_t *)cf_common_capsule_get_pointer(
        l_net_obj, "dap_chain_net_t", "Second argument must be a network capsule"
    );
    if (!l_net) {
        return NULL;
    }

    void *l_custom = NULL;
    if (l_custom_obj != Py_None) {
        if (!PyCapsule_CheckExact(l_custom_obj)) {
            PyErr_SetString(PyExc_TypeError, "custom_data must be a capsule or None");
            return NULL;
        }
        l_custom = PyCapsule_GetPointer(l_custom_obj, NULL);
    }

    dap_chain_rpc_service_params_t l_params = {
        .chain = l_chain, .net = l_net, .service_name = l_service, .custom_data = l_custom
    };
    int l_rc = dap_chain_rpc_callbacks_notify_service(&l_params);
    return PyLong_FromLong(l_rc);
}

PyObject *dap_chain_rpc_callbacks_notify_storage_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    PyObject *l_chain_obj = NULL;
    PyObject *l_net_obj = NULL;
    PyObject *l_hash_obj = NULL;
    PyObject *l_custom_obj = Py_None;

    if (!PyArg_ParseTuple(a_args, "OOO|O", &l_chain_obj, &l_net_obj, &l_hash_obj, &l_custom_obj)) {
        return NULL;
    }

    dap_chain_t *l_chain = (dap_chain_t *)cf_common_capsule_get_pointer(
        l_chain_obj, "dap_chain_t", "First argument must be a chain capsule"
    );
    if (!l_chain) {
        return NULL;
    }

    dap_chain_net_t *l_net = (dap_chain_net_t *)cf_common_capsule_get_pointer(
        l_net_obj, "dap_chain_net_t", "Second argument must be a network capsule"
    );
    if (!l_net) {
        return NULL;
    }

    dap_hash_fast_t l_hash = {0};
    if (cf_common_parse_hash_fast(l_hash_obj, &l_hash) != 0) {
        return NULL;
    }

    void *l_custom = NULL;
    if (l_custom_obj != Py_None) {
        if (!PyCapsule_CheckExact(l_custom_obj)) {
            PyErr_SetString(PyExc_TypeError, "custom_data must be a capsule or None");
            return NULL;
        }
        l_custom = PyCapsule_GetPointer(l_custom_obj, NULL);
    }

    dap_chain_rpc_storage_params_t l_params = {
        .chain = l_chain, .net = l_net, .block_hash = &l_hash, .custom_data = l_custom
    };
    int l_rc = dap_chain_rpc_callbacks_notify_storage(&l_params);
    return PyLong_FromLong(l_rc);
}

PyObject *dap_chain_rpc_callbacks_notify_tx_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    PyObject *l_net_obj = NULL;
    PyObject *l_tx_obj = NULL;
    PyObject *l_hash_obj = NULL;
    PyObject *l_ledger_obj = NULL;
    PyObject *l_custom_obj = Py_None;

    if (!PyArg_ParseTuple(a_args, "OOOO|O", &l_net_obj, &l_tx_obj, &l_hash_obj,
                          &l_ledger_obj, &l_custom_obj)) {
        return NULL;
    }

    dap_chain_net_t *l_net = (dap_chain_net_t *)cf_common_capsule_get_pointer(
        l_net_obj, "dap_chain_net_t", "First argument must be a network capsule"
    );
    if (!l_net) {
        return NULL;
    }

    dap_chain_datum_t *l_datum = (dap_chain_datum_t *)cf_common_capsule_get_pointer(
        l_tx_obj, "dap_chain_datum_t", "Second argument must be a datum capsule"
    );
    if (!l_datum) {
        return NULL;
    }

    dap_chain_hash_fast_t l_hash = {0};
    if (cf_common_parse_hash_fast(l_hash_obj, &l_hash) != 0) {
        return NULL;
    }

    dap_ledger_t *l_ledger = (dap_ledger_t *)cf_common_capsule_get_pointer(
        l_ledger_obj, "dap_ledger_t", "Fourth argument must be a ledger capsule"
    );
    if (!l_ledger) {
        return NULL;
    }

    void *l_custom = NULL;
    if (l_custom_obj != Py_None) {
        if (!PyCapsule_CheckExact(l_custom_obj)) {
            PyErr_SetString(PyExc_TypeError, "custom_data must be a capsule or None");
            return NULL;
        }
        l_custom = PyCapsule_GetPointer(l_custom_obj, NULL);
    }

    dap_chain_rpc_tx_notify_params_t l_params = {
        .net = l_net,
        .tx_datum = l_datum,
        .tx_hash = &l_hash,
        .ledger = l_ledger,
        .custom_data = l_custom
    };
    dap_chain_rpc_callbacks_notify_tx(&l_params);
    Py_RETURN_NONE;
}

PyObject *dap_chain_rpc_callbacks_notify_wallet_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    PyObject *l_net_obj = NULL;
    const char *l_wallet_name = NULL;
    PyObject *l_value_obj = NULL;
    PyObject *l_custom_obj = Py_None;

    if (!PyArg_ParseTuple(a_args, "OsO|O", &l_net_obj, &l_wallet_name, &l_value_obj, &l_custom_obj)) {
        return NULL;
    }

    dap_chain_net_t *l_net = (dap_chain_net_t *)cf_common_capsule_get_pointer(
        l_net_obj, "dap_chain_net_t", "First argument must be a network capsule"
    );
    if (!l_net) {
        return NULL;
    }

    uint256_t l_value = {0};
    if (cf_common_parse_uint256(l_value_obj, &l_value) != 0) {
        return NULL;
    }

    void *l_custom = NULL;
    if (l_custom_obj != Py_None) {
        if (!PyCapsule_CheckExact(l_custom_obj)) {
            PyErr_SetString(PyExc_TypeError, "custom_data must be a capsule or None");
            return NULL;
        }
        l_custom = PyCapsule_GetPointer(l_custom_obj, NULL);
    }

    dap_chain_rpc_wallet_params_t l_params = {
        .net = l_net,
        .wallet_name = l_wallet_name,
        .value = l_value,
        .custom_data = l_custom
    };
    int l_rc = dap_chain_rpc_callbacks_notify_wallet(&l_params);
    return PyLong_FromLong(l_rc);
}

PyObject *dap_chain_rpc_callbacks_register_consensus_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    PyObject *l_callback = NULL;
    PyObject *l_user_data = Py_None;

    if (!PyArg_ParseTuple(a_args, "O|O", &l_callback, &l_user_data)) {
        return NULL;
    }

    if (!PyCallable_Check(l_callback)) {
        PyErr_SetString(PyExc_TypeError, "Callback must be callable");
        return NULL;
    }

    python_callback_ctx_t *l_ctx = DAP_NEW_Z(python_callback_ctx_t);
    if (!l_ctx) {
        PyErr_SetString(PyExc_MemoryError, "Failed to allocate callback context");
        return NULL;
    }
    l_ctx->callback = l_callback;
    l_ctx->user_data = l_user_data;

    int l_rc = dap_chain_rpc_callbacks_register_consensus(s_rpc_consensus_wrapper, l_ctx);
    if (l_rc != 0) {
        DAP_DELETE(l_ctx);
        return PyLong_FromLong(l_rc);
    }

    cf_common_clear_ctx(&s_rpc_consensus_ctx);
    s_rpc_consensus_ctx = l_ctx;

    if (cf_callbacks_registry_add(CF_CALLBACK_TYPE_RPC_CONSENSUS, l_callback, l_user_data,
                                  l_ctx, "rpc_consensus") != 0) {
        log_it(L_WARNING, "Failed to register RPC consensus callback in registry");
    }

    return PyLong_FromLong(l_rc);
}

PyObject *dap_chain_rpc_callbacks_register_service_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    PyObject *l_callback = NULL;
    PyObject *l_user_data = Py_None;

    if (!PyArg_ParseTuple(a_args, "O|O", &l_callback, &l_user_data)) {
        return NULL;
    }

    if (!PyCallable_Check(l_callback)) {
        PyErr_SetString(PyExc_TypeError, "Callback must be callable");
        return NULL;
    }

    python_callback_ctx_t *l_ctx = DAP_NEW_Z(python_callback_ctx_t);
    if (!l_ctx) {
        PyErr_SetString(PyExc_MemoryError, "Failed to allocate callback context");
        return NULL;
    }
    l_ctx->callback = l_callback;
    l_ctx->user_data = l_user_data;

    int l_rc = dap_chain_rpc_callbacks_register_service(s_rpc_service_wrapper, l_ctx);
    if (l_rc != 0) {
        DAP_DELETE(l_ctx);
        return PyLong_FromLong(l_rc);
    }

    cf_common_clear_ctx(&s_rpc_service_ctx);
    s_rpc_service_ctx = l_ctx;

    if (cf_callbacks_registry_add(CF_CALLBACK_TYPE_RPC_SERVICE, l_callback, l_user_data,
                                  l_ctx, "rpc_service") != 0) {
        log_it(L_WARNING, "Failed to register RPC service callback in registry");
    }

    return PyLong_FromLong(l_rc);
}

PyObject *dap_chain_rpc_callbacks_register_storage_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    PyObject *l_callback = NULL;
    PyObject *l_user_data = Py_None;

    if (!PyArg_ParseTuple(a_args, "O|O", &l_callback, &l_user_data)) {
        return NULL;
    }

    if (!PyCallable_Check(l_callback)) {
        PyErr_SetString(PyExc_TypeError, "Callback must be callable");
        return NULL;
    }

    python_callback_ctx_t *l_ctx = DAP_NEW_Z(python_callback_ctx_t);
    if (!l_ctx) {
        PyErr_SetString(PyExc_MemoryError, "Failed to allocate callback context");
        return NULL;
    }
    l_ctx->callback = l_callback;
    l_ctx->user_data = l_user_data;

    int l_rc = dap_chain_rpc_callbacks_register_storage(s_rpc_storage_wrapper, l_ctx);
    if (l_rc != 0) {
        DAP_DELETE(l_ctx);
        return PyLong_FromLong(l_rc);
    }

    cf_common_clear_ctx(&s_rpc_storage_ctx);
    s_rpc_storage_ctx = l_ctx;

    if (cf_callbacks_registry_add(CF_CALLBACK_TYPE_RPC_STORAGE, l_callback, l_user_data,
                                  l_ctx, "rpc_storage") != 0) {
        log_it(L_WARNING, "Failed to register RPC storage callback in registry");
    }

    return PyLong_FromLong(l_rc);
}

PyObject *dap_chain_rpc_callbacks_register_tx_notify_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    PyObject *l_callback = NULL;
    PyObject *l_user_data = Py_None;

    if (!PyArg_ParseTuple(a_args, "O|O", &l_callback, &l_user_data)) {
        return NULL;
    }

    if (!PyCallable_Check(l_callback)) {
        PyErr_SetString(PyExc_TypeError, "Callback must be callable");
        return NULL;
    }

    python_callback_ctx_t *l_ctx = DAP_NEW_Z(python_callback_ctx_t);
    if (!l_ctx) {
        PyErr_SetString(PyExc_MemoryError, "Failed to allocate callback context");
        return NULL;
    }
    l_ctx->callback = l_callback;
    l_ctx->user_data = l_user_data;

    int l_rc = dap_chain_rpc_callbacks_register_tx_notify(s_rpc_tx_notify_wrapper, l_ctx);
    if (l_rc != 0) {
        DAP_DELETE(l_ctx);
        return PyLong_FromLong(l_rc);
    }

    cf_common_clear_ctx(&s_rpc_tx_notify_ctx);
    s_rpc_tx_notify_ctx = l_ctx;

    if (cf_callbacks_registry_add(CF_CALLBACK_TYPE_RPC_TX_NOTIFY, l_callback, l_user_data,
                                  l_ctx, "rpc_tx_notify") != 0) {
        log_it(L_WARNING, "Failed to register RPC tx notify callback in registry");
    }

    return PyLong_FromLong(l_rc);
}

PyObject *dap_chain_rpc_callbacks_register_wallet_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    PyObject *l_callback = NULL;
    PyObject *l_user_data = Py_None;

    if (!PyArg_ParseTuple(a_args, "O|O", &l_callback, &l_user_data)) {
        return NULL;
    }

    if (!PyCallable_Check(l_callback)) {
        PyErr_SetString(PyExc_TypeError, "Callback must be callable");
        return NULL;
    }

    python_callback_ctx_t *l_ctx = DAP_NEW_Z(python_callback_ctx_t);
    if (!l_ctx) {
        PyErr_SetString(PyExc_MemoryError, "Failed to allocate callback context");
        return NULL;
    }
    l_ctx->callback = l_callback;
    l_ctx->user_data = l_user_data;

    int l_rc = dap_chain_rpc_callbacks_register_wallet(s_rpc_wallet_wrapper, l_ctx);
    if (l_rc != 0) {
        DAP_DELETE(l_ctx);
        return PyLong_FromLong(l_rc);
    }

    cf_common_clear_ctx(&s_rpc_wallet_ctx);
    s_rpc_wallet_ctx = l_ctx;

    if (cf_callbacks_registry_add(CF_CALLBACK_TYPE_RPC_WALLET, l_callback, l_user_data,
                                  l_ctx, "rpc_wallet") != 0) {
        log_it(L_WARNING, "Failed to register RPC wallet callback in registry");
    }

    return PyLong_FromLong(l_rc);
}

PyObject *dap_chain_type_from_str_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    const char *l_str = NULL;
    if (!PyArg_ParseTuple(a_args, "s", &l_str)) {
        return NULL;
    }
    dap_chain_type_t l_type = dap_chain_type_from_str(l_str);
    return PyLong_FromLong((long)l_type);
}

PyObject *dap_chain_type_to_datum_type_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    int l_type = 0;
    if (!PyArg_ParseTuple(a_args, "i", &l_type)) {
        return NULL;
    }
    uint16_t l_datum_type = dap_chain_type_to_datum_type((dap_chain_type_t)l_type);
    return PyLong_FromUnsignedLong((unsigned long)l_datum_type);
}

PyObject *dap_chain_type_to_str_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    int l_type = 0;
    if (!PyArg_ParseTuple(a_args, "i", &l_type)) {
        return NULL;
    }
    const char *l_str = dap_chain_type_to_str((dap_chain_type_t)l_type);
    if (!l_str) {
        Py_RETURN_NONE;
    }
    return PyUnicode_FromString(l_str);
}

PyObject *dap_datum_type_to_chain_type_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    unsigned int l_type = 0;
    if (!PyArg_ParseTuple(a_args, "I", &l_type)) {
        return NULL;
    }
    dap_chain_type_t l_chain_type = dap_datum_type_to_chain_type((uint16_t)l_type);
    return PyLong_FromLong((long)l_chain_type);
}

PyObject *dap_datum_type_to_str_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    unsigned int l_type = 0;
    if (!PyArg_ParseTuple(a_args, "I", &l_type)) {
        return NULL;
    }
    const char *l_str = dap_datum_type_to_str((uint16_t)l_type);
    if (!l_str) {
        Py_RETURN_NONE;
    }
    return PyUnicode_FromString(l_str);
}

PyObject *dap_cli_error_code_get_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    const char *l_name = NULL;
    if (!PyArg_ParseTuple(a_args, "s", &l_name)) {
        return NULL;
    }
    int l_code = dap_cli_error_code_get(l_name);
    return PyLong_FromLong(l_code);
}

PyObject *dap_cli_error_code_register_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    const char *l_name = NULL;
    int l_code = 0;
    const char *l_desc = NULL;
    if (!PyArg_ParseTuple(a_args, "si|s", &l_name, &l_code, &l_desc)) {
        return NULL;
    }
    int l_rc = dap_cli_error_code_register(l_name, l_code, l_desc);
    return PyLong_FromLong(l_rc);
}

PyObject *dap_cli_error_codes_deinit_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    (void)a_args;
    dap_cli_error_codes_deinit();
    Py_RETURN_NONE;
}

PyObject *dap_cli_error_codes_init_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    (void)a_args;
    dap_cli_error_codes_init();
    Py_RETURN_NONE;
}

// =============================================================================
// MODULE INITIALIZATION
// =============================================================================

int cellframe_common_init(PyObject *module) {
    static PyMethodDef common_methods[] = {
        {"dap_chain_atom_verify_res_to_str", dap_chain_atom_verify_res_to_str_py, METH_VARARGS,
         "Convert atom verify result enum to string"},
        {"dap_chain_block_callbacks_deinit", dap_chain_block_callbacks_deinit_py, METH_NOARGS,
         "Deinitialize block callbacks registry"},
        {"dap_chain_block_callbacks_get_sovereign_tax", dap_chain_block_callbacks_get_sovereign_tax_py, METH_VARARGS,
         "Get sovereign tax info for key"},
        {"dap_chain_block_callbacks_init", dap_chain_block_callbacks_init_py, METH_NOARGS,
         "Initialize block callbacks registry"},
        {"dap_chain_block_callbacks_register_sovereign_tax", dap_chain_block_callbacks_register_sovereign_tax_py, METH_VARARGS,
         "Register sovereign tax callback"},
        {"dap_chain_addr_compare", dap_chain_addr_compare_py, METH_VARARGS,
         "Compare two addresses"},
        {"dap_chain_addr_fill", dap_chain_addr_fill_py, METH_VARARGS,
         "Fill address from sign type and pkey hash"},
        {"dap_chain_addr_fill_from_key", dap_chain_addr_fill_from_key_py, METH_VARARGS,
         "Fill address from enc key"},
        {"dap_chain_addr_fill_from_sign", dap_chain_addr_fill_from_sign_py, METH_VARARGS,
         "Fill address from sign"},
        {"dap_chain_addr_from_str_array", dap_chain_addr_from_str_array_py, METH_VARARGS,
         "Parse comma-separated address list"},
        {"dap_chain_addr_to_str_static_", dap_chain_addr_to_str_static_py, METH_VARARGS,
         "Convert address to string (static)"},
        {"dap_chain_balance_scan", dap_chain_balance_scan_py, METH_VARARGS,
         "Parse balance string into uint256"},
        {"dap_chain_balance_to_coins_uint64", dap_chain_balance_to_coins_uint64_py, METH_VARARGS,
         "Convert uint256 balance to uint64 coins"},
        {"dap_chain_hash_slow_kind_check", dap_chain_hash_slow_kind_check_py, METH_VARARGS,
         "Check slow hash kind"},
        {"dap_chain_hash_slow_to_str", dap_chain_hash_slow_to_str_py, METH_VARARGS,
         "Convert slow hash to string"},
        {"dap_chain_hash_slow_to_str_new", dap_chain_hash_slow_to_str_new_py, METH_VARARGS,
         "Convert slow hash to new string"},
        {"dap_chain_mempool_group_name", dap_chain_mempool_group_name_py, METH_VARARGS,
         "Compose mempool group name"},
        {"dap_chain_node_role_to_str", dap_chain_node_role_to_str_py, METH_VARARGS,
         "Convert node role to string"},
        {"dap_chain_set_offset_limit_json", dap_chain_set_offset_limit_json_py, METH_VARARGS,
         "Set offset/limit in JSON"},
        {"dap_chain_srv_str_to_unit_enum", dap_chain_srv_str_to_unit_enum_py, METH_VARARGS,
         "Convert service unit string to enum"},
        {"dap_chain_srv_uid_from_str", dap_chain_srv_uid_from_str_py, METH_VARARGS,
         "Parse service UID from string"},
        {"dap_chain_srv_unit_enum_to_str", dap_chain_srv_unit_enum_to_str_py, METH_VARARGS,
         "Convert service unit enum to string"},
        {"dap_chain_uint128_from", dap_chain_uint128_from_py, METH_VARARGS,
         "Convert uint64 to uint128"},
        {"dap_chain_uint256_from", dap_chain_uint256_from_py, METH_VARARGS,
         "Convert uint64 to uint256"},
        {"dap_chain_uint256_from_uint128", dap_chain_uint256_from_uint128_py, METH_VARARGS,
         "Convert uint128 to uint256"},
        {"dap_chain_net_api_add_reward", dap_chain_net_api_add_reward_py, METH_VARARGS,
         "Add reward to network"},
        {"dap_chain_net_api_by_id", dap_chain_net_api_by_id_py, METH_VARARGS,
         "Get network by ID"},
        {"dap_chain_net_api_by_name", dap_chain_net_api_by_name_py, METH_VARARGS,
         "Get network by name"},
        {"dap_chain_net_api_datum_add_to_mempool", dap_chain_net_api_datum_add_to_mempool_py, METH_VARARGS,
         "Add datum to mempool"},
        {"dap_chain_net_api_deinit", dap_chain_net_api_deinit_py, METH_NOARGS,
         "Deinitialize network API registry"},
        {"dap_chain_net_api_get_chain_by_name", dap_chain_net_api_get_chain_by_name_py, METH_VARARGS,
         "Get chain by name from network"},
        {"dap_chain_net_api_get_chain_by_type", dap_chain_net_api_get_chain_by_type_py, METH_VARARGS,
         "Get chain by type from network"},
        {"dap_chain_net_api_get_cur_cell", dap_chain_net_api_get_cur_cell_py, METH_VARARGS,
         "Get current cell ID from network"},
        {"dap_chain_net_api_get_default_chain_by_type", dap_chain_net_api_get_default_chain_by_type_py, METH_VARARGS,
         "Get default chain by type"},
        {"dap_chain_net_api_get_load_mode", dap_chain_net_api_get_load_mode_py, METH_VARARGS,
         "Get network load mode"},
        {"dap_chain_net_api_get_reward", dap_chain_net_api_get_reward_py, METH_VARARGS,
         "Get reward for block"},
        {"dap_chain_net_api_init", dap_chain_net_api_init_py, METH_NOARGS,
         "Initialize network API registry"},
        {"dap_chain_net_api_register", dap_chain_net_api_register_py, METH_VARARGS,
         "Register network API functions"},
        {"dap_chain_rpc_callbacks_deinit", dap_chain_rpc_callbacks_deinit_py, METH_NOARGS,
         "Deinitialize RPC callbacks registry"},
        {"dap_chain_rpc_callbacks_init", dap_chain_rpc_callbacks_init_py, METH_NOARGS,
         "Initialize RPC callbacks registry"},
        {"dap_chain_rpc_callbacks_notify_consensus", dap_chain_rpc_callbacks_notify_consensus_py, METH_VARARGS,
         "Notify consensus RPC callback"},
        {"dap_chain_rpc_callbacks_notify_service", dap_chain_rpc_callbacks_notify_service_py, METH_VARARGS,
         "Notify service RPC callback"},
        {"dap_chain_rpc_callbacks_notify_storage", dap_chain_rpc_callbacks_notify_storage_py, METH_VARARGS,
         "Notify storage RPC callback"},
        {"dap_chain_rpc_callbacks_notify_tx", dap_chain_rpc_callbacks_notify_tx_py, METH_VARARGS,
         "Notify tx RPC callback"},
        {"dap_chain_rpc_callbacks_notify_wallet", dap_chain_rpc_callbacks_notify_wallet_py, METH_VARARGS,
         "Notify wallet RPC callback"},
        {"dap_chain_rpc_callbacks_register_consensus", dap_chain_rpc_callbacks_register_consensus_py, METH_VARARGS,
         "Register consensus RPC callback"},
        {"dap_chain_rpc_callbacks_register_service", dap_chain_rpc_callbacks_register_service_py, METH_VARARGS,
         "Register service RPC callback"},
        {"dap_chain_rpc_callbacks_register_storage", dap_chain_rpc_callbacks_register_storage_py, METH_VARARGS,
         "Register storage RPC callback"},
        {"dap_chain_rpc_callbacks_register_tx_notify", dap_chain_rpc_callbacks_register_tx_notify_py, METH_VARARGS,
         "Register tx notify RPC callback"},
        {"dap_chain_rpc_callbacks_register_wallet", dap_chain_rpc_callbacks_register_wallet_py, METH_VARARGS,
         "Register wallet RPC callback"},
        {"dap_chain_type_from_str", dap_chain_type_from_str_py, METH_VARARGS,
         "Convert chain type string to enum"},
        {"dap_chain_type_to_datum_type", dap_chain_type_to_datum_type_py, METH_VARARGS,
         "Convert chain type to datum type"},
        {"dap_chain_type_to_str", dap_chain_type_to_str_py, METH_VARARGS,
         "Convert chain type to string"},
        {"dap_datum_type_to_chain_type", dap_datum_type_to_chain_type_py, METH_VARARGS,
         "Convert datum type to chain type"},
        {"dap_datum_type_to_str", dap_datum_type_to_str_py, METH_VARARGS,
         "Convert datum type to string"},
        {"dap_cli_error_code_get", dap_cli_error_code_get_py, METH_VARARGS,
         "Get CLI error code by name"},
        {"dap_cli_error_code_register", dap_cli_error_code_register_py, METH_VARARGS,
         "Register CLI error code"},
        {"dap_cli_error_codes_deinit", dap_cli_error_codes_deinit_py, METH_NOARGS,
         "Deinitialize CLI error codes"},
        {"dap_cli_error_codes_init", dap_cli_error_codes_init_py, METH_NOARGS,
         "Initialize CLI error codes"},
        {NULL, NULL, 0, NULL}
    };

    for (PyMethodDef *method = common_methods; method->ml_name != NULL; method++) {
        PyObject *func = PyCFunction_New(method, NULL);
        if (!func) {
            return -1;
        }
        if (PyModule_AddObject(module, method->ml_name, func) < 0) {
            Py_DECREF(func);
            return -1;
        }
    }
    return 0;
}
