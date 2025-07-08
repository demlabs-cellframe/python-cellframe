#include "dap_chain_datum_tx_python.h"

#define LOG_TAG "dap_chain_datum_tx_python"

// Initialize datum tx Python module
int dap_chain_datum_tx_init_py(void) {
    return 0;
}

// Deinitialize datum tx Python module
void dap_chain_datum_tx_deinit_py(void) {
    // Nothing to cleanup for now
}

// Python wrapper for dap_chain_datum_tx_delete - similar to Java JNI close
PyObject *dap_chain_datum_tx_close_py(PyObject *self, PyObject *args) {
    PyObject *obj_datum_tx;
    
    if (!PyArg_ParseTuple(args, "O", &obj_datum_tx)) {
        PyErr_SetString(PyExc_TypeError, "Invalid arguments. Expected DatumTx object.");
        return NULL;
    }
    
    if (!PyObject_TypeCheck(obj_datum_tx, &DapChainDatumTxObjectType)) {
        PyErr_SetString(PyExc_TypeError, "Argument must be a DatumTx object.");
        return NULL;
    }
    
    dap_chain_datum_tx_t *datum_tx = ((PyDapChainDatumTxObject*)obj_datum_tx)->datum_tx;
    if (datum_tx) {
        dap_chain_datum_tx_delete(datum_tx);
        ((PyDapChainDatumTxObject*)obj_datum_tx)->datum_tx = NULL;
    }
    
    Py_RETURN_NONE;
}

// Python wrapper for dap_chain_net_tx_create_by_json - similar to Java JNI fromJSON
PyObject *dap_chain_datum_tx_from_json_py(PyObject *self, PyObject *args) {
    (void)self;
    
    const char *json_str;
    
    if (!PyArg_ParseTuple(args, "s", &json_str)) {
        PyErr_SetString(PyExc_TypeError, "Invalid arguments. Expected JSON string.");
        return NULL;
    }
    
    if (!json_str) {
        PyErr_SetString(PyExc_ValueError, "Can't get JSON string");
        return NULL;
    }
    
    struct json_object *json_obj = json_tokener_parse(json_str);
    if (!json_obj) {
        PyErr_SetString(PyExc_ValueError, "Can't parse JSON");
        return NULL;
    }
    
    dap_chain_datum_tx_t *datum_tx = NULL;
    json_object *jobj_errors = json_object_new_array();
    
    if (dap_chain_net_tx_create_by_json(json_obj, NULL, jobj_errors, &datum_tx, NULL, NULL) == 0) {
        // Success - create Python object
        PyDapChainDatumTxObject *obj_datum_tx = PyObject_New(PyDapChainDatumTxObject, &DapChainDatumTxObjectType);
        if (!obj_datum_tx) {
            dap_chain_datum_tx_delete(datum_tx);
            json_object_put(jobj_errors);
            json_object_put(json_obj);
            PyErr_SetString(PyExc_MemoryError, "Failed to create DatumTx object.");
            return NULL;
        }
        
        obj_datum_tx->datum_tx = datum_tx;
        json_object_put(jobj_errors);
        json_object_put(json_obj);
        return (PyObject*)obj_datum_tx;
    } else {
        // Error - return error message
        const char *error_buf = json_object_to_json_string_ext(jobj_errors, JSON_C_TO_STRING_PRETTY);
        PyErr_SetString(PyExc_RuntimeError, error_buf);
        json_object_put(jobj_errors);
        json_object_put(json_obj);
        return NULL;
    }
}

// Python wrapper for dap_chain_net_tx_to_json - similar to Java JNI toJSON
PyObject *dap_chain_datum_tx_to_json_py(PyObject *self, PyObject *args) {
    (void)self;
    
    PyObject *obj_datum_tx;
    
    if (!PyArg_ParseTuple(args, "O", &obj_datum_tx)) {
        PyErr_SetString(PyExc_TypeError, "Invalid arguments. Expected DatumTx object.");
        return NULL;
    }
    
    if (!PyObject_TypeCheck(obj_datum_tx, &DapChainDatumTxObjectType)) {
        PyErr_SetString(PyExc_TypeError, "Argument must be a DatumTx object.");
        return NULL;
    }
    
    dap_chain_datum_tx_t *datum_tx = ((PyDapChainDatumTxObject*)obj_datum_tx)->datum_tx;
    if (!datum_tx) {
        PyErr_SetString(PyExc_ValueError, "Invalid datum tx pointer.");
        return NULL;
    }
    
    json_object *out_json = json_object_new_object();
    
    int res = dap_chain_net_tx_to_json(datum_tx, out_json);
    if (res == 0) {
        const char *out_buf = json_object_to_json_string_ext(out_json, JSON_C_TO_STRING_PRETTY);
        PyObject *result = PyUnicode_FromString(out_buf);
        json_object_put(out_json);
        return result;
    } else {
        json_object_put(out_json);
        PyErr_SetString(PyExc_RuntimeError, "Error converting datum to JSON");
        return NULL;
    }
}

// Method definitions for datum tx functions
static PyMethodDef DapChainDatumTxMethods[] = {
    {"close", (PyCFunction)dap_chain_datum_tx_close_py, METH_VARARGS | METH_STATIC, 
     "Close and delete datum transaction"},
    {"fromJSON", (PyCFunction)dap_chain_datum_tx_from_json_py, METH_VARARGS | METH_STATIC, 
     "Create datum transaction from JSON string"},
    {"toJSON", (PyCFunction)dap_chain_datum_tx_to_json_py, METH_VARARGS | METH_STATIC, 
     "Convert datum transaction to JSON string"},
    {NULL, NULL, 0, NULL}
};

// Type object for datum tx functions
PyTypeObject DapChainDatumTxObjectType = {
    .ob_base = PyVarObject_HEAD_INIT(NULL, 0)
    .tp_name = "CellFrame.ChainDatumTx",
    .tp_basicsize = sizeof(PyDapChainDatumTxObject),
    .tp_flags = Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE,
    .tp_doc = "Chain datum transaction object",
    .tp_methods = DapChainDatumTxMethods,
    .tp_new = PyType_GenericNew,
}; 