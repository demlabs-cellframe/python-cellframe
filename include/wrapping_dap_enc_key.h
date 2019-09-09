#ifndef WRAPPING_DAP_ENC_KEY_
#define WRAPPING_DAP_ENC_KEY_

#include "dap_enc_key.h"
#include "Python.h"
#include "dap_common.h"
#include "libdap_crypto_key_python.h"

#ifdef __cplusplus
extern "C" {
#endif

#undef LOG_TAG
#define LOG_TAG "wrapping-dap-enc-key"

PyObject* dap_enc_key_get_enc_size_py(PyObject *self, PyObject *args);//dap_enc_key_t * a_key, const size_t buf_in_size); -> size_t
PyObject* dap_enc_key_get_dec_size_py(PyObject *self, PyObject *args);//dap_enc_key_t * a_key, const size_t buf_in_size); -> size_t


// allocate memory for key struct
PyObject* dap_enc_key_new_py(PyObject *self, PyObject *args);//dap_enc_key_type_t a_key_type);     ->dap_enc_key_t*


// default gen key
PyObject *dap_enc_key_new_generate_py(PyObject *self, PyObject *args);//dap_enc_key_type_t key_type, const void *kex_buf,  ->dap_enc_key_t*
                                        //size_t kex_size, const void* seed,
                                        //size_t seed_size, size_t key_size);

// update struct dap_enc_key_t after insert foreign keys
PyObject* dap_enc_key_update_py(PyObject *self, PyObject *args);//dap_enc_key_t *a_key);            ->void

// for asymmetric gen public key
PyObject *dap_enc_gen_pub_key_from_priv_py(PyObject *self, PyObject *args);//struct dap_enc_key *a_key, void **priv_key, size_t *alice_msg_len);  ->dap_enc_key_t *


PyObject *dap_enc_gen_key_public_size_py(PyObject *self, PyObject *args);//dap_enc_key_t *a_key); ->size_t
PyObject *dap_enc_gen_key_public_py(PyObject *self, PyObject *args);//dap_enc_key_t *a_key, void * a_output); ->int

PyObject *dap_enc_key_signature_delete_py(PyObject *self, PyObject *args);//dap_enc_key_type_t a_key_type, uint8_t *a_sig_buf);     ->void
PyObject *dap_enc_key_delete_py(PyObject *self, PyObject *args);//dap_enc_key_t * a_key);       ->void


#ifdef __cplusplus
}
#endif

#endif //WRAPPING_DAP_ENC_KEY_
