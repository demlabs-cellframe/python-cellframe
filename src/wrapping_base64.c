#include "wrapping_base64.h"

 PyObject *dap_encode_base64_py(PyObject *self, PyObject *args){
     PyObject *in_data;
     if (!PyArg_ParseTuple(args, "S", & in_data)){
         return NULL;
     }
     char* data = PyBytes_AsString(in_data);
     size_t size_t_str = strlen(data);
     char res[DAP_ENC_BASE64_ENCODE_SIZE(size_t_str)];
     dap_enc_base64_encode(data, size_t_str,res, DAP_ENC_DATA_TYPE_B64);
     PyBytesObject *out_obj = (PyBytesObject *)PyBytes_FromFormat("%s",res);
     return Py_BuildValue("Si", out_obj, size_t_str);
}

 PyObject *dap_decode_base64_py(PyObject *self, PyObject *args){
     PyObject *data;
     int size_t_str;
     if (!PyArg_ParseTuple(args, "S|i", &data, &size_t_str)) {
         return NULL;
     }
     char *in_data = PyBytes_AsString(data);
     char res[size_t_str];
     dap_enc_base64_decode(in_data, strlen(in_data), res, DAP_ENC_DATA_TYPE_B64);
     PyObject *pyBytesObject = PyBytes_FromFormat("%s", res);
     return Py_BuildValue("O", pyBytesObject);
}