#include "wrapping_base58.h"

PyObject *dap_encode_base58_py(PyObject *self, PyObject *args){
    const char* data;
    if (!PyArg_ParseTuple(args, "s", &data)){
        return NULL;
    }
    size_t encode_result_size = DAP_ENC_BASE58_ENCODE_SIZE(strlen(data));
    char res[encode_result_size];
    dap_enc_base58_encode(data, strlen(data), res);
    return Py_BuildValue("s", res);
}

PyObject *dap_decode_base58_py(PyObject *self, PyObject *args){
    const char* data;
    if (!PyArg_ParseTuple(args, "s", &data)){
        return NULL;
    }
    size_t decode_result_size = DAP_ENC_BASE58_DECODE_SIZE(strlen(data));
    char res[decode_result_size];
    dap_enc_base58_decode(data, res);
    return Py_BuildValue("s", res);
}