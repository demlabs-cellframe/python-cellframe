#include "dap_enc_key_python.h"

#define LOG_TAG "dap_enc_key_python"

// Initialize enc key Python module
int dap_enc_key_init_py(void) {
    return 0;
}

// Deinitialize enc key Python module
void dap_enc_key_deinit_py(void) {
    // Nothing to cleanup for now
}

// Python wrapper for dap_enc_key_delete - similar to Java JNI close
PyObject *dap_enc_key_close_py(PyObject *self, PyObject *args) {
    PyObject *obj_enc_key;
    
    if (!PyArg_ParseTuple(args, "O", &obj_enc_key)) {
        PyErr_SetString(PyExc_TypeError, "Invalid arguments. Expected EncKey object.");
        return NULL;
    }
    
    if (!PyObject_TypeCheck(obj_enc_key, &DapEncKeyObjectType)) {
        PyErr_SetString(PyExc_TypeError, "Argument must be an EncKey object.");
        return NULL;
    }
    
    dap_enc_key_t *enc_key = ((PyDapEncKeyObject*)obj_enc_key)->enc_key;
    if (enc_key) {
        dap_enc_key_delete(enc_key);
        ((PyDapEncKeyObject*)obj_enc_key)->enc_key = NULL;
    }
    
    Py_RETURN_NONE;
}

// Python wrapper for getting key type - similar to Java JNI nativeGetType
PyObject *dap_enc_key_get_type_py(PyObject *self, PyObject *args) {
    PyObject *obj_enc_key;
    
    if (!PyArg_ParseTuple(args, "O", &obj_enc_key)) {
        PyErr_SetString(PyExc_TypeError, "Invalid arguments. Expected EncKey object.");
        return NULL;
    }
    
    if (!PyObject_TypeCheck(obj_enc_key, &DapEncKeyObjectType)) {
        PyErr_SetString(PyExc_TypeError, "Argument must be an EncKey object.");
        return NULL;
    }
    
    dap_enc_key_t *enc_key = ((PyDapEncKeyObject*)obj_enc_key)->enc_key;
    if (!enc_key) {
        PyErr_SetString(PyExc_ValueError, "Invalid encryption key pointer.");
        return NULL;
    }
    
    return PyLong_FromLong(enc_key->type);
}

// Python wrapper for dap_enc_key_new_generate - similar to Java JNI createFromSeed
PyObject *dap_enc_key_create_from_seed_py(PyObject *self, PyObject *args) {
    (void)self;
    
    int sign_type;
    const char *seed_str;
    
    if (!PyArg_ParseTuple(args, "is", &sign_type, &seed_str)) {
        PyErr_SetString(PyExc_TypeError, "Invalid arguments. Expected int and string.");
        return NULL;
    }
    
    if (!seed_str) {
        PyErr_SetString(PyExc_ValueError, "Seed string cannot be NULL.");
        return NULL;
    }
    
    uint8_t seed_hash[DAP_HASH_FAST_SIZE];
    if (!get_seed_hash(seed_str, NULL, false, seed_hash)) {
        PyErr_SetString(PyExc_RuntimeError, "Failed to generate seed hash.");
        return NULL;
    }
    
    dap_enc_key_t *enc_key = dap_enc_key_new_generate(sign_type, NULL, 0, seed_hash, DAP_HASH_FAST_SIZE, 0);
    if (!enc_key) {
        PyErr_SetString(PyExc_RuntimeError, "Failed to create encryption key from seed.");
        return NULL;
    }
    
    PyDapEncKeyObject *obj_enc_key = PyObject_New(PyDapEncKeyObject, &DapEncKeyObjectType);
    if (!obj_enc_key) {
        dap_enc_key_delete(enc_key);
        PyErr_SetString(PyExc_MemoryError, "Failed to create EncKey object.");
        return NULL;
    }
    
    obj_enc_key->enc_key = enc_key;
    return (PyObject*)obj_enc_key;
}

// Method definitions for enc key functions
static PyMethodDef DapEncKeyMethods[] = {
    {"close", (PyCFunction)dap_enc_key_close_py, METH_VARARGS | METH_STATIC, 
     "Close and delete encryption key"},
    {"getType", (PyCFunction)dap_enc_key_get_type_py, METH_VARARGS | METH_STATIC, 
     "Get encryption key type"},
    {"createFromSeed", (PyCFunction)dap_enc_key_create_from_seed_py, METH_VARARGS | METH_STATIC, 
     "Create encryption key from seed"},
    {NULL, NULL, 0, NULL}
};

// Type object for enc key functions
PyTypeObject DapEncKeyObjectType = {
    .ob_base = PyVarObject_HEAD_INIT(NULL, 0)
    .tp_name = "CellFrame.EncKey",
    .tp_basicsize = sizeof(PyDapEncKeyObject),
    .tp_flags = Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE,
    .tp_doc = "Encryption key object",
    .tp_methods = DapEncKeyMethods,
    .tp_new = PyType_GenericNew,
}; 