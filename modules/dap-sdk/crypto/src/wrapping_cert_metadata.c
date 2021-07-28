#include "wrapping_cert_metadata.h"
#include "wrapping_cert.h"


///!!! META

PyObject *dap_cert_new_meta_py(PyObject *self, PyObject *args){
    //
    const char *l_key = NULL;
    PyObject *l_obj_type_metadata = NULL;
    PyObject *l_obj_value = NULL;
    if (!PyArg_ParseTuple(args, "sOO", &l_key, &l_obj_type_metadata, &l_obj_value)){
        PyErr_SetString(PyExc_SyntaxError, "Wrong arguments list in function call");
        return NULL;
    }
    if (!PyBytes_Check(l_obj_value)){
        PyErr_SetString(PyExc_BytesWarning, "The first agrument the is not a bytes object");
        return NULL;
    }
    size_t l_value_size = (size_t)PyBytes_Size(l_obj_value);
    void *l_value = PyBytes_AsString(l_obj_value);
    dap_cert_metadata_t *l_meta = dap_cert_new_meta(l_key, ((PyDapCertMetadataTypeObject*)l_obj_type_metadata)->type, l_value, l_value_size);
    if (l_meta == NULL){
        return Py_None;
    } else {
        PyObject *l_obj_metadata = _PyObject_New(&g_crypto_cert_metadata_type_py);
        ((PyDapCertMetadataObject*)l_obj_metadata)->metadata = l_meta;
        return Py_BuildValue("O", l_obj_metadata);
    }
}
PyObject *dap_cert_add_meta_bytes_py(PyObject *self, PyObject *args){
    (void)self;
    PyObject *l_obj_cert = NULL;
    PyObject *l_obj_type = NULL;
    PyObject *l_obj_value = NULL;
    const char *l_key = NULL;
    if (!PyArg_ParseTuple(args, "OsOO", &l_obj_cert, &l_key, &l_obj_type, &l_obj_value)){
        PyErr_SetString(PyExc_SyntaxError, "Wrong arguments list in function call");
        return NULL;
    }
    if (!PyBytes_Check(l_obj_value)){
        PyErr_SetString(PyExc_BytesWarning, "The first agrument the is not a bytes object");
        return NULL;
    }
    size_t l_value_size = (size_t)PyBytes_Size(l_obj_value);
    void *l_value = PyBytes_AsString(l_obj_value);
    dap_cert_add_meta(((PyCryptoCertObject*)l_obj_cert)->cert, l_key, ((PyDapCertMetadataTypeObject*)l_obj_type)->type, l_value, l_value_size);
    return Py_None;
}
PyObject *dap_cert_add_meta_scalar_py(PyObject *self, PyObject *args){
    PyObject *l_obj_cert = NULL;
    const char *l_key = NULL;
    PyObject *l_obj_type_metadata = NULL;
    PyObject *l_obj_value = NULL;
    if (!PyArg_ParseTuple(args, "OsOO", &l_obj_cert, &l_key, &l_obj_type_metadata, &l_obj_value)){
        PyErr_SetString(PyExc_SyntaxError, "Wrong arguments list in function call");
        return NULL;
    }
    if (PyFloat_Check(l_obj_value)){
        double l_data = PyFloat_AsDouble(l_obj_value);
        dap_cert_add_meta_scalar(((PyCryptoCertObject*)self)->cert, l_key, ((PyDapCertMetadataTypeObject*)l_obj_type_metadata)->type, l_data, sizeof(double));
        return Py_None;
    }
    if (PyLong_Check(l_obj_value)){
        PyErr_SetString(PyExc_TypeError, "Sorry, but this function's behavior hasn't been wrapped yet");
        return NULL;
    }
    if (PyUnicode_Check(l_obj_value)){
        PyErr_SetString(PyExc_TypeError, "Sorry, but this function's behavior hasn't been wrapped yet");
        return NULL;
        if (PyUnicode_GET_LENGTH(l_obj_value) == 1){
//            Py_UCS4 symbol = PyUnicode_ReadChar(l_obj_value, 0);
        }
    }
    PyErr_SetString(PyExc_SyntaxError, "Sorry, but the resulting type cannot be added as scalar to the certificate");
    return NULL;
}

