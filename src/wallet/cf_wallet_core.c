#include "include/cf_wallet_internal.h"

#include "dap_chain_net.h"
#include "dap_sign.h"

#include <stdlib.h>
#include <string.h>

static dap_chain_wallet_t *cf_wallet_get_wallet(PyObject *obj) {
    return (dap_chain_wallet_t *)cf_common_capsule_get_pointer(
        obj, "dap_chain_wallet_t", "First argument must be a dap_chain_wallet_t capsule"
    );
}

static int cf_wallet_parse_net_id(PyObject *obj, dap_chain_net_id_t *out) {
    if (!obj || obj == Py_None) {
        PyErr_SetString(PyExc_TypeError, "net_id is required");
        return -1;
    }
    if (PyLong_Check(obj)) {
        out->uint64 = PyLong_AsUnsignedLongLong(obj);
        if (PyErr_Occurred()) {
            return -1;
        }
        return 0;
    }
    if (PyUnicode_Check(obj)) {
        const char *net_name = PyUnicode_AsUTF8(obj);
        if (!net_name) {
            return -1;
        }
        dap_chain_net_t *net = dap_chain_net_by_name(net_name);
        if (!net) {
            PyErr_Format(PyExc_ValueError, "Network '%s' not found", net_name);
            return -1;
        }
        *out = net->pub.id;
        return 0;
    }
    if (PyCapsule_CheckExact(obj)) {
        dap_chain_net_id_t *ptr = (dap_chain_net_id_t *)PyCapsule_GetPointer(obj, "dap_chain_net_id_t");
        if (!ptr) {
            PyErr_SetString(PyExc_ValueError, "Invalid dap_chain_net_id_t capsule");
            return -1;
        }
        *out = *ptr;
        return 0;
    }
    PyErr_SetString(PyExc_TypeError, "net_id must be int, str, or dap_chain_net_id_t capsule");
    return -1;
}

static int cf_wallet_parse_optional_str(PyObject *obj, const char **out, char **allocated) {
    *out = NULL;
    *allocated = NULL;
    if (!obj || obj == Py_None) {
        return 0;
    }
    if (PyUnicode_Check(obj)) {
        const char *val = PyUnicode_AsUTF8(obj);
        if (!val) {
            return -1;
        }
        *out = val;
        return 0;
    }
    if (PyBytes_Check(obj)) {
        char *bytes = NULL;
        Py_ssize_t size = 0;
        if (PyBytes_AsStringAndSize(obj, &bytes, &size) != 0) {
            return -1;
        }
        char *buf = DAP_NEW_Z_SIZE(char, (size_t)size + 1);
        if (!buf) {
            PyErr_SetString(PyExc_MemoryError, "Failed to allocate string buffer");
            return -1;
        }
        memcpy(buf, bytes, (size_t)size);
        buf[size] = '\0';
        *allocated = buf;
        *out = buf;
        return 0;
    }
    PyErr_SetString(PyExc_TypeError, "value must be str, bytes, or None");
    return -1;
}

static int cf_wallet_parse_seed(PyObject *obj, const void **out_seed, size_t *out_size, void **allocated) {
    *out_seed = NULL;
    *out_size = 0;
    *allocated = NULL;
    if (!obj || obj == Py_None) {
        return 0;
    }
    if (PyBytes_Check(obj)) {
        char *bytes = NULL;
        Py_ssize_t size = 0;
        if (PyBytes_AsStringAndSize(obj, &bytes, &size) != 0) {
            return -1;
        }
        *out_seed = bytes;
        *out_size = (size_t)size;
        return 0;
    }
    if (PyUnicode_Check(obj)) {
        const char *hex = PyUnicode_AsUTF8(obj);
        if (!hex) {
            return -1;
        }
        size_t hex_len = strlen(hex);
        if (hex_len == 0 || (hex_len % 2) != 0) {
            PyErr_SetString(PyExc_ValueError, "Seed hex string must have even length");
            return -1;
        }
        size_t out_len = hex_len / 2;
        uint8_t *buf = DAP_NEW_Z_SIZE(uint8_t, out_len);
        if (!buf) {
            PyErr_SetString(PyExc_MemoryError, "Failed to allocate seed buffer");
            return -1;
        }
        for (size_t i = 0; i < out_len; i++) {
            char hex_byte[3] = {hex[i * 2], hex[i * 2 + 1], '\0'};
            buf[i] = (uint8_t)strtoul(hex_byte, NULL, 16);
        }
        *out_seed = buf;
        *out_size = out_len;
        *allocated = buf;
        return 0;
    }
    PyErr_SetString(PyExc_TypeError, "seed must be bytes, hex str, or None");
    return -1;
}

