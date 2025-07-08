#include "dap_chain_addr_python.h"

#define LOG_TAG "dap_chain_addr_python"

// Initialize wallet address Python module
int dap_chain_addr_wallet_init_py(void) {
    return 0;
}

// Deinitialize wallet address Python module
void dap_chain_addr_wallet_deinit_py(void) {
    // Nothing to cleanup for now
}

// Python wrapper for dap_chain_addr_to_str - similar to Java JNI toStringNative
PyObject *dap_chain_addr_to_str_wallet_py(PyObject *self, PyObject *args) {
    PyObject *obj_addr;
    
    if (!PyArg_ParseTuple(args, "O", &obj_addr)) {
        PyErr_SetString(PyExc_TypeError, "Invalid arguments. Expected ChainAddr object.");
        return NULL;
    }
    
    if (!PyDapChainAddrObject_Check((PyDapChainAddrObject*)obj_addr)) {
        PyErr_SetString(PyExc_TypeError, "Argument must be a ChainAddr object.");
        return NULL;
    }
    
    dap_chain_addr_t *addr = ((PyDapChainAddrObject*)obj_addr)->addr;
    if (!addr) {
        PyErr_SetString(PyExc_ValueError, "Invalid address pointer.");
        return NULL;
    }
    
    const char *addr_str = dap_chain_addr_to_str(addr);
    if (!addr_str) {
        PyErr_SetString(PyExc_RuntimeError, "Failed to convert address to string.");
        return NULL;
    }
    
    PyObject *result = PyUnicode_FromString(addr_str);
    return result;
}

// Python wrapper for dap_chain_addr_fill_from_key - similar to Java JNI fromEncKeyNative
PyObject *dap_chain_addr_from_enc_key_wallet_py(PyObject *self, PyObject *args) {
    (void)self;
    
    PyObject *obj_enc_key;
    PyObject *obj_net_id;
    
    if (!PyArg_ParseTuple(args, "OO", &obj_enc_key, &obj_net_id)) {
        PyErr_SetString(PyExc_TypeError, "Invalid arguments. Expected CryptoKey and ChainNetID objects.");
        return NULL;
    }
    
    if (!PyCryptoKeyObject_check(obj_enc_key)) {
        PyErr_SetString(PyExc_TypeError, "First argument must be a CryptoKey object.");
        return NULL;
    }
    
    if (!PyObject_TypeCheck(obj_net_id, &DapChainNetIdObjectType)) {
        PyErr_SetString(PyExc_TypeError, "Second argument must be a ChainNetID object.");
        return NULL;
    }
    
    dap_enc_key_t *enc_key = ((PyCryptoKeyObject*)obj_enc_key)->key;
    if (!enc_key) {
        PyErr_SetString(PyExc_ValueError, "Invalid encryption key pointer.");
        return NULL;
    }
    
    dap_chain_net_id_t net_id = ((PyDapChainNetIdObject*)obj_net_id)->net_id;
    
    // Create new address object
    PyDapChainAddrObject *obj_addr = PyObject_New(PyDapChainAddrObject, &DapChainAddrObjectType);
    if (!obj_addr) {
        PyErr_SetString(PyExc_MemoryError, "Failed to create address object.");
        return NULL;
    }
    
    // Allocate memory for address
    obj_addr->addr = DAP_NEW(dap_chain_addr_t);
    if (!obj_addr->addr) {
        Py_DECREF(obj_addr);
        PyErr_SetString(PyExc_MemoryError, "Failed to allocate address memory.");
        return NULL;
    }
    
    // Fill address from encryption key
    int result = dap_chain_addr_fill_from_key(obj_addr->addr, enc_key, net_id);
    if (result != 0) {
        DAP_DELETE(obj_addr->addr);
        Py_DECREF(obj_addr);
        PyErr_SetString(PyExc_RuntimeError, "Failed to create address from encryption key.");
        return NULL;
    }
    
    return (PyObject*)obj_addr;
}

// Method definitions for wallet address functions
static PyMethodDef DapChainAddrWalletMethods[] = {
    {"toStr", (PyCFunction)dap_chain_addr_to_str_wallet_py, METH_VARARGS | METH_STATIC, 
     "Convert address to string representation"},
    {"fromEncKey", (PyCFunction)dap_chain_addr_from_enc_key_wallet_py, METH_VARARGS | METH_STATIC, 
     "Create address from encryption key and network ID"},
    {NULL, NULL, 0, NULL}
};

// Type object for wallet address functions
PyTypeObject DapChainAddrWalletObjectType = {
    .ob_base = PyVarObject_HEAD_INIT(NULL, 0)
    .tp_name = "CellFrame.ChainAddrWallet",
    .tp_basicsize = sizeof(PyObject),
    .tp_flags = Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE,
    .tp_doc = "Chain address wallet operations",
    .tp_methods = DapChainAddrWalletMethods,
    .tp_new = PyType_GenericNew,
}; 