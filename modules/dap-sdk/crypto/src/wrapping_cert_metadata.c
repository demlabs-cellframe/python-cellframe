#include "wrapping_cert_metadata.h"
#include "wrapping_cert.h"
#include "wrapping_dap_sign.h"
#include "datetime.h"


///!!! META

time_t _wrapping_cert_metadata_get_time_t(PyObject *a_date_time){
    struct tm l_tm_time;
    l_tm_time.tm_year = PyDateTime_GET_YEAR(a_date_time);
    l_tm_time.tm_mon = PyDateTime_GET_MONTH(a_date_time);
    l_tm_time.tm_mday = PyDateTime_GET_DAY(a_date_time);
    l_tm_time.tm_hour = PyDateTime_DATE_GET_HOUR(a_date_time);
    l_tm_time.tm_min = PyDateTime_DATE_GET_MINUTE(a_date_time);
    l_tm_time.tm_sec = PyDateTime_DATE_GET_SECOND(a_date_time);
    time_t l_time = mktime(&l_tm_time);
    return l_time;
}

PyObject *_wrapping_cert_metadata_get_dateTime(time_t *a_time){
    struct tm l_tm = *(localtime(a_time));
    PyObject *l_obj_time = PyDateTime_FromDateAndTime(l_tm.tm_year, l_tm.tm_mon, l_tm.tm_mday, l_tm.tm_hour, l_tm.tm_min, l_tm.tm_sec, 0);
    return l_obj_time;
}

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
        PyErr_SetString(PyExc_BytesWarning, "The third agrument the is not a bytes object");
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
        PyErr_SetString(PyExc_BytesWarning, "The third agrument the is not a bytes object");
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

