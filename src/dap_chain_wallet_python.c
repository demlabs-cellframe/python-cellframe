#include "dap_chain_wallet_python.h"

int dap_chain_wallet_init_py(void){
    return dap_chain_wallet_init();
}
void dap_chain_wallet_deinit_py(void){
    dap_chain_wallet_deinit();
}

PyObject *dap_chain_wallet_get_path_py(PyObject *self, PyObject *argv){
    (void)self;
    (void)argv;
    const char *path = dap_chain_wallet_get_path(g_config);
    return Py_BuildValue("(s)", path);
}

PyObject *dap_chain_wallet_create_with_seed_py(PyObject *self, PyObject *argv){
    (void)self;
    const char *wallet_name;
    const char *path_wallets;
    PyObject *obj_sig_type;
    PyObject *obj_seed;
    if (!PyArg_ParseTuple(argv, "ssOO", &wallet_name, &path_wallets, &obj_sig_type, &obj_seed))
        return NULL;
    if (PyBytes_Check(obj_seed)){
        PyErr_SetString(PyExc_TypeError, "Fourth argument to not have a Bytes object type");
        return NULL;
    }
    void *seed = (void *)PyBytes_AsString(obj_seed);
    size_t seed_size = PyBytes_Size(obj_seed);
    PyObject *obj_wallet = _PyObject_New(&DapChainWallet_dapChainWalletType);
    ((PyDapChainWalletObject*)obj_wallet)->wallet = dap_chain_wallet_create_with_seed(
                wallet_name,
                path_wallets,
                *((PyDapSignTypeObject*)obj_sig_type)->sign_type,
                seed,
                seed_size);
    return Py_BuildValue("(O)", obj_wallet);
}
PyObject *dap_chain_wallet_create_py(PyTypeObject *type, PyObject *argv, PyObject *kwds){
    (void)kwds;
    PyDapChainWalletObject *self;
    const char *wallet_name;
    const char *path_wallets;
    PyObject *obj_sign_type;
    if (!PyArg_ParseTuple(argv, "ssO", &wallet_name, &path_wallets, &obj_sign_type))
        return NULL;
    self = (PyDapChainWalletObject*)type->tp_alloc(type, 0);
    if (self != NULL){
        self->wallet = dap_chain_wallet_create(wallet_name, path_wallets, *((PyDapSignTypeObject*)obj_sign_type)->sign_type);
        if (self->wallet == NULL){
            Py_XDECREF(self);
            return NULL;
        }
    }
    return (PyObject*)self;
}
PyObject *dap_chain_wallet_open_file_py(PyObject *self, PyObject *argv){
    (void)self;
    const char *file_path;
    if (!PyArg_ParseTuple(argv, "s", &file_path))
        return NULL;
    PyObject *obj_wallet = _PyObject_New(&DapChainWallet_dapChainWalletType);
    ((PyDapChainWalletObject*)obj_wallet)->wallet = dap_chain_wallet_open_file(file_path);
    return Py_BuildValue("(O)", obj_wallet);
}
PyObject *dap_chain_wallet_open_py(PyObject *self, PyObject *argv){
    (void)self;
    const char *wallet_name;
    const char *wallet_path;
    if (!PyArg_ParseTuple(argv, "ss", &wallet_name, &wallet_path))
        return NULL;
    PyObject *obj_wallet = _PyObject_New(&DapChainWallet_dapChainWalletType);
    ((PyDapChainWalletObject*)obj_wallet)->wallet = dap_chain_wallet_open(wallet_name, wallet_path);
    return Py_BuildValue("(O)", obj_wallet);
}
PyObject *dap_chain_wallet_save_py(PyObject *self, PyObject *argv){
    (void)argv;
    int result = dap_chain_wallet_save(((PyDapChainWalletObject*)self)->wallet);
    return PyLong_FromLong(result);
}

void dap_chain_wallet_close_py(PyDapChainWalletObject *self){
    dap_chain_wallet_close(self->wallet);
    Py_TYPE(self)->tp_free((PyObject*)self);
}

PyObject *dap_cert_to_addr_py(PyObject *self, PyObject *argv){
    (void)self;
    PyObject *obj_cert;
    PyObject *obj_net_id;
    if (!PyArg_ParseTuple(argv, "OO", &obj_cert, &obj_net_id))
        return NULL;
    PyObject *obj_addr = _PyObject_New(&DapChainAddrObject_DapChainAddrObjectType);
    ((PyDapChainAddrObject*)obj_addr)->addr = dap_cert_to_addr(
                ((PyCryptoCertObject*)obj_cert)->cert,
                ((PyDapChainNetIdObject*)obj_net_id)->net_id
                );
    return Py_BuildValue("(O)", obj_addr);
}

PyObject *dap_chain_wallet_get_addr_py(PyObject *self, PyObject *argv){
    PyObject *obj_net_id;
    if (!PyArg_ParseTuple(argv, "O", &obj_net_id))
        return NULL;
    PyObject *obj_addr = _PyObject_New(&DapChainAddrObject_DapChainAddrObjectType);
    ((PyDapChainAddrObject*)obj_addr)->addr = dap_chain_wallet_get_addr(
                ((PyDapChainWalletObject*)self)->wallet,
                ((PyDapChainNetIdObject*)obj_net_id)->net_id
                );
    return Py_BuildValue("(O)", obj_addr);
}
PyObject *dap_chain_wallet_get_certs_number_py(PyObject *self, PyObject *argv){
    (void)argv;
    size_t result = dap_chain_wallet_get_certs_number(((PyDapChainWalletObject*)self)->wallet);
    return PyLong_FromLong(result);
}
PyObject *dap_chain_wallet_get_pkey_py(PyObject *self, PyObject *argv){
    uint32_t key_idx;
    if (!PyArg_ParseTuple(argv, "I", &key_idx))
            return NULL;
    PyObject *obj_pkey = _PyObject_New(&DapPkeyObject_DapPkeyObjectType);
    ((PyDapPkeyObject*)obj_pkey)->pkey = dap_chain_wallet_get_pkey(((PyDapChainWalletObject*)self)->wallet,
                                                                   key_idx);
    return Py_BuildValue("(O)", obj_pkey);
}
PyObject *dap_chain_wallet_get_key_py(PyObject *self, PyObject *argv){
    uint32_t key_idx;
    if (!PyArg_ParseTuple(argv, "I", &key_idx))
            return NULL;
    PyObject *obj_key = _PyObject_New(&PyCryptoKeyObject_PyCryptoKeyType);
    ((PyCryptoKeyObject*)obj_key)->key = dap_chain_wallet_get_key(
                ((PyDapChainWalletObject*)self)->wallet,
                key_idx
                );
    return Py_BuildValue("(O)", obj_key);
}
