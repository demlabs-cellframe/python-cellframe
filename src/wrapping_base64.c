#include "wrapping_base64.h"

 PyObject *dap_encode_base64_py(PyObject *self, PyObject *args){
    const char* data;
    if (!PyArg_ParseTuple(args, "s", &data)){
        return NULL;
    }
    char res[DAP_ENC_BASE64_ENCODE_SIZE(strlen(data))];
    dap_enc_base64_encode(data, strlen(data),res, DAP_ENC_DATA_TYPE_B64);
    return Py_BuildValue("s", res);
}

 PyObject *dap_decode_base64_py(PyObject *self, PyObject *args){
    const char* data;
    int size_source;
    if (!PyArg_ParseTuple(args, "s|i", &data, &size_source)){
        return NULL;
    }
    char res[size_source];
    dap_enc_base64_decode(data, strlen(data), res, DAP_ENC_DATA_TYPE_B64);
    return Py_BuildValue("s", res);
}