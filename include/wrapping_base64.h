#include "Python.h"
#include "dap_enc_base64.h"
#include "py_bytes_object_worker.h"

#ifdef __cplusplus
extern "C" {
#endif

PyObject *dap_encode_base64_py(PyObject *self, PyObject *args);

PyObject *dap_decode_base64_py(PyObject *self, PyObject *args);

#ifdef __cplusplus
}
#endif