static int cf_wallet_parse_sig_types(PyObject *obj, dap_sign_type_t **out_types, size_t *out_count) {
    *out_types = NULL;
    *out_count = 0;
    if (!obj || obj == Py_None) {
        PyErr_SetString(PyExc_TypeError, "sig_types is required");
        return -1;
    }
    if (PyLong_Check(obj)) {
        dap_sign_type_t *types = DAP_NEW_Z_SIZE(dap_sign_type_t, sizeof(dap_sign_type_t));
        if (!types) {
            PyErr_SetString(PyExc_MemoryError, "Failed to allocate signature types");
            return -1;
        }
        types[0].raw = (uint32_t)PyLong_AsUnsignedLong(obj);
        if (PyErr_Occurred()) {
            DAP_DELETE(types);
            return -1;
        }
        *out_types = types;
        *out_count = 1;
        return 0;
    }
    if (!PyList_Check(obj) && !PyTuple_Check(obj)) {
        PyErr_SetString(PyExc_TypeError, "sig_types must be int or list/tuple of ints");
        return -1;
    }
    Py_ssize_t count = PySequence_Size(obj);
    if (count <= 0) {
        PyErr_SetString(PyExc_ValueError, "sig_types list must not be empty");
        return -1;
    }
    dap_sign_type_t *types = DAP_NEW_Z_SIZE(dap_sign_type_t, sizeof(dap_sign_type_t) * (size_t)count);
    if (!types) {
        PyErr_SetString(PyExc_MemoryError, "Failed to allocate signature types");
        return -1;
    }
    for (Py_ssize_t i = 0; i < count; i++) {
        PyObject *item = PySequence_GetItem(obj, i);
        if (!item) {
            DAP_DELETE(types);
            return -1;
        }
        if (!PyLong_Check(item)) {
            Py_DECREF(item);
            DAP_DELETE(types);
            PyErr_SetString(PyExc_TypeError, "All signature types must be integers");
            return -1;
        }
        types[i].raw = (uint32_t)PyLong_AsUnsignedLong(item);
        Py_DECREF(item);
        if (PyErr_Occurred()) {
            DAP_DELETE(types);
            return -1;
        }
    }
    *out_types = types;
    *out_count = (size_t)count;
    return 0;
}

static void s_wallet_notify_callback(dap_chain_wallet_t *wallet, void *arg) {
    python_callback_ctx_t *ctx = (python_callback_ctx_t *)arg;
    if (!ctx || !ctx->callback) {
        return;
    }

    PyGILState_STATE gstate = PyGILState_Ensure();
    PyObject *wallet_capsule = PyCapsule_New(wallet, "dap_chain_wallet_t", NULL);
    if (!wallet_capsule) {
        PyGILState_Release(gstate);
        return;
    }

    PyObject *user_data = ctx->user_data ? ctx->user_data : Py_None;
    PyObject *result = PyObject_CallFunctionObjArgs(ctx->callback, wallet_capsule, user_data, NULL);
    Py_DECREF(wallet_capsule);

    if (!result) {
        PyErr_Print();
    } else {
        Py_DECREF(result);
    }
    PyGILState_Release(gstate);
}

PyObject *dap_chain_wallet_init_py(PyObject *self, PyObject *args) {
    (void)self;
    (void)args;
    int rc = dap_chain_wallet_init();
    return PyLong_FromLong(rc);
}

PyObject *dap_chain_wallet_deinit_py(PyObject *self, PyObject *args) {
    (void)self;
    (void)args;
    dap_chain_wallet_deinit();
    Py_RETURN_NONE;
}

PyObject *dap_chain_wallet_get_path_py(PyObject *self, PyObject *args) {
    (void)self;
    PyObject *config_obj = Py_None;
    if (!PyArg_ParseTuple(args, "|O", &config_obj)) {
        return NULL;
    }
    dap_config_t *config = NULL;
    if (config_obj && config_obj != Py_None) {
        config = (dap_config_t *)PyCapsule_GetPointer(config_obj, "dap_config_t");
        if (!config) {
            PyErr_SetString(PyExc_ValueError, "Invalid config capsule");
            return NULL;
        }
    }
    const char *path = dap_chain_wallet_get_path(config);
    if (!path) {
        Py_RETURN_NONE;
    }
    return PyUnicode_FromString(path);
}

