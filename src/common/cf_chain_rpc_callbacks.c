#include "cf_common_utils.h"
#include "dap_chain_rpc_callbacks.h"

#define LOG_TAG "python_cellframe_common"

static python_callback_ctx_t *s_rpc_consensus_ctx = NULL;
static python_callback_ctx_t *s_rpc_storage_ctx = NULL;
static python_callback_ctx_t *s_rpc_service_ctx = NULL;
static python_callback_ctx_t *s_rpc_wallet_ctx = NULL;
static python_callback_ctx_t *s_rpc_tx_notify_ctx = NULL;

static int s_parse_custom_capsule(PyObject *obj, void **out) {
    if (!out) {
        PyErr_SetString(PyExc_RuntimeError, "custom_data output pointer is NULL");
        return -1;
    }
    *out = NULL;
    if (!obj || obj == Py_None) {
        return 0;
    }
    if (!PyCapsule_CheckExact(obj)) {
        PyErr_SetString(PyExc_TypeError, "custom_data must be a capsule or None");
        return -1;
    }
    const char *name = PyCapsule_GetName(obj);
    if (PyErr_Occurred()) {
        return -1;
    }
    void *ptr = PyCapsule_GetPointer(obj, name ? name : NULL);
    if (!ptr) {
        return -1;
    }
    *out = ptr;
    return 0;
}

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

    if (!l_result) {
        log_it(L_ERROR, "Python RPC consensus callback raised an exception");
        PyErr_Print();
        PyGILState_Release(l_gstate);
        return -1;
    }

    int l_rc = (int)PyLong_AsLong(l_result);
    Py_DECREF(l_result);

    PyGILState_Release(l_gstate);
    return l_rc;
}

static int s_rpc_storage_wrapper(dap_chain_rpc_storage_params_t *params, void *user_data) {
    python_callback_ctx_t *l_ctx = (python_callback_ctx_t *)user_data;
    if (!params || !l_ctx || !l_ctx->callback) {
        return -1;
    }

    PyGILState_STATE l_gstate = PyGILState_Ensure();

    PyObject *l_chain = PyCapsule_New(params->chain, "dap_chain_t", NULL);
    PyObject *l_net = PyCapsule_New(params->net, "dap_chain_net_t", NULL);
    PyObject *l_block_hash = params->block_hash
        ? PyBytes_FromStringAndSize((const char *)params->block_hash, sizeof(dap_hash_fast_t))
        : Py_None;
    PyObject *l_custom = params->custom_data ? PyCapsule_New(params->custom_data, "void_ptr", NULL) : Py_None;

    if (!l_chain || !l_net || !l_block_hash) {
        Py_XDECREF(l_chain);
        Py_XDECREF(l_net);
        Py_XDECREF(l_block_hash);
        if (l_custom != Py_None) {
            Py_XDECREF(l_custom);
        }
        PyGILState_Release(l_gstate);
        return -1;
    }

    PyObject *l_result = PyObject_CallFunctionObjArgs(
        l_ctx->callback, l_chain, l_net, l_block_hash, l_custom, l_ctx->user_data, NULL
    );

    Py_DECREF(l_chain);
    Py_DECREF(l_net);
    Py_XDECREF(l_block_hash);
    if (l_custom != Py_None) {
        Py_DECREF(l_custom);
    }

    if (!l_result) {
        log_it(L_ERROR, "Python RPC storage callback raised an exception");
        PyErr_Print();
        PyGILState_Release(l_gstate);
        return -1;
    }

    int l_rc = (int)PyLong_AsLong(l_result);
    Py_DECREF(l_result);

    PyGILState_Release(l_gstate);
    return l_rc;
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
    Py_XDECREF(l_service);
    if (l_custom != Py_None) {
        Py_DECREF(l_custom);
    }

    if (!l_result) {
        log_it(L_ERROR, "Python RPC service callback raised an exception");
        PyErr_Print();
        PyGILState_Release(l_gstate);
        return -1;
    }

    int l_rc = (int)PyLong_AsLong(l_result);
    Py_DECREF(l_result);

    PyGILState_Release(l_gstate);
    return l_rc;
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
    Py_XDECREF(l_wallet);
    Py_XDECREF(l_value);
    if (l_custom != Py_None) {
        Py_DECREF(l_custom);
    }

    if (!l_result) {
        log_it(L_ERROR, "Python RPC wallet callback raised an exception");
        PyErr_Print();
        PyGILState_Release(l_gstate);
        return -1;
    }

    int l_rc = (int)PyLong_AsLong(l_result);
    Py_DECREF(l_result);

    PyGILState_Release(l_gstate);
    return l_rc;
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
    if (s_parse_custom_capsule(l_custom_obj, &l_custom) != 0) {
        return NULL;
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
    if (s_parse_custom_capsule(l_custom_obj, &l_custom) != 0) {
        return NULL;
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
    if (s_parse_custom_capsule(l_custom_obj, &l_custom) != 0) {
        return NULL;
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
    if (s_parse_custom_capsule(l_custom_obj, &l_custom) != 0) {
        return NULL;
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
    if (s_parse_custom_capsule(l_custom_obj, &l_custom) != 0) {
        return NULL;
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

int cf_common_register_chain_rpc_callbacks(PyObject *module) {
    static PyMethodDef methods[] = {
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
        {NULL, NULL, 0, NULL}
    };

    return cf_common_register_methods(module, methods);
}