PyObject *dap_cert_add_meta_all_py(PyObject *self, PyObject *args){
    (void) self;
    PyObject *l_obj_cert = NULL;
    const char *l_key = NULL;
    PyObject *l_obj_data = NULL;
    if (!PyArg_ParseTuple(args, "OsO", &l_obj_cert, &l_key, &l_obj_data)){
        PyErr_SetString(PyExc_SyntaxError, "Wrong arguments list in function call");
        return NULL;
    }
    if (PyUnicode_Check(l_obj_data)){}
    if (PyLong_Check(l_obj_data)){}
    if (PyBytes_Check(l_obj_data)){}
    /// TODO: Implement it!
    PyErr_SetString(PyExc_TypeError, "Unimplemented function");
    return NULL;
}
PyObject *dap_cert_add_meta_string_py(PyObject *self, PyObject *args){
    (void) self;
    PyObject *l_obj_cert = NULL;
    const char *l_key = NULL;
    const char *l_data = NULL;
    if (!PyArg_ParseTuple(args, "Oss", &l_obj_cert, &l_key, &l_data)){
        PyErr_SetString(PyExc_SyntaxError, "Wrong arguments list in function call");
        return NULL;
    }
    dap_cert_add_meta_string(((PyCryptoCertObject*)l_obj_cert)->cert, l_key, l_data);
    return Py_None;
}
PyObject *dap_cert_add_meta_sign_py(PyObject *self, PyObject *args){
    (void)self;
    PyObject *l_obj_cert = NULL;
    const char *l_key = NULL;
    PyObject *l_obj_sign = NULL;
    if (!PyArg_ParseTuple(args, "OsO", &l_obj_cert, &l_key, &l_obj_sign)){
        PyErr_SetString(PyExc_SyntaxError, "Wrong arguments list in function call");
        return NULL;
    }
    dap_cert_add_meta_sign(((PyCryptoCertObject*)l_obj_cert)->cert, l_key, ((PyDapSignObject*)l_obj_sign)->sign);
    return Py_None;
}
PyObject *dap_cert_add_meta_custom_py(PyObject *self, PyObject *args){
    (void) self;
    PyObject *l_obj_cert = NULL;
    const char *l_key = NULL;
    PyObject *l_obj_bytes = NULL;
    if (!PyArg_ParseTuple(args, "OsO", &l_obj_cert, &l_key, &l_obj_bytes)){
        PyErr_SetString(PyExc_SyntaxError, "Wrong arguments list in function call");
        return NULL;
    }
    if (!PyBytes_Check(l_obj_bytes)){
        PyErr_SetString(PyExc_BytesWarning, "The third agrument the is not a bytes object");
        return NULL;
    }
    size_t l_value_size = (size_t)PyBytes_Size(l_obj_bytes);
    void *l_value = PyBytes_AsString(l_obj_bytes);
    dap_cert_add_meta_custom(((PyCryptoCertObject*)l_obj_cert)->cert, l_key, l_value, l_value_size);
    return Py_None;
}
PyObject *dap_cert_add_meta_bool_py(PyObject *self, PyObject *args){
    (void) self;
    PyObject *l_obj_cert = NULL;
    const char *l_key = NULL;
    PyObject *l_obj_bool = NULL;
    if (!PyArg_ParseTuple(args, "OsO", &l_obj_cert, &l_key, &l_obj_bool)){
        PyErr_SetString(PyExc_SyntaxError, "Wrong arguments list in function call");
        return NULL;
    }
    return NULL;
}
PyObject *dap_cert_add_meta_int_py(PyObject *self, PyObject *args){
    (void) self;
    PyObject *l_obj_cert = NULL;
    const char *l_key = NULL;
    int l_data;
    if (!PyArg_ParseTuple(args, "Osi", &l_obj_cert, &l_key, &l_data)){
        PyErr_SetString(PyExc_SyntaxError, "Wrong arguments list in function call");
        return NULL;
    }
    dap_cert_add_meta_int(((PyCryptoCertObject*)l_obj_cert)->cert, l_key, l_data);
    return Py_None;
}
PyObject *dap_cert_add_meta_time_py(PyObject *self, PyObject *args){
    (void) self;
    PyObject *l_obj_cert = NULL;
    const char *l_key = NULL;
    PyObject *l_obj_data = NULL;
    if (!PyArg_ParseTuple(args, "OsO", &l_obj_cert, &l_key, &l_obj_data)){
        PyErr_SetString(PyExc_SyntaxError, "Wrong arguments list in function call");
        return NULL;
    }
    if (!PyDateTime_Check(l_obj_data)){
        PyErr_SetString(PyExc_AttributeError, "The third agrument the is not a DateTime object");
        return NULL;
    }
    time_t l_time = _wrapping_cert_metadata_get_time_t(l_obj_data);
    dap_cert_add_meta_time(((PyCryptoCertObject*)l_obj_cert)->cert, l_key, l_time);
    return Py_None;
}
PyObject *dap_cert_add_meta_period_py(PyObject *self, PyObject *args){
    (void) self;
    PyObject *l_obj_cert = NULL;
    const char *l_key = NULL;
    PyObject *l_obj_data = NULL;
    if (!PyArg_ParseTuple(args, "OsO", &l_obj_cert, &l_key, &l_obj_data)){
        PyErr_SetString(PyExc_SyntaxError, "Wrong arguments list in function call");
        return NULL;
    }
    if (!PyDateTime_Check(l_obj_data)){
        PyErr_SetString(PyExc_AttributeError, "The third agrument the is not a DateTime object");
        return NULL;
    }
    time_t l_time = _wrapping_cert_metadata_get_time_t(l_obj_data);
    dap_cert_add_meta_period(((PyCryptoCertObject*)l_obj_cert)->cert, l_key, l_time);
    return Py_None;
}

