#include "wrapping_dap_client_error.h"

PyObject* GET_ERROR_NO_ERROR(PyObject *self, PyObject *args){
    (void)self;
    (void)args;
    PyDapClientErrorObject *l_obj = (PyDapClientErrorObject*)_PyObject_New(&dapClientErrorObject_dapClientErrorType);
    l_obj->error = ERROR_NO_ERROR;
    return Py_BuildValue("O", l_obj);
}
PyObject* GET_ERROR_OUT_OF_MEMORY(PyObject *self, PyObject *args){
    (void)self;
    (void)args;
    PyDapClientErrorObject *l_obj = (PyDapClientErrorObject*)_PyObject_New(&dapClientErrorObject_dapClientErrorType);
    l_obj->error = ERROR_OUT_OF_MEMORY;
    return Py_BuildValue("O", l_obj);
}
PyObject* GET_ERROR_ENC_NO_KEY(PyObject *self, PyObject *args){
    (void)self;
    (void)args;
    PyDapClientErrorObject *l_obj = (PyDapClientErrorObject*)_PyObject_New(&dapClientErrorObject_dapClientErrorType);
    l_obj->error = ERROR_ENC_NO_KEY;
    return Py_BuildValue("O", l_obj);
}
PyObject* GET_ERROR_ENC_WRONG_KEY(PyObject *self, PyObject *args){
    (void)self;
    (void)args;
    PyDapClientErrorObject *l_obj = (PyDapClientErrorObject*)_PyObject_New(&dapClientErrorObject_dapClientErrorType);
    l_obj->error = ERROR_ENC_WRONG_KEY;
    return Py_BuildValue("O", l_obj);
}
PyObject* GET_ERROR_ENC_SESSION_CLOSED(PyObject *self, PyObject *args){
    (void)self;
    (void)args;
    PyDapClientErrorObject *l_obj = (PyDapClientErrorObject*)_PyObject_New(&dapClientErrorObject_dapClientErrorType);
    l_obj->error = ERROR_ENC_SESSION_CLOSED;
    return Py_BuildValue("O", l_obj);
}
PyObject* GET_ERROR_STREAM_CTL_ERROR(PyObject *self, PyObject *args){
    (void)self;
    (void)args;
    PyDapClientErrorObject *l_obj = (PyDapClientErrorObject*)_PyObject_New(&dapClientErrorObject_dapClientErrorType);
    l_obj->error = ERROR_STREAM_CTL_ERROR;
    return Py_BuildValue("O", l_obj);
}
PyObject* GET_ERROR_STREAM_CTL_ERROR_AUTH(PyObject *self, PyObject *args){
    (void)self;
    (void)args;
    PyDapClientErrorObject *l_obj = (PyDapClientErrorObject*)_PyObject_New(&dapClientErrorObject_dapClientErrorType);
    l_obj->error = ERROR_STREAM_CTL_ERROR_AUTH;
    return Py_BuildValue("O", l_obj);
}
PyObject* GET_ERROR_STREAM_CTL_ERROR_RESPONSE_FORMAT(PyObject *self, PyObject *args){
    (void)self;
    (void)args;
    PyDapClientErrorObject *l_obj = (PyDapClientErrorObject*)_PyObject_New(&dapClientErrorObject_dapClientErrorType);
    l_obj->error = ERROR_STREAM_CTL_ERROR_RESPONSE_FORMAT;
    return Py_BuildValue("O", l_obj);
}
PyObject* GET_ERROR_STREAM_CONNECT(PyObject *self, PyObject *args){
    (void)self;
    (void)args;
    PyDapClientErrorObject *l_obj = (PyDapClientErrorObject*)_PyObject_New(&dapClientErrorObject_dapClientErrorType);
    l_obj->error = ERROR_STREAM_CONNECT;
    return Py_BuildValue("O", l_obj);
}
PyObject* GET_ERROR_STREAM_RESPONSE_WRONG(PyObject *self, PyObject *args){
    (void)self;
    (void)args;
    PyDapClientErrorObject *l_obj = (PyDapClientErrorObject*)_PyObject_New(&dapClientErrorObject_dapClientErrorType);
    l_obj->error = ERROR_STREAM_RESPONSE_WRONG;
    return Py_BuildValue("O", l_obj);
}
PyObject* GET_ERROR_STREAM_RESPONSE_TIMEOUT(PyObject *self, PyObject *args){
    (void)self;
    (void)args;
    PyDapClientErrorObject *l_obj = (PyDapClientErrorObject*)_PyObject_New(&dapClientErrorObject_dapClientErrorType);
    l_obj->error = ERROR_STREAM_RESPONSE_TIMEOUT;
    return Py_BuildValue("O", l_obj);
}
PyObject* GET_ERROR_STREAM_FREEZED(PyObject *self, PyObject *args){
    (void)self;
    (void)args;
    PyDapClientErrorObject *l_obj = (PyDapClientErrorObject*)_PyObject_New(&dapClientErrorObject_dapClientErrorType);
    l_obj->error = ERROR_STREAM_FREEZED;
    return Py_BuildValue("O", l_obj);
}
PyObject* GET_ERROR_NETWORK_CONNECTION_REFUSE(PyObject *self, PyObject *args){
    (void)self;
    (void)args;
    PyDapClientErrorObject *l_obj = (PyDapClientErrorObject*)_PyObject_New(&dapClientErrorObject_dapClientErrorType);
    l_obj->error = ERROR_NETWORK_CONNECTION_REFUSE;
    return Py_BuildValue("O", l_obj);
}
PyObject* GET_ERROR_NETWORK_CONNECTION_TIMEOUT(PyObject *self, PyObject *args){
    (void)self;
    (void)args;
    PyDapClientErrorObject *l_obj = (PyDapClientErrorObject*)_PyObject_New(&dapClientErrorObject_dapClientErrorType);
    l_obj->error = ERROR_NETWORK_CONNECTION_TIMEOUT;
    return Py_BuildValue("O", l_obj);
}
PyObject* getName(PyObject *self, PyObject *args){
    (void)args;
    const char *l_error_str = dap_client_error_str(((PyDapClientErrorObject*)self)->error);
    return Py_BuildValue("s", l_error_str);
}
PyObject* getValue(PyObject *self, PyObject *args){
    (void)args;
    int l_value = ((PyDapClientErrorObject*)self)->error;
    return Py_BuildValue("i", l_value);
}
