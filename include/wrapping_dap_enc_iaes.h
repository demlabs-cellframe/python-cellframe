#ifndef WRAPPING_DAP_ENC_IAES_
#define WRAPPING_DAP_ENC_IAES_

#include "Python.h"
#include "dap_enc_iaes.h"
#include "dap_common.h"
#include "libdap_crypto_key_python.h"

#ifdef __cplusplus
extern "C" {
#endif

#undef LOG_TAG
#define LOG_TAG "wrapping-dap-enc-iaes"

PyObject* dap_enc_iaes_key_new_py(PyObject *self, PyObject *args);

PyObject* dap_enc_iaes_key_delete_py(PyObject *self, PyObject *args);
PyObject* dap_enc_iaes_key_generate_py(PyObject *self, PyObject *args);

PyObject* dap_enc_iaes256_calc_decode_size_py(PyObject *self, PyObject *args);
PyObject* dap_enc_iaes256_calc_encode_size_py(PyObject *self, PyObject *args);

PyObject* dap_enc_iaes256_cbc_decrypt_py(PyObject *self, PyObject *args);  //TODO
PyObject* dap_enc_iaes256_cbc_encrypt_py(PyObject *self, PyObject *args);  //TODO

// Writes result ( out ) in already allocated buffer
PyObject* dap_enc_iaes256_cbc_decrypt_fast_py(PyObject *self, PyObject *args);
// if "a_in size mod IAES_BLOCK_SIZE = 0" encryption will be faster
PyObject* dap_enc_iaes256_cbc_encrypt_fast_py(PyObject *self, PyObject *args);

#ifdef __cplusplus
}
#endif

#endif //WRAPPING_DAP_ENC_IAES_
