#include "Python.h"
#include "dap_enc_base58.h"
#include "dap_common.h"

PyObject *dap_encode_base58_py(PyObject *self, PyObject *args);

PyObject *dap_decode_base58_py(PyObject *self, PyObject *args);

void* getData(PyObject* obj);