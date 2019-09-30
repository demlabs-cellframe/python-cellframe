#ifndef WRAPPING_DAP_ENC_OAES_
#define WRAPPING_DAP_ENC_OAES_

#include "Python.h"
#include "dap_enc_oaes.h"
#include "dap_common.h"
#include "libdap_crypto_key_python.h"

#ifdef __cplusplus
extern "C" {
#endif

PyObject *dap_enc_oaes_key_new_py(PyObject *self, PyObject *args);
PyObject *dap_enc_oaes_key_delete_py(PyObject *self, PyObject *args);
PyObject *dap_enc_oaes_key_generate_py(PyObject *self, PyObject *args);

PyObject *dap_enc_oaes_calc_decode_size_py(PyObject *self, PyObject *args);
PyObject *dap_enc_oaes_calc_encode_size_py(PyObject *self, PyObject *args);

PyObject *dap_enc_oaes_decrypt_py(PyObject *self, PyObject *args);
PyObject *dap_enc_oaes_encrypt_py(PyObject *self, PyObject *args);

// Writes result ( out ) in already allocated buffer
PyObject *dap_enc_oaes_decrypt_fast_py(PyObject *self, PyObject *args);
// Writes result ( out ) in already allocated buffer
PyObject *dap_enc_oaes_encrypt_fast_py(PyObject *self, PyObject *args);



#ifdef __cplusplus
}
#endif

#endif //WRAPPING_DAP_ENC_OAES_