PyObject *dap_chain_wallet_create_with_seed_multi_py(PyObject *self, PyObject *args) {
    (void)self;
    const char *wallet_name = NULL;
    const char *wallets_path = NULL;
    PyObject *sig_types_obj = NULL;
    PyObject *seed_obj = Py_None;
    PyObject *pass_obj = Py_None;

    if (!PyArg_ParseTuple(args, "ssO|OO", &wallet_name, &wallets_path, &sig_types_obj, &seed_obj, &pass_obj)) {
        return NULL;
    }

    dap_sign_type_t *sig_types = NULL;
    size_t sig_count = 0;
    if (cf_wallet_parse_sig_types(sig_types_obj, &sig_types, &sig_count) != 0) {
        return NULL;
    }

    const void *seed = NULL;
    size_t seed_size = 0;
    void *seed_alloc = NULL;
    if (cf_wallet_parse_seed(seed_obj, &seed, &seed_size, &seed_alloc) != 0) {
        DAP_DELETE(sig_types);
        return NULL;
    }

    const char *pass = NULL;
    char *pass_alloc = NULL;
    if (cf_wallet_parse_optional_str(pass_obj, &pass, &pass_alloc) != 0) {
        DAP_DELETE(sig_types);
        if (seed_alloc) {
            DAP_DELETE(seed_alloc);
        }
        return NULL;
    }

    dap_chain_wallet_t *wallet = dap_chain_wallet_create_with_seed_multi(
        wallet_name, wallets_path, sig_types, sig_count, seed, seed_size, pass
    );

    DAP_DELETE(sig_types);
    if (seed_alloc) {
        DAP_DELETE(seed_alloc);
    }
    if (pass_alloc) {
        DAP_DELETE(pass_alloc);
    }

    if (!wallet) {
        PyErr_SetString(CellframeWalletError, "Failed to create wallet");
        return NULL;
    }

    return PyCapsule_New(wallet, "dap_chain_wallet_t", NULL);
}

PyObject *dap_chain_wallet_create_with_seed_py(PyObject *self, PyObject *args) {
    (void)self;
    const char *wallet_name = NULL;
    const char *wallets_path = NULL;
    unsigned int sig_type_raw = 0;
    PyObject *seed_obj = Py_None;
    PyObject *pass_obj = Py_None;

    if (!PyArg_ParseTuple(args, "ssI|OO", &wallet_name, &wallets_path, &sig_type_raw, &seed_obj, &pass_obj)) {
        return NULL;
    }

    const void *seed = NULL;
    size_t seed_size = 0;
    void *seed_alloc = NULL;
    if (cf_wallet_parse_seed(seed_obj, &seed, &seed_size, &seed_alloc) != 0) {
        return NULL;
    }

    const char *pass = NULL;
    char *pass_alloc = NULL;
    if (cf_wallet_parse_optional_str(pass_obj, &pass, &pass_alloc) != 0) {
        if (seed_alloc) {
            DAP_DELETE(seed_alloc);
        }
        return NULL;
    }

    dap_sign_type_t sig_type = {.raw = sig_type_raw};
    dap_chain_wallet_t *wallet = dap_chain_wallet_create_with_seed_multi(
        wallet_name, wallets_path, &sig_type, 1, seed, seed_size, pass
    );

    if (seed_alloc) {
        DAP_DELETE(seed_alloc);
    }
    if (pass_alloc) {
        DAP_DELETE(pass_alloc);
    }

    if (!wallet) {
        PyErr_SetString(CellframeWalletError, "Failed to create wallet with seed");
        return NULL;
    }

    return PyCapsule_New(wallet, "dap_chain_wallet_t", NULL);
}

PyObject *dap_chain_wallet_create_with_pass_py(PyObject *self, PyObject *args) {
    (void)self;
    const char *wallet_name = NULL;
    const char *wallets_path = NULL;
    PyObject *pass_obj = NULL;

    if (!PyArg_ParseTuple(args, "ssO", &wallet_name, &wallets_path, &pass_obj)) {
        return NULL;
    }

    const char *pass = NULL;
    char *pass_alloc = NULL;
    if (cf_wallet_parse_optional_str(pass_obj, &pass, &pass_alloc) != 0) {
        return NULL;
    }

    dap_sign_type_t sig_type = {.type = SIG_TYPE_DILITHIUM};
    dap_chain_wallet_t *wallet = dap_chain_wallet_create_with_seed_multi(
        wallet_name, wallets_path, &sig_type, 1, NULL, 0, pass
    );

    if (pass_alloc) {
        DAP_DELETE(pass_alloc);
    }

    if (!wallet) {
        PyErr_SetString(CellframeWalletError, "Failed to create wallet with pass");
        return NULL;
    }

    return PyCapsule_New(wallet, "dap_chain_wallet_t", NULL);
}

