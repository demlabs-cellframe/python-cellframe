#include "wrapping_base58.h"

static PyObject *dap_encode_base58_py(PyObject *self, PyObject *args){
    const char* data;
    if (!PyArg_ParseTuple(self, "s", &data)){
        return NULL;
    }
    char* res;
    dap_enc_base58_encode(data, strlen(data), res);
    return Py_BuildValue("s", res);
}

static PyObject *dap_decode_base58_py(PyObject *self, PyObject *args){
    const char* data;
    if (!PyArg_ParseTuple(self, "s", &data)){
        return NULL;
    }
    char* res;
    dap_enc_base58_decode(data, res);
    return Py_BuildValue("s", res);
}