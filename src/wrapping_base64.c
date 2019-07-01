#include "wrapping_base64.h"

static PyObject *dap_encode_base64_py(PyObject *self, PyObject *args){
    const char* data;
    if (!PyArg_ParseTuple(self, "s", &data)){
        return NULL;
    }
    char* res;
    dap_enc_base64_encode(data, strlen(data),res, DAP_ENC_DATA_TYPE_B64);
    return Py_BuildValue("s", res);
}

static PyObject *dap_decode_base64_py(PyObject *self, PyObject *args){
    const char* data;
    if (!PyArg_ParseTuple(self, "s", &data)){
        return NULL;
    }
    char* res;
    dap_enc_base64_decode(data, strlen(data), res, DAP_ENC_DATA_TYPE_B64);
    return Py_BuildValue("s", res);
}