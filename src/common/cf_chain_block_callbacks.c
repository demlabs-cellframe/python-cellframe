#include "cf_common_utils.h"
#include "dap_chain_block_callbacks.h"
#include <pthread.h>

#define LOG_TAG "python_cellframe_common"

static python_callback_ctx_t *s_block_tax_ctx = NULL;

typedef struct cf_tax_info_node {
    dap_chain_sovereign_tax_info_t *info;
    struct cf_tax_info_node *next;
} cf_tax_info_node_t;

static pthread_mutex_t s_tax_info_lock = PTHREAD_MUTEX_INITIALIZER;
static cf_tax_info_node_t *s_tax_info_head = NULL;

static void s_tax_info_track(dap_chain_sovereign_tax_info_t *info) {
    if (!info) {
        return;
    }
    cf_tax_info_node_t *node = DAP_NEW_Z(cf_tax_info_node_t);
    if (!node) {
        log_it(L_WARNING, "Failed to track sovereign tax info");
        return;
    }
    node->info = info;
    pthread_mutex_lock(&s_tax_info_lock);
    node->next = s_tax_info_head;
    s_tax_info_head = node;
    pthread_mutex_unlock(&s_tax_info_lock);
}

static bool s_tax_info_untrack_and_free(dap_chain_sovereign_tax_info_t *info) {
    if (!info) {
        return false;
    }
    cf_tax_info_node_t *found = NULL;
    pthread_mutex_lock(&s_tax_info_lock);
    cf_tax_info_node_t *prev = NULL;
    cf_tax_info_node_t *cur = s_tax_info_head;
    while (cur) {
        if (cur->info == info) {
            if (prev) {
                prev->next = cur->next;
            } else {
                s_tax_info_head = cur->next;
            }
            found = cur;
            break;
        }
        prev = cur;
        cur = cur->next;
    }
    pthread_mutex_unlock(&s_tax_info_lock);
    if (found) {
        DAP_DELETE(found->info);
        DAP_DELETE(found);
        return true;
    }
    return false;
}

static void s_tax_info_cleanup_all(void) {
    pthread_mutex_lock(&s_tax_info_lock);
    cf_tax_info_node_t *cur = s_tax_info_head;
    s_tax_info_head = NULL;
    pthread_mutex_unlock(&s_tax_info_lock);
    while (cur) {
        cf_tax_info_node_t *next = cur->next;
        DAP_DELETE(cur->info);
        DAP_DELETE(cur);
        cur = next;
    }
}

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
        s_tax_info_track(l_tax_info);
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
    s_tax_info_cleanup_all();
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
        s_tax_info_untrack_and_free(l_info);
        return NULL;
    }

    PyObject *l_has_tax = PyBool_FromLong(l_info->has_tax ? 1 : 0);
    PyObject *l_tax_bytes = PyBytes_FromStringAndSize((const char *)&l_info->sovereign_tax, sizeof(uint256_t));

    dap_chain_addr_t *l_addr_copy = DAP_NEW_Z(dap_chain_addr_t);
    if (!l_addr_copy || !l_has_tax || !l_tax_bytes) {
        if (!l_addr_copy) {
            PyErr_NoMemory();
        }
        Py_XDECREF(l_has_tax);
        Py_XDECREF(l_tax_bytes);
        if (l_addr_copy) {
            DAP_DELETE(l_addr_copy);
        }
        Py_DECREF(l_dict);
        s_tax_info_untrack_and_free(l_info);
        return NULL;
    }

    *l_addr_copy = l_info->sovereign_addr;
    PyObject *l_addr_capsule = cf_common_capsule_new_addr(l_addr_copy);
    if (!l_addr_capsule) {
        Py_DECREF(l_has_tax);
        Py_DECREF(l_tax_bytes);
        Py_DECREF(l_dict);
        s_tax_info_untrack_and_free(l_info);
        return NULL;
    }

    PyDict_SetItemString(l_dict, "has_tax", l_has_tax);
    PyDict_SetItemString(l_dict, "sovereign_addr", l_addr_capsule);
    PyDict_SetItemString(l_dict, "sovereign_tax", l_tax_bytes);

    Py_DECREF(l_has_tax);
    Py_DECREF(l_addr_capsule);
    Py_DECREF(l_tax_bytes);

    s_tax_info_untrack_and_free(l_info);
    return l_dict;
}

int cf_common_register_chain_block_callbacks(PyObject *module) {
    static PyMethodDef methods[] = {
        {"dap_chain_block_callbacks_deinit", dap_chain_block_callbacks_deinit_py, METH_NOARGS,
         "Deinitialize block callbacks registry"},
        {"dap_chain_block_callbacks_get_sovereign_tax", dap_chain_block_callbacks_get_sovereign_tax_py, METH_VARARGS,
         "Get sovereign tax info for key"},
        {"dap_chain_block_callbacks_init", dap_chain_block_callbacks_init_py, METH_NOARGS,
         "Initialize block callbacks registry"},
        {"dap_chain_block_callbacks_register_sovereign_tax", dap_chain_block_callbacks_register_sovereign_tax_py, METH_VARARGS,
         "Register sovereign tax callback"},
        {NULL, NULL, 0, NULL}
    };

    return cf_common_register_methods(module, methods);
}