PyObject *dap_cert_get_meta_string_py(PyObject *self, PyObject *args){
    (void) self;
    PyObject *l_obj_cert = NULL;
    const char *l_key = NULL;
    if (!PyArg_ParseTuple(args, "Os", &l_obj_cert, &l_key)){
        PyErr_SetString(PyExc_SyntaxError, "Wrong arguments list in function call");
        return NULL;
    }
    const char *l_str = dap_cert_get_meta_string(((PyCryptoCertObject*)l_obj_cert)->cert, l_key);
    if (l_str){
        return Py_BuildValue("s", l_str);
    } else {
        return Py_None;
    }
}
PyObject *dap_cert_get_meta_bool_py(PyObject *self, PyObject *args){
    (void) self;
    PyObject *l_obj_cert = NULL;
    const char *l_key = NULL;
    if (!PyArg_ParseTuple(args, "Os", &l_obj_cert, &l_key)){
        PyErr_SetString(PyExc_SyntaxError, "Wrong arguments list in function call");
        return NULL;
    }
    bool l_boolean = dap_cert_get_meta_bool(((PyCryptoCertObject*)l_obj_cert)->cert, l_key);
    return l_boolean ? Py_BuildValue("O", Py_True) : Py_BuildValue("O", Py_False);
}
PyObject *dap_cert_get_meta_int_py(PyObject *self, PyObject *args){
    (void) self;
    PyObject *l_obj_cert = NULL;
    const char *l_key = NULL;
    if (!PyArg_ParseTuple(args, "Os", &l_obj_cert, &l_key)){
        PyErr_SetString(PyExc_SyntaxError, "Wrong arguments list in function call");
        return NULL;
    }
    int l_int = dap_cert_get_meta_int(((PyCryptoCertObject*)l_obj_cert)->cert, l_key);
    return Py_BuildValue("i", l_int);
}
PyObject *dap_cert_get_meta_time_py(PyObject *self, PyObject *args){
    (void) self;
    PyObject *l_obj_cert = NULL;
    const char *l_key = NULL;
    if (!PyArg_ParseTuple(args, "Os", &l_obj_cert, &l_key)){
        PyErr_SetString(PyExc_SyntaxError, "Wrong arguments list in function call");
        return NULL;
    }
    time_t l_time = dap_cert_get_meta_time(((PyCryptoCertObject*)l_obj_cert)->cert, l_key);
    PyObject *l_obj_time = _wrapping_cert_metadata_get_dateTime(&l_time);
    return Py_BuildValue("O", l_obj_time);
}
PyObject *dap_cert_get_meta_period_py(PyObject *self, PyObject *args){
    (void) self;
    PyObject *l_obj_cert = NULL;
    const char *l_key = NULL;
    if (!PyArg_ParseTuple(args, "Os", &l_obj_cert, &l_key)){
        PyErr_SetString(PyExc_SyntaxError, "Wrong arguments list in function call");
        return NULL;
    }
    time_t l_time = dap_cert_get_meta_time(((PyCryptoCertObject*)l_obj_cert)->cert, l_key);
    PyObject *l_obj_time = _wrapping_cert_metadata_get_dateTime(&l_time);
    return Py_BuildValue("O", l_obj_time);
}
PyObject *dap_cert_get_meta_sign_py(PyObject *self, PyObject *args){
    (void) self;
    PyObject *l_obj_cert = NULL;
    const char *l_key = NULL;
    if (!PyArg_ParseTuple(args, "Os", &l_obj_cert, &l_key)){
        PyErr_SetString(PyExc_SyntaxError, "Wrong arguments list in function call");
        return NULL;
    }
    dap_sign_t *l_sign = dap_cert_get_meta_sign(((PyCryptoCertObject*)l_obj_cert)->cert, l_key);
    if (l_sign){
        PyDapSignObject* l_obj_sign = (PyDapSignObject*)_PyObject_New(&DapSignTypeObject_DapSignTypeObjectType);
        l_obj_sign->sign = l_sign;
        return Py_BuildValue("O", l_obj_sign);
    }else{
        return Py_None;
    }
}
PyObject *dap_cert_get_meta_custom_py(PyObject *self, PyObject *args){
    (void) self;
    PyObject *l_obj_cert = NULL;
    const char *l_key = NULL;
    if (!PyArg_ParseTuple(args, "Os", &l_obj_cert, &l_key)){
        PyErr_SetString(PyExc_SyntaxError, "Wrong arguments list in function call");
        return NULL;
    }
    size_t l_size_meta;
    void *l_out_meta = dap_cert_get_meta_custom(((PyCryptoCertObject*)l_obj_cert)->cert, l_key, &l_size_meta);
    PyObject *l_obj_data = PyBytes_FromStringAndSize(l_out_meta, l_size_meta);
    return Py_BuildValue("O", l_obj_data);
}

///!!! META TYPE