PyObject *dap_chain_wallet_create_py(PyObject *self, PyObject *args) {
    (void)self;
    const char *wallet_name = NULL;
    const char *wallets_path = NULL;
    unsigned int sig_type_raw = 0;
    PyObject *pass_obj = Py_None;

    if (!PyArg_ParseTuple(args, "ssI|O", &wallet_name, &wallets_path, &sig_type_raw, &pass_obj)) {
        return NULL;
    }

    const char *pass = NULL;
    char *pass_alloc = NULL;
    if (cf_wallet_parse_optional_str(pass_obj, &pass, &pass_alloc) != 0) {
        return NULL;
    }

    dap_sign_type_t sig_type = {.raw = sig_type_raw};
    dap_chain_wallet_t *wallet = dap_chain_wallet_create(wallet_name, wallets_path, sig_type, pass);

    if (pass_alloc) {
        DAP_DELETE(pass_alloc);
    }

    if (!wallet) {
        PyErr_SetString(CellframeWalletError, "Failed to create wallet");
        return NULL;
    }

    return PyCapsule_New(wallet, "dap_chain_wallet_t", NULL);
}

PyObject *dap_chain_wallet_open_file_py(PyObject *self, PyObject *args) {
    (void)self;
    const char *file_name = NULL;
    PyObject *pass_obj = Py_None;

    if (!PyArg_ParseTuple(args, "s|O", &file_name, &pass_obj)) {
        return NULL;
    }

    const char *pass = NULL;
    char *pass_alloc = NULL;
    if (cf_wallet_parse_optional_str(pass_obj, &pass, &pass_alloc) != 0) {
        return NULL;
    }

    unsigned int status = 0;
    dap_chain_wallet_t *wallet = dap_chain_wallet_open_file(file_name, pass, &status);

    if (pass_alloc) {
        DAP_DELETE(pass_alloc);
    }

    if (!wallet) {
        PyErr_Format(CellframeWalletError, "Failed to open wallet file (status: %u)", status);
        return NULL;
    }

    return PyCapsule_New(wallet, "dap_chain_wallet_t", NULL);
}

static int cf_wallet_build_path(const char *wallets_path, const char *wallet_name, char *out, size_t out_size) {
    const char *ext = ".dwallet";
    size_t name_len = strlen(wallet_name);
    size_t ext_len = strlen(ext);
    int rc = 0;
    if (name_len >= ext_len && strcmp(wallet_name + name_len - ext_len, ext) == 0) {
        rc = snprintf(out, out_size, "%s/%s", wallets_path, wallet_name);
    } else {
        rc = snprintf(out, out_size, "%s/%s%s", wallets_path, wallet_name, ext);
    }
    if (rc < 0 || (size_t)rc >= out_size) {
        return -1;
    }
    return rc;
}

PyObject *dap_chain_wallet_open_py(PyObject *self, PyObject *args) {
    (void)self;
    const char *wallet_name = NULL;
    const char *wallets_path = NULL;
    PyObject *pass_obj = Py_None;

    if (!PyArg_ParseTuple(args, "ss|O", &wallet_name, &wallets_path, &pass_obj)) {
        return NULL;
    }

    if (pass_obj && pass_obj != Py_None) {
        const char *pass = NULL;
        char *pass_alloc = NULL;
        if (cf_wallet_parse_optional_str(pass_obj, &pass, &pass_alloc) != 0) {
            return NULL;
        }
        char file_path[MAX_PATH + 1] = {0};
        if (cf_wallet_build_path(wallets_path, wallet_name, file_path, sizeof(file_path)) < 0) {
            if (pass_alloc) {
                DAP_DELETE(pass_alloc);
            }
            PyErr_SetString(CellframeWalletError, "Failed to build wallet file path");
            return NULL;
        }
        unsigned int status = 0;
        dap_chain_wallet_t *wallet = dap_chain_wallet_open_file(file_path, pass, &status);
        if (pass_alloc) {
            DAP_DELETE(pass_alloc);
        }
        if (!wallet) {
            PyErr_Format(CellframeWalletError, "Failed to open wallet (status: %u)", status);
            return NULL;
        }
        return PyCapsule_New(wallet, "dap_chain_wallet_t", NULL);
    }

    unsigned int status = 0;
    dap_chain_wallet_t *wallet = dap_chain_wallet_open(wallet_name, wallets_path, &status);
    if (!wallet) {
        PyErr_Format(CellframeWalletError, "Failed to open wallet (status: %u)", status);
        return NULL;
    }
    return PyCapsule_New(wallet, "dap_chain_wallet_t", NULL);
}

PyObject *dap_chain_wallet_open_ext_py(PyObject *self, PyObject *args) {
    (void)self;
    const char *wallet_name = NULL;
    const char *wallets_path = NULL;
    PyObject *pass_obj = NULL;

    if (!PyArg_ParseTuple(args, "ssO", &wallet_name, &wallets_path, &pass_obj)) {
        return NULL;
    }

    const char *pass = NULL;
    char *pass_alloc = NULL;
    if (cf_wallet_parse_optional_str(pass_obj, &pass, &pass_alloc) != 0) {
        return NULL;
    }

    char file_path[MAX_PATH + 1] = {0};
    if (cf_wallet_build_path(wallets_path, wallet_name, file_path, sizeof(file_path)) < 0) {
        if (pass_alloc) {
            DAP_DELETE(pass_alloc);
        }
        PyErr_SetString(CellframeWalletError, "Failed to build wallet file path");
        return NULL;
    }

    unsigned int status = 0;
    dap_chain_wallet_t *wallet = dap_chain_wallet_open_file(file_path, pass, &status);
    if (pass_alloc) {
        DAP_DELETE(pass_alloc);
    }
    if (!wallet) {
        PyErr_Format(CellframeWalletError, "Failed to open wallet (status: %u)", status);
        return NULL;
    }
    return PyCapsule_New(wallet, "dap_chain_wallet_t", NULL);
}

