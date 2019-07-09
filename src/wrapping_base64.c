#include "wrapping_base64.h"

 PyObject *dap_encode_base64_py(PyObject *self, PyObject *args){
     PyObject *in_data;
     short int l_dap_enc_data_type;
     if (!PyArg_ParseTuple(args, "S|h", &in_data, &l_dap_enc_data_type)){
         return NULL;
     }
     if (l_dap_enc_data_type < 1 || l_dap_enc_data_type > 2){
         return NULL;
     }
     char* data = PyBytes_AsString(in_data);
     size_t size_t_str = strlen(data);
     char res[DAP_ENC_BASE64_ENCODE_SIZE(size_t_str)];
     dap_enc_base64_encode(data, size_t_str,res, l_dap_enc_data_type);
     PyBytesObject *out_obj = (PyBytesObject *)PyBytes_FromFormat("%s", res);
     return Py_BuildValue("S", out_obj);
}

 PyObject *dap_decode_base64_py(PyObject *self, PyObject *args){
     PyObject *data;
     short int l_dap_enc_data_type=1;
     if (!PyArg_ParseTuple(args, "S|h", &data, &l_dap_enc_data_type)) {
         return NULL;
     }
     if (l_dap_enc_data_type < 1 || l_dap_enc_data_type > 2){
         return NULL;
     }
     char *in_data = PyBytes_AsString(data);
     char *res = NULL;
     res = DAP_NEW_SIZE(void, DAP_ENC_BASE64_ENCODE_SIZE(strlen(in_data)));
     dap_enc_base64_decode(in_data, strlen(in_data), res, l_dap_enc_data_type);
     PyBytesObject *pyBytesObject = (PyBytesObject *)PyBytes_FromFormat("%s", res);
     return Py_BuildValue("S", pyBytesObject);
}