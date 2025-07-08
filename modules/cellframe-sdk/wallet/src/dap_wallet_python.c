#include "dap_wallet_python.h"
#include "dap_chain_addr_python.h"
#include "dap_enc_key_python.h"

#define LOG_TAG "dap_wallet_python"

// Initialize wallet Python module
int dap_wallet_init_py(void) {
    return 0;
}

// Deinitialize wallet Python module
void dap_wallet_deinit_py(void) {
    // Nothing to cleanup for now
}

// Python wrapper for dap_chain_wallet_open_file - similar to Java JNI openFile
PyObject *dap_wallet_open_file_py(PyObject *self, PyObject *args) {
    (void)self;
    
    const char *file_name;
    const char *password = NULL;
    
    if (!PyArg_ParseTuple(args, "s|s", &file_name, &password)) {
        PyErr_SetString(PyExc_TypeError, "Invalid arguments. Expected filename and optional password.");
        return NULL;
    }
    
    // Check if file exists
    FILE *f = fopen(file_name, "r");
    if (f == NULL) {
        PyErr_SetString(PyExc_FileNotFoundError, "Wallet file not found.");
        return NULL;
    }
    fclose(f);
    
    unsigned int status = 0;
    dap_chain_wallet_t *wallet = dap_chain_wallet_open_file(file_name, password, &status);
    
    if (!wallet) {
        PyErr_Format(PyExc_RuntimeError, "Failed to open wallet file. Status: %u", status);
        return NULL;
    }
    
    PyDapWalletObject *obj_wallet = PyObject_New(PyDapWalletObject, &DapWalletObjectType);
    if (!obj_wallet) {
        dap_chain_wallet_close(wallet);
        PyErr_SetString(PyExc_MemoryError, "Failed to create Wallet object.");
        return NULL;
    }
    
    obj_wallet->wallet = wallet;
    return (PyObject*)obj_wallet;
}

// Python wrapper for dap_chain_wallet_close - similar to Java JNI close
PyObject *dap_wallet_close_py(PyObject *self, PyObject *args) {
    PyObject *obj_wallet;
    
    if (!PyArg_ParseTuple(args, "O", &obj_wallet)) {
        PyErr_SetString(PyExc_TypeError, "Invalid arguments. Expected Wallet object.");
        return NULL;
    }
    
    if (!PyObject_TypeCheck(obj_wallet, &DapWalletObjectType)) {
        PyErr_SetString(PyExc_TypeError, "Argument must be a Wallet object.");
        return NULL;
    }
    
    dap_chain_wallet_t *wallet = ((PyDapWalletObject*)obj_wallet)->wallet;
    if (wallet) {
        dap_chain_wallet_close(wallet);
        ((PyDapWalletObject*)obj_wallet)->wallet = NULL;
    }
    
    Py_RETURN_NONE;
}

// Python wrapper for dap_chain_wallet_get_certs_number - similar to Java JNI getCertsNumber
PyObject *dap_wallet_get_certs_number_py(PyObject *self, PyObject *args) {
    PyObject *obj_wallet;
    
    if (!PyArg_ParseTuple(args, "O", &obj_wallet)) {
        PyErr_SetString(PyExc_TypeError, "Invalid arguments. Expected Wallet object.");
        return NULL;
    }
    
    if (!PyObject_TypeCheck(obj_wallet, &DapWalletObjectType)) {
        PyErr_SetString(PyExc_TypeError, "Argument must be a Wallet object.");
        return NULL;
    }
    
    dap_chain_wallet_t *wallet = ((PyDapWalletObject*)obj_wallet)->wallet;
    if (!wallet) {
        PyErr_SetString(PyExc_ValueError, "Invalid wallet pointer.");
        return NULL;
    }
    
    size_t num = dap_chain_wallet_get_certs_number(wallet);
    return PyLong_FromSize_t(num);
}

// Python wrapper for dap_chain_wallet_get_addr - similar to Java JNI getAddress
PyObject *dap_wallet_get_address_py(PyObject *self, PyObject *args) {
    PyObject *obj_wallet;
    unsigned long long net_id;
    
    if (!PyArg_ParseTuple(args, "OK", &obj_wallet, &net_id)) {
        PyErr_SetString(PyExc_TypeError, "Invalid arguments. Expected Wallet object and network ID.");
        return NULL;
    }
    
    if (!PyObject_TypeCheck(obj_wallet, &DapWalletObjectType)) {
        PyErr_SetString(PyExc_TypeError, "First argument must be a Wallet object.");
        return NULL;
    }
    
    dap_chain_wallet_t *wallet = ((PyDapWalletObject*)obj_wallet)->wallet;
    if (!wallet) {
        PyErr_SetString(PyExc_ValueError, "Invalid wallet pointer.");
        return NULL;
    }
    
    dap_chain_net_id_t chain_net_id = {.uint64 = net_id};
    dap_chain_addr_t *addr = dap_chain_wallet_get_addr(wallet, chain_net_id);
    
    if (!addr) {
        PyErr_SetString(PyExc_RuntimeError, "Failed to get address from wallet.");
        return NULL;
    }
    
    PyDapChainAddrObject *obj_addr = PyObject_New(PyDapChainAddrObject, &DapChainAddrObjectType);
    if (!obj_addr) {
        PyErr_SetString(PyExc_MemoryError, "Failed to create ChainAddr object.");
        return NULL;
    }
    
    obj_addr->addr = addr;
    return (PyObject*)obj_addr;
}