PyObject *dap_chain_wallet_save_py(PyObject *self, PyObject *args) {
    (void)self;
    PyObject *wallet_obj = NULL;
    PyObject *pass_obj = Py_None;

    if (!PyArg_ParseTuple(args, "O|O", &wallet_obj, &pass_obj)) {
        return NULL;
    }

    dap_chain_wallet_t *wallet = cf_wallet_get_wallet(wallet_obj);
    if (!wallet) {
        return NULL;
    }

    const char *pass = NULL;
    char *pass_alloc = NULL;
    if (cf_wallet_parse_optional_str(pass_obj, &pass, &pass_alloc) != 0) {
        return NULL;
    }

    int rc = dap_chain_wallet_save(wallet, pass);
    if (pass_alloc) {
        DAP_DELETE(pass_alloc);
    }
    return PyLong_FromLong(rc);
}

PyObject *dap_chain_wallet_save_file_py(PyObject *self, PyObject *args) {
    (void)self;
    PyObject *wallet_obj = NULL;

    if (!PyArg_ParseTuple(args, "O", &wallet_obj)) {
        return NULL;
    }

    dap_chain_wallet_t *wallet = cf_wallet_get_wallet(wallet_obj);
    if (!wallet) {
        return NULL;
    }

    int rc = dap_chain_wallet_save(wallet, NULL);
    return PyLong_FromLong(rc);
}

PyObject *dap_chain_wallet_close_py(PyObject *self, PyObject *args) {
    (void)self;
    PyObject *wallet_obj = NULL;

    if (!PyArg_ParseTuple(args, "O", &wallet_obj)) {
        return NULL;
    }

    dap_chain_wallet_t *wallet = cf_wallet_get_wallet(wallet_obj);
    if (!wallet) {
        return NULL;
    }

    dap_chain_wallet_close(wallet);
    Py_RETURN_NONE;
}

PyObject *dap_chain_wallet_get_addr_py(PyObject *self, PyObject *args) {
    (void)self;
    PyObject *wallet_obj = NULL;
    PyObject *net_id_obj = NULL;

    if (!PyArg_ParseTuple(args, "OO", &wallet_obj, &net_id_obj)) {
        return NULL;
    }

    dap_chain_wallet_t *wallet = cf_wallet_get_wallet(wallet_obj);
    if (!wallet) {
        return NULL;
    }

    dap_chain_net_id_t net_id = {0};
    if (cf_wallet_parse_net_id(net_id_obj, &net_id) != 0) {
        return NULL;
    }

    dap_chain_addr_t *addr = dap_chain_wallet_get_addr(wallet, net_id);
    if (!addr) {
        Py_RETURN_NONE;
    }
    return cf_common_capsule_new_addr(addr);
}

PyObject *dap_cert_to_addr_py(PyObject *self, PyObject *args) {
    (void)self;
    PyObject *certs_obj = NULL;
    PyObject *net_id_obj = NULL;
    size_t key_start_index = 0;

    if (!PyArg_ParseTuple(args, "OnO", &certs_obj, &key_start_index, &net_id_obj)) {
        return NULL;
    }
    if (!PyList_Check(certs_obj) && !PyTuple_Check(certs_obj)) {
        PyErr_SetString(PyExc_TypeError, "certs must be list or tuple");
        return NULL;
    }

    Py_ssize_t count = PySequence_Size(certs_obj);
    if (count <= 0) {
        PyErr_SetString(PyExc_ValueError, "certs list must not be empty");
        return NULL;
    }

    dap_chain_net_id_t net_id = {0};
    if (cf_wallet_parse_net_id(net_id_obj, &net_id) != 0) {
        return NULL;
    }

    dap_cert_t **certs = DAP_NEW_Z_SIZE(dap_cert_t *, sizeof(dap_cert_t *) * (size_t)count);
    if (!certs) {
        PyErr_SetString(PyExc_MemoryError, "Failed to allocate cert list");
        return NULL;
    }

    for (Py_ssize_t i = 0; i < count; i++) {
        PyObject *item = PySequence_GetItem(certs_obj, i);
        if (!item) {
            DAP_DELETE(certs);
            return NULL;
        }
        dap_cert_t *cert = (dap_cert_t *)PyCapsule_GetPointer(item, "dap_cert_t");
        Py_DECREF(item);
        if (!cert) {
            DAP_DELETE(certs);
            PyErr_SetString(PyExc_TypeError, "Certificate must be a dap_cert_t capsule");
            return NULL;
        }
        certs[i] = cert;
    }

    dap_chain_addr_t *addr = dap_cert_to_addr(certs, (size_t)count, key_start_index, net_id);
    DAP_DELETE(certs);

    if (!addr) {
        Py_RETURN_NONE;
    }

    return cf_common_capsule_new_addr(addr);
}

