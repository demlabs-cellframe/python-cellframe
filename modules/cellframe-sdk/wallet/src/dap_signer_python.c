#include "dap_signer_python.h"

#define LOG_TAG "dap_signer_python"

// Initialize signer Python module
int dap_signer_init_py(void) {
    return 0;
}

// Deinitialize signer Python module
void dap_signer_deinit_py(void) {
    // Nothing to cleanup for now
}

// Python wrapper for dap_chain_datum_tx_add_sign_item - similar to Java JNI signDatumTx
PyObject *dap_signer_sign_datum_tx_py(PyObject *self, PyObject *args) {
    (void)self;
    
    PyObject *obj_datum_tx;
    PyObject *obj_enc_key;
    
    if (!PyArg_ParseTuple(args, "OO", &obj_datum_tx, &obj_enc_key)) {
        PyErr_SetString(PyExc_TypeError, "Invalid arguments. Expected DatumTx and EncKey objects.");
        return NULL;
    }
    
    if (!PyObject_TypeCheck(obj_datum_tx, &DapChainDatumTxObjectType)) {
        PyErr_SetString(PyExc_TypeError, "First argument must be a DatumTx object.");
        return NULL;
    }
    
    if (!PyObject_TypeCheck(obj_enc_key, &DapEncKeyObjectType)) {
        PyErr_SetString(PyExc_TypeError, "Second argument must be an EncKey object.");
        return NULL;
    }
    
    dap_chain_datum_tx_t *datum_tx = ((PyDapChainDatumTxObject*)obj_datum_tx)->datum_tx;
    dap_enc_key_t *enc_key = ((PyDapEncKeyObject*)obj_enc_key)->enc_key;
    
    if (!datum_tx || !enc_key) {
        PyErr_SetString(PyExc_ValueError, "Invalid datum transaction or encryption key pointer.");
        return NULL;
    }
    
    // Create a copy of the datum transaction
    dap_chain_datum_tx_t *signed_tx = DAP_DUP_SIZE(datum_tx, dap_chain_datum_tx_get_size(datum_tx));
    if (!signed_tx) {
        PyErr_SetString(PyExc_MemoryError, "Failed to duplicate datum transaction.");
        return NULL;
    }
    
    // Add signature to the transaction
    int result = dap_chain_datum_tx_add_sign_item(&signed_tx, enc_key);
    
    if (result != 1) {
        DAP_DELETE(signed_tx);
        PyErr_SetString(PyExc_RuntimeError, "Failed to sign datum transaction.");
        return NULL;
    }
    
    // Create new Python object for the signed transaction
    PyDapChainDatumTxObject *obj_signed_tx = PyObject_New(PyDapChainDatumTxObject, &DapChainDatumTxObjectType);
    if (!obj_signed_tx) {
        DAP_DELETE(signed_tx);
        PyErr_SetString(PyExc_MemoryError, "Failed to create signed DatumTx object.");
        return NULL;
    }
    
    obj_signed_tx->datum_tx = signed_tx;
    return (PyObject*)obj_signed_tx;
}

// Method definitions for signer functions
static PyMethodDef DapSignerMethods[] = {
    {"signDatumTx", (PyCFunction)dap_signer_sign_datum_tx_py, METH_VARARGS | METH_STATIC, 
     "Sign datum transaction with encryption key"},
    {NULL, NULL, 0, NULL}
};

// Type object for signer functions
PyTypeObject DapSignerObjectType = {
    .ob_base = PyVarObject_HEAD_INIT(NULL, 0)
    .tp_name = "CellFrame.Signer",
    .tp_basicsize = sizeof(PyObject),
    .tp_flags = Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE,
    .tp_doc = "Signer object for signing transactions",
    .tp_methods = DapSignerMethods,
    .tp_new = PyType_GenericNew,
}; 