// Python wrapper for dap_chain_wallet_create - similar to Java JNI createNative
PyObject *dap_wallet_create_py(PyObject *self, PyObject *args) {
    (void)self;
    
    const char *path = "./";
    const char *name = "wallet";
    const char *sign_type_str = "sig_dil";
    const char *seed = NULL;
    const char *password = NULL;
    int use_sha2 = 0;
    int use_version0 = 0;
    
    if (!PyArg_ParseTuple(args, "|sssssii", &path, &name, &sign_type_str, &seed, &password, &use_sha2, &use_version0)) {
        PyErr_SetString(PyExc_TypeError, "Invalid arguments.");
        return NULL;
    }
    
    dap_chain_wallet_t *wallet = NULL;
    dap_sign_type_t sig_type = dap_sign_type_from_str(sign_type_str);
    
    if (!seed) {
        wallet = dap_chain_wallet_create(name, path, sig_type, password);
    } else {
        uint8_t seed_hash[DAP_HASH_FAST_SIZE];
        if (!get_seed_hash(seed, password ? password : "", use_sha2, seed_hash)) {
            PyErr_SetString(PyExc_RuntimeError, "Failed to generate seed hash.");
            return NULL;
        }
        wallet = dap_chain_wallet_create_with_seed(name, path, sig_type, seed_hash, DAP_HASH_FAST_SIZE, password);
    }
    
    if (!wallet) {
        PyErr_SetString(PyExc_RuntimeError, "Failed to create wallet.");
        return NULL;
    }
    
    PyDapWalletObject *obj_wallet = PyObject_New(PyDapWalletObject, &DapWalletObjectType);
    if (!obj_wallet) {
        dap_chain_wallet_close(wallet);
        PyErr_SetString(PyExc_MemoryError, "Failed to create Wallet object.");
        return NULL;
    }
    
    obj_wallet->wallet = wallet;
    return (PyObject*)obj_wallet;
}

// Python wrapper for dap_chain_wallet_get_key - similar to Java JNI getEncKey
PyObject *dap_wallet_get_enc_key_py(PyObject *self, PyObject *args) {
    PyObject *obj_wallet;
    
    if (!PyArg_ParseTuple(args, "O", &obj_wallet)) {
        PyErr_SetString(PyExc_TypeError, "Invalid arguments. Expected Wallet object.");
        return NULL;
    }
    
    if (!PyObject_TypeCheck(obj_wallet, &DapWalletObjectType)) {
        PyErr_SetString(PyExc_TypeError, "Argument must be a Wallet object.");
        return NULL;
    }
    
    dap_chain_wallet_t *wallet = ((PyDapWalletObject*)obj_wallet)->wallet;
    if (!wallet) {
        PyErr_SetString(PyExc_ValueError, "Invalid wallet pointer.");
        return NULL;
    }
    
    dap_enc_key_t *enc_key = dap_chain_wallet_get_key(wallet, 0);
    if (!enc_key) {
        PyErr_SetString(PyExc_RuntimeError, "Failed to get encryption key from wallet.");
        return NULL;
    }
    
    PyDapEncKeyObject *obj_enc_key = PyObject_New(PyDapEncKeyObject, &DapEncKeyObjectType);
    if (!obj_enc_key) {
        PyErr_SetString(PyExc_MemoryError, "Failed to create EncKey object.");
        return NULL;
    }
    
    obj_enc_key->enc_key = enc_key;
    return (PyObject*)obj_enc_key;
}

// Method definitions for wallet functions
static PyMethodDef DapWalletMethods[] = {
    {"openFile", (PyCFunction)dap_wallet_open_file_py, METH_VARARGS | METH_STATIC, 
     "Open wallet from file"},
    {"close", (PyCFunction)dap_wallet_close_py, METH_VARARGS | METH_STATIC, 
     "Close wallet"},
    {"getCertsNumber", (PyCFunction)dap_wallet_get_certs_number_py, METH_VARARGS | METH_STATIC, 
     "Get number of certificates in wallet"},
    {"getAddress", (PyCFunction)dap_wallet_get_address_py, METH_VARARGS | METH_STATIC, 
     "Get address from wallet for network ID"},
    {"create", (PyCFunction)dap_wallet_create_py, METH_VARARGS | METH_STATIC, 
     "Create new wallet"},
    {"getEncKey", (PyCFunction)dap_wallet_get_enc_key_py, METH_VARARGS | METH_STATIC, 
     "Get encryption key from wallet"},
    {NULL, NULL, 0, NULL}
};

// Type object for wallet functions
PyTypeObject DapWalletObjectType = {
    .ob_base = PyVarObject_HEAD_INIT(NULL, 0)
    .tp_name = "CellFrame.Wallet",
    .tp_basicsize = sizeof(PyDapWalletObject),
    .tp_flags = Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE,
    .tp_doc = "Wallet object",
    .tp_methods = DapWalletMethods,
    .tp_new = PyType_GenericNew,
}; 