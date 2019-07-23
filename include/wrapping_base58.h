#include "Python.h"
#include "dap_enc_base58.h"
#include "dap_common.h"

#ifdef __cplusplus
extern "C" {
#endif

PyObject *dap_encode_base58_py(PyObject *self, PyObject *args);

PyObject *dap_decode_base58_py(PyObject *self, PyObject *args);

#ifdef __cplusplus
}
#endif
