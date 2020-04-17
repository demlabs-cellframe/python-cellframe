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

PyObject *dap_chain_wallet_get_certs_number_py(PyObject *self, PyObject *argv){
    (void)argv;
    size_t result = dap_chain_wallet_get_certs_number(((PyDapChainWalletObject*)self)->wallet);
    return PyLong_FromLong(result);
}
