#include "dap_chain_wallet_python.h"
#define LOG_TAG "dap_chain_wallet_python"

int dap_chain_wallet_init_py(void){
    return dap_chain_wallet_init();
}
void dap_chain_wallet_deinit_py(void){
    dap_chain_wallet_deinit();
}

PyMethodDef ChainWalletMethods[] = {
        {"getPath", (PyCFunction)dap_chain_wallet_get_path_py, METH_VARARGS | METH_STATIC, ""},
        {"createWithSeed", (PyCFunction)dap_chain_wallet_create_with_seed_py, METH_VARARGS | METH_STATIC, ""},
        {"openFile", (PyCFunction)dap_chain_wallet_open_file_py, METH_VARARGS | METH_STATIC, ""},
        {"open", (PyCFunction)dap_chain_wallet_open_py, METH_VARARGS | METH_STATIC, ""},
        {"save", (PyCFunction)dap_chain_wallet_save_py, METH_NOARGS, ""},
        {"certToAddr", (PyCFunction)dap_cert_to_addr_py, METH_VARARGS | METH_STATIC, ""},
        {"getAddr", (PyCFunction)dap_chain_wallet_get_addr_py, METH_VARARGS, ""},
        {"getCertsNumber", (PyCFunction)dap_chain_wallet_get_certs_number_py, METH_NOARGS, ""},
        {"getPKey", (PyCFunction)dap_chain_wallet_get_pkey_py, METH_VARARGS, ""},
        {"getKey", (PyCFunction)dap_chain_wallet_get_key_py, METH_VARARGS, ""},
        {NULL, NULL, 0, NULL}
};

PyTypeObject DapChainWalletObjectType = {
        PyVarObject_HEAD_INIT(NULL, 0)
        "CellFrame.ChainWallet",             /* tp_name */
        sizeof(PyDapChainWalletObject),         /* tp_basicsize */
        0,                         /* tp_itemsize */
        (destructor)dap_chain_wallet_close_py, /* tp_dealloc */
        0,                         /* tp_print */
        0,                         /* tp_getattr */
        0,                         /* tp_setattr */
        0,                         /* tp_reserved */
        0,                         /* tp_repr */
        0,                         /* tp_as_number */
        0,                         /* tp_as_sequence */
        0,                         /* tp_as_mapping */
        0,                         /* tp_hash  */
        0,                         /* tp_call */
        0,                         /* tp_str */
        0,                         /* tp_getattro */
        0,                         /* tp_setattro */
        0,                         /* tp_as_buffer */
        Py_TPFLAGS_DEFAULT |
        Py_TPFLAGS_BASETYPE,   /* tp_flags */
        "Chain wallet object",           /* tp_doc */
        0,		               /* tp_traverse */
        0,		               /* tp_clear */
        0,		               /* tp_richcompare */
        0,		               /* tp_weaklistoffset */
        0,		               /* tp_iter */
        0,		               /* tp_iternext */
        ChainWalletMethods,             /* tp_methods */
        0,                         /* tp_members */
        0,                         /* tp_getset */
        0,                         /* tp_base */
        0,                         /* tp_dict */
        0,                         /* tp_descr_get */
        0,                         /* tp_descr_set */
        0,                         /* tp_dictoffset */
        0,                         /* tp_init */
        0,                         /* tp_alloc */
        dap_chain_wallet_create_py,/* tp_new */
        0,                         /* tp_free */
        0,                         /* tp_is_gc*/
        0,                          /* tp_bases*/
        0,                           /* tp_mro */
        0,                           /* tp_cache */
        0,                           /* tp_subclasses */
        0,                           /* tp_weaklist */
        0,                           /* tp_del */
        0,                          /* tp_version_tag*/
        0,                         /* tp_finalize*/
};

PyObject *dap_chain_wallet_get_path_py(PyObject *self, PyObject *argv){
    (void)self;
    (void)argv;
    const char *path = dap_chain_wallet_get_path(g_config);
    return Py_BuildValue("s", path);
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
        PyErr_SetString(PyExc_TypeError, "The fourth argument must be bytes");
        return NULL;
    }
    void *seed = (void *)PyBytes_AsString(obj_seed);
    size_t seed_size = PyBytes_Size(obj_seed);
    PyObject *obj_wallet = _PyObject_New(&DapChainWalletObjectType);
    PyObject_Dir(obj_wallet);
    ((PyDapChainWalletObject*)obj_wallet)->wallet = dap_chain_wallet_create_with_seed(
                wallet_name,
                path_wallets,
                *((PyDapSignTypeObject*)obj_sig_type)->sign_type,
                seed,
                seed_size);
    return Py_BuildValue("O", obj_wallet);
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
    PyObject *obj_wallet = _PyObject_New(&DapChainWalletObjectType);
    PyObject_Dir(obj_wallet);
    ((PyDapChainWalletObject*)obj_wallet)->wallet = dap_chain_wallet_open_file(file_path);
    return Py_BuildValue("O", obj_wallet);
}
PyObject *dap_chain_wallet_open_py(PyObject *self, PyObject *argv){
    (void)self;
    const char *wallet_name;
    const char *wallet_path;
    if (!PyArg_ParseTuple(argv, "ss", &wallet_name, &wallet_path))
        return NULL;
    PyObject *obj_wallet = _PyObject_New(&DapChainWalletObjectType);
    PyObject_Dir(obj_wallet);
    ((PyDapChainWalletObject*)obj_wallet)->wallet = dap_chain_wallet_open(wallet_name, wallet_path);
    return Py_BuildValue("O", obj_wallet);
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
    PyObject *obj_addr = _PyObject_New(&DapChainAddrObjectType);
    PyObject_Dir(obj_addr);
    ((PyDapChainAddrObject*)obj_addr)->addr = dap_cert_to_addr(
                ((PyCryptoCertObject*)obj_cert)->cert,
                ((PyDapChainNetIdObject*)obj_net_id)->net_id
                );
    return Py_BuildValue("O", obj_addr);
}

PyObject *dap_chain_wallet_get_addr_py(PyObject *self, PyObject *argv){
    PyObject *obj_net_id;
    if (!PyArg_ParseTuple(argv, "O", &obj_net_id))
        return NULL;
    PyObject *obj_addr = _PyObject_New(&DapChainAddrObjectType);
    PyObject_Dir(obj_addr);
    ((PyDapChainAddrObject*)obj_addr)->addr = dap_chain_wallet_get_addr(
                ((PyDapChainWalletObject*)self)->wallet,
                ((PyDapChainNetIdObject*)obj_net_id)->net_id
                );
    return Py_BuildValue("O", obj_addr);
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
    PyObject_Dir(obj_pkey);
    ((PyDapPkeyObject*)obj_pkey)->pkey = dap_chain_wallet_get_pkey(((PyDapChainWalletObject*)self)->wallet,
                                                                   key_idx);
    return Py_BuildValue("O", obj_pkey);
}
PyObject *dap_chain_wallet_get_key_py(PyObject *self, PyObject *argv){
    uint32_t key_idx;
    if (!PyArg_ParseTuple(argv, "I", &key_idx))
            return NULL;
    PyObject *obj_key = _PyObject_New(&PyCryptoKeyObjectType);
    PyObject_Dir(obj_key);
    ((PyCryptoKeyObject*)obj_key)->key = dap_chain_wallet_get_key(
                ((PyDapChainWalletObject*)self)->wallet,
                key_idx
                );
    return Py_BuildValue("O", obj_key);
}