PyObject *GET_DAP_CERT_META_STRING(PyObject *self, PyObject *args){
    (void)self;
    (void)args;
    PyDapCertMetadataTypeObject *l_obj_metadata_type = (PyDapCertMetadataTypeObject*)_PyObject_New(&g_crypto_cert_type_metadata_type_py);
    l_obj_metadata_type->type = DAP_CERT_META_STRING;
    return Py_BuildValue("O", l_obj_metadata_type);
}
PyObject *GET_DAP_CERT_META_BOOL(PyObject *self, PyObject *args){
    (void)self;
    (void)args;
    PyDapCertMetadataTypeObject *l_obj_metadata_type = (PyDapCertMetadataTypeObject*)_PyObject_New(&g_crypto_cert_type_metadata_type_py);
    l_obj_metadata_type->type = DAP_CERT_META_BOOL;
    return Py_BuildValue("O", l_obj_metadata_type);
}
PyObject *GET_DAP_CERT_META_INT(PyObject *self, PyObject *args){
    (void)self;
    (void)args;
    PyDapCertMetadataTypeObject *l_obj_metadata_type = (PyDapCertMetadataTypeObject*)_PyObject_New(&g_crypto_cert_type_metadata_type_py);
    l_obj_metadata_type->type = DAP_CERT_META_INT;
    return Py_BuildValue("O", l_obj_metadata_type);
}
PyObject *GET_DAP_CERT_META_DATETIME(PyObject *self, PyObject *args){
    (void)self;
    (void)args;
    PyDapCertMetadataTypeObject *l_obj_metadata_type = (PyDapCertMetadataTypeObject*)_PyObject_New(&g_crypto_cert_type_metadata_type_py);
    l_obj_metadata_type->type = DAP_CERT_META_DATETIME;
    return Py_BuildValue("O", l_obj_metadata_type);
}
PyObject *GET_DAP_CERT_META_DATETIME_PERIOD(PyObject *self, PyObject *args){
    (void)self;
    (void)args;
    PyDapCertMetadataTypeObject *l_obj_metadata_type = (PyDapCertMetadataTypeObject*)_PyObject_New(&g_crypto_cert_type_metadata_type_py);
    l_obj_metadata_type->type = DAP_CERT_META_DATETIME_PERIOD;
    return Py_BuildValue("O", l_obj_metadata_type);
}
PyObject *GET_DAP_CERT_META_SIGN(PyObject *self, PyObject *args){
    (void)self;
    (void)args;
    PyDapCertMetadataTypeObject *l_obj_metadata_type = (PyDapCertMetadataTypeObject*)_PyObject_New(&g_crypto_cert_type_metadata_type_py);
    l_obj_metadata_type->type = DAP_CERT_META_SIGN;
    return Py_BuildValue("O", l_obj_metadata_type);
}
PyObject *GET_DAP_CERT_META_CUSTOM(PyObject *self, PyObject *args){
    (void)self;
    (void)args;
    PyDapCertMetadataTypeObject *l_obj_metadata_type = (PyDapCertMetadataTypeObject*)_PyObject_New(&g_crypto_cert_type_metadata_type_py);
    l_obj_metadata_type->type = DAP_CERT_META_CUSTOM;
    return Py_BuildValue("O", l_obj_metadata_type);
}
PyObject *getName(PyObject *self, PyObject *args){
    (void)args;
    char *s = NULL;
    switch (((PyDapCertMetadataTypeObject*)self)->type){
    case DAP_CERT_META_STRING:
        s = "DAP_CERT_META_STRING";
        break;
    case DAP_CERT_META_BOOL:
        s = "DAP_CERT_META_BOOL";
        break;
    case DAP_CERT_META_INT:
        s = "DAP_CERT_META_INT";
        break;
    case DAP_CERT_META_DATETIME:
        s = "DAP_CERT_META_DATETIME";
        break;
    case DAP_CERT_META_DATETIME_PERIOD:
        s = "DAP_CERT_META_DATETIME_PERIOD";
        break;
    case DAP_CERT_META_SIGN:
        s = "DAP_CERT_META_SIGN";
        break;
    case DAP_CERT_META_CUSTOM:
        s = "DAP_CERT_META_CUSTOM";
        break;
    }
    return Py_BuildValue("s", s);
}
PyObject *getValue(PyObject *self, PyObject *args){
    (void)args;
    int l_code = ((PyDapCertMetadataTypeObject*)self)->type;
    return Py_BuildValue("i", l_code);
}