PyObject *dap_chain_wallet_get_certs_number_py(PyObject *self, PyObject *args) {
    (void)self;
    PyObject *wallet_obj = NULL;

    if (!PyArg_ParseTuple(args, "O", &wallet_obj)) {
        return NULL;
    }

    dap_chain_wallet_t *wallet = cf_wallet_get_wallet(wallet_obj);
    if (!wallet) {
        return NULL;
    }

    size_t count = dap_chain_wallet_get_certs_number(wallet);
    return PyLong_FromUnsignedLongLong((unsigned long long)count);
}

PyObject *dap_chain_wallet_get_pkey_py(PyObject *self, PyObject *args) {
    (void)self;
    PyObject *wallet_obj = NULL;
    unsigned int key_idx = 0;

    if (!PyArg_ParseTuple(args, "OI", &wallet_obj, &key_idx)) {
        return NULL;
    }

    dap_chain_wallet_t *wallet = cf_wallet_get_wallet(wallet_obj);
    if (!wallet) {
        return NULL;
    }

    dap_pkey_t *pkey = dap_chain_wallet_get_pkey(wallet, key_idx);
    if (!pkey) {
        Py_RETURN_NONE;
    }
    return PyCapsule_New(pkey, "dap_pkey_t", NULL);
}

PyObject *dap_chain_wallet_get_key_py(PyObject *self, PyObject *args) {
    (void)self;
    PyObject *wallet_obj = NULL;
    unsigned int key_idx = 0;

    if (!PyArg_ParseTuple(args, "OI", &wallet_obj, &key_idx)) {
        return NULL;
    }

    dap_chain_wallet_t *wallet = cf_wallet_get_wallet(wallet_obj);
    if (!wallet) {
        return NULL;
    }

    dap_enc_key_t *key = dap_chain_wallet_get_key(wallet, key_idx);
    if (!key) {
        Py_RETURN_NONE;
    }
    return PyCapsule_New(key, "dap_enc_key_t", NULL);
}

PyObject *dap_chain_wallet_activate_py(PyObject *self, PyObject *args) {
    (void)self;
    const char *wallet_name = NULL;
    const char *wallet_pass = NULL;
    PyObject *path_obj = Py_None;
    unsigned int ttl = 0;

    if (!PyArg_ParseTuple(args, "ss|OI", &wallet_name, &wallet_pass, &path_obj, &ttl)) {
        return NULL;
    }

    if (PyTuple_Size(args) == 3 && PyLong_Check(path_obj)) {
        ttl = (unsigned int)PyLong_AsUnsignedLong(path_obj);
        if (PyErr_Occurred()) {
            return NULL;
        }
        path_obj = Py_None;
    }

    const char *path = NULL;
    char *path_alloc = NULL;
    if (cf_wallet_parse_optional_str(path_obj, &path, &path_alloc) != 0) {
        return NULL;
    }

    int rc = dap_chain_wallet_activate(
        wallet_name,
        (ssize_t)strlen(wallet_name),
        path,
        wallet_pass,
        (ssize_t)strlen(wallet_pass),
        ttl
    );

    if (path_alloc) {
        DAP_DELETE(path_alloc);
    }

    return PyLong_FromLong(rc);
}

PyObject *dap_chain_wallet_deactivate_py(PyObject *self, PyObject *args) {
    (void)self;
    const char *wallet_name = NULL;

    if (!PyArg_ParseTuple(args, "s", &wallet_name)) {
        return NULL;
    }

    int rc = dap_chain_wallet_deactivate(wallet_name, (ssize_t)strlen(wallet_name));
    return PyLong_FromLong(rc);
}

PyObject *dap_chain_wallet_check_sign_py(PyObject *self, PyObject *args) {
    (void)self;
    PyObject *wallet_obj = NULL;

    if (!PyArg_ParseTuple(args, "O", &wallet_obj)) {
        return NULL;
    }

    dap_chain_wallet_t *wallet = cf_wallet_get_wallet(wallet_obj);
    if (!wallet) {
        return NULL;
    }

    const char *msg = dap_chain_wallet_check_sign(wallet);
    if (!msg) {
        Py_RETURN_NONE;
    }
    return PyUnicode_FromString(msg);
}