PyObject *dap_cert_add_meta_string_py(PyObject *self, PyObject *args){
    (void) self;
    (void) args;
    /// TODO: Implement it!
    PyErr_SetString(PyExc_TypeError, "Unimplemented function");
    return NULL;
}
PyObject *dap_cert_add_meta_sign_py(PyObject *self, PyObject *args){
    (void) self;
    (void) args;
    /// TODO: Implement it!
    PyErr_SetString(PyExc_TypeError, "Unimplemented function");
    return NULL;
}
PyObject *dap_cert_add_meta_custom_py(PyObject *self, PyObject *args){
    (void) self;
    (void) args;
    /// TODO: Implement it!
    PyErr_SetString(PyExc_TypeError, "Unimplemented function");
    return NULL;
}
PyObject *dap_cert_add_meta_bool_py(PyObject *self, PyObject *args){
    (void) self;
    (void) args;
    /// TODO: Implement it!
    PyErr_SetString(PyExc_TypeError, "Unimplemented function");
    return NULL;
}
PyObject *dap_cert_add_meta_int_py(PyObject *self, PyObject *args){
    (void) self;
    (void) args;
    /// TODO: Implement it!
    PyErr_SetString(PyExc_TypeError, "Unimplemented function");
    return NULL;
}
PyObject *dap_cert_add_meta_time_py(PyObject *self, PyObject *args){
    (void) self;
    (void) args;
    /// TODO: Implement it!
    PyErr_SetString(PyExc_TypeError, "Unimplemented function");
    return NULL;
}
PyObject *dap_cert_add_meta_period_py(PyObject *self, PyObject *args){
    (void) self;
    (void) args;
    /// TODO: Implement it!
    PyErr_SetString(PyExc_TypeError, "Unimplemented function");
    return NULL;
}

PyObject *dap_cert_get_meta_string_py(PyObject *self, PyObject *args){
    (void) self;
    (void) args;
    /// TODO: Implement it!
    PyErr_SetString(PyExc_TypeError, "Unimplemented function");
    return NULL;
}
PyObject *dap_cert_get_meta_bool_py(PyObject *self, PyObject *args){
    (void) self;
    (void) args;
    /// TODO: Implement it!
    PyErr_SetString(PyExc_TypeError, "Unimplemented function");
    return NULL;
}
PyObject *dap_cert_get_meta_int_py(PyObject *self, PyObject *args){
    (void) self;
    (void) args;
    /// TODO: Implement it!
    PyErr_SetString(PyExc_TypeError, "Unimplemented function");
    return NULL;
}
PyObject *dap_cert_get_meta_time_py(PyObject *self, PyObject *args){
    (void) self;
    (void) args;
    /// TODO: Implement it!
    PyErr_SetString(PyExc_TypeError, "Unimplemented function");
    return NULL;
}
PyObject *dap_cert_get_meta_period_py(PyObject *self, PyObject *args){
    (void) self;
    (void) args;
    /// TODO: Implement it!
    PyErr_SetString(PyExc_TypeError, "Unimplemented function");
    return NULL;
}
PyObject *dap_cert_get_meta_sign_py(PyObject *self, PyObject *args){
    (void) self;
    (void) args;
    /// TODO: Implement it!
    PyErr_SetString(PyExc_TypeError, "Unimplemented function");
    return NULL;
}
PyObject *dap_cert_get_meta_custom_py(PyObject *self, PyObject *args){
    (void) self;
    (void) args;
    /// TODO: Implement it!
    PyErr_SetString(PyExc_TypeError, "Unimplemented function");
    return NULL;
}

///!!! META TYPE