PyObject *dap_chain_wallet_addr_cache_get_name_py(PyObject *self, PyObject *args) {
    (void)self;
    PyObject *addr_obj = NULL;

    if (!PyArg_ParseTuple(args, "O", &addr_obj)) {
        return NULL;
    }

    dap_chain_addr_t addr = {0};
    if (cf_common_parse_addr(addr_obj, &addr) != 0) {
        return NULL;
    }

    const char *name = dap_chain_wallet_addr_cache_get_name(&addr);
    if (!name) {
        Py_RETURN_NONE;
    }
    return PyUnicode_FromString(name);
}

PyObject *dap_chain_wallet_add_wallet_opened_notify_py(PyObject *self, PyObject *args) {
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

    python_callback_ctx_t *ctx = DAP_NEW_Z(python_callback_ctx_t);
    if (!ctx) {
        PyErr_SetString(PyExc_MemoryError, "Failed to allocate callback context");
        return NULL;
    }

    ctx->callback = callback;
    ctx->user_data = (user_data != Py_None) ? user_data : NULL;

    int rc = dap_chain_wallet_add_wallet_opened_notify(s_wallet_notify_callback, ctx);
    if (rc != 0) {
        DAP_DELETE(ctx);
        return PyLong_FromLong(rc);
    }

    if (cf_callbacks_registry_add(CF_CALLBACK_TYPE_WALLET_OPENED, callback, ctx->user_data, ctx, NULL) != 0) {
        log_it(L_WARNING, "Failed to register wallet opened callback in registry");
    }

    return PyLong_FromLong(rc);
}

PyObject *dap_chain_wallet_add_wallet_created_notify_py(PyObject *self, PyObject *args) {
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

    python_callback_ctx_t *ctx = DAP_NEW_Z(python_callback_ctx_t);
    if (!ctx) {
        PyErr_SetString(PyExc_MemoryError, "Failed to allocate callback context");
        return NULL;
    }

    ctx->callback = callback;
    ctx->user_data = (user_data != Py_None) ? user_data : NULL;

    int rc = dap_chain_wallet_add_wallet_created_notify(s_wallet_notify_callback, ctx);
    if (rc != 0) {
        DAP_DELETE(ctx);
        return PyLong_FromLong(rc);
    }

    if (cf_callbacks_registry_add(CF_CALLBACK_TYPE_WALLET_CREATED, callback, ctx->user_data, ctx, NULL) != 0) {
        log_it(L_WARNING, "Failed to register wallet created callback in registry");
    }

    return PyLong_FromLong(rc);
}

PyObject *dap_chain_wallet_get_pkey_hash_py(PyObject *self, PyObject *args) {
    (void)self;
    PyObject *wallet_obj = NULL;

    if (!PyArg_ParseTuple(args, "O", &wallet_obj)) {
        return NULL;
    }

    dap_chain_wallet_t *wallet = cf_wallet_get_wallet(wallet_obj);
    if (!wallet) {
        return NULL;
    }

    dap_hash_fast_t hash = {0};
    int rc = dap_chain_wallet_get_pkey_hash(wallet, &hash);
    if (rc != 0) {
        PyErr_Format(CellframeWalletError, "Failed to get public key hash (error: %d)", rc);
        return NULL;
    }

    char hash_str[DAP_CHAIN_HASH_FAST_STR_SIZE] = {0};
    if (dap_chain_hash_fast_to_str(&hash, hash_str, sizeof(hash_str)) <= 0) {
        PyErr_SetString(CellframeWalletError, "Failed to convert public key hash");
        return NULL;
    }

    return PyUnicode_FromString(hash_str);
}

PyObject *dap_chain_wallet_get_pkey_str_py(PyObject *self, PyObject *args) {
    (void)self;
    PyObject *wallet_obj = NULL;
    PyObject *type_obj = Py_None;

    if (!PyArg_ParseTuple(args, "O|O", &wallet_obj, &type_obj)) {
        return NULL;
    }

    dap_chain_wallet_t *wallet = cf_wallet_get_wallet(wallet_obj);
    if (!wallet) {
        return NULL;
    }

    const char *str_type = NULL;
    char *type_alloc = NULL;
    if (cf_wallet_parse_optional_str(type_obj, &str_type, &type_alloc) != 0) {
        return NULL;
    }

    char *pkey_str = dap_chain_wallet_get_pkey_str(wallet, str_type);
    if (type_alloc) {
        DAP_DELETE(type_alloc);
    }
    if (!pkey_str) {
        Py_RETURN_NONE;
    }

    PyObject *result = PyUnicode_FromString(pkey_str);
    DAP_DELETE(pkey_str);
    return result;
}

PyObject *dap_chain_wallet_get_local_addr_py(PyObject *self, PyObject *args) {
    (void)self;
    (void)args;
    dap_list_t *list = dap_chain_wallet_get_local_addr();
    PyObject *py_list = PyList_New(0);
    if (!py_list) {
        return NULL;
    }

    for (dap_list_t *it = list; it; it = it->next) {
        dap_chain_addr_t *addr = (dap_chain_addr_t *)it->data;
        PyObject *capsule = cf_common_capsule_new_addr(addr);
        if (!capsule) {
            dap_list_free(list);
            Py_DECREF(py_list);
            return NULL;
        }
        PyList_Append(py_list, capsule);
        Py_DECREF(capsule);
    }

    dap_list_free(list);
    return py_list;
}

PyMethodDef *cellframe_wallet_core_get_methods(void) {
    static PyMethodDef methods[] = {
        {"dap_chain_wallet_init", dap_chain_wallet_init_py, METH_NOARGS, "Initialize wallet subsystem"},
        {"dap_chain_wallet_deinit", dap_chain_wallet_deinit_py, METH_NOARGS, "Deinitialize wallet subsystem"},
        {"dap_chain_wallet_get_path", dap_chain_wallet_get_path_py, METH_VARARGS, "Get wallet storage path"},
        {"dap_chain_wallet_create_with_seed_multi", dap_chain_wallet_create_with_seed_multi_py, METH_VARARGS, "Create wallet with multiple signatures"},
        {"dap_chain_wallet_create_with_seed", dap_chain_wallet_create_with_seed_py, METH_VARARGS, "Create wallet with seed"},
        {"dap_chain_wallet_create_with_pass", dap_chain_wallet_create_with_pass_py, METH_VARARGS, "Create wallet with pass"},
        {"dap_chain_wallet_create", dap_chain_wallet_create_py, METH_VARARGS, "Create wallet"},
        {"dap_chain_wallet_open_file", dap_chain_wallet_open_file_py, METH_VARARGS, "Open wallet by file path"},
        {"dap_chain_wallet_open", dap_chain_wallet_open_py, METH_VARARGS, "Open wallet"},
        {"dap_chain_wallet_open_ext", dap_chain_wallet_open_ext_py, METH_VARARGS, "Open wallet with explicit pass"},
        {"dap_chain_wallet_save", dap_chain_wallet_save_py, METH_VARARGS, "Save wallet"},
        {"dap_chain_wallet_save_file", dap_chain_wallet_save_file_py, METH_VARARGS, "Save wallet to file"},
        {"dap_chain_wallet_close", dap_chain_wallet_close_py, METH_VARARGS, "Close wallet"},
        {"dap_cert_to_addr", dap_cert_to_addr_py, METH_VARARGS, "Convert certs to address"},
        {"dap_chain_wallet_get_addr", dap_chain_wallet_get_addr_py, METH_VARARGS, "Get wallet address"},
        {"dap_chain_wallet_get_certs_number", dap_chain_wallet_get_certs_number_py, METH_VARARGS, "Get number of certs in wallet"},
        {"dap_chain_wallet_get_pkey", dap_chain_wallet_get_pkey_py, METH_VARARGS, "Get wallet public key"},
        {"dap_chain_wallet_get_key", dap_chain_wallet_get_key_py, METH_VARARGS, "Get wallet key"},
        {"dap_chain_wallet_activate", dap_chain_wallet_activate_py, METH_VARARGS, "Activate wallet"},
        {"dap_chain_wallet_deactivate", dap_chain_wallet_deactivate_py, METH_VARARGS, "Deactivate wallet"},
        {"dap_chain_wallet_check_sign", dap_chain_wallet_check_sign_py, METH_VARARGS, "Check wallet signatures"},
        {"dap_chain_wallet_addr_cache_get_name", dap_chain_wallet_addr_cache_get_name_py, METH_VARARGS, "Get wallet name by address"},
        {"dap_chain_wallet_add_wallet_opened_notify", dap_chain_wallet_add_wallet_opened_notify_py, METH_VARARGS, "Add wallet opened notify callback"},
        {"dap_chain_wallet_add_wallet_created_notify", dap_chain_wallet_add_wallet_created_notify_py, METH_VARARGS, "Add wallet created notify callback"},
        {"dap_chain_wallet_get_pkey_hash", dap_chain_wallet_get_pkey_hash_py, METH_VARARGS, "Get wallet public key hash"},
        {"dap_chain_wallet_get_pkey_str", dap_chain_wallet_get_pkey_str_py, METH_VARARGS, "Get wallet public key string"},
        {"dap_chain_wallet_get_local_addr", dap_chain_wallet_get_local_addr_py, METH_NOARGS, "Get local wallet addresses"},
        {NULL, NULL, 0, NULL}
    };
    return methods;
}
