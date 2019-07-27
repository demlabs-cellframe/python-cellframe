#include "Python.h"
#include "dap_enc_oaes.h"
#include "key_list.h"

#ifdef __cplusplus
extern "C" {
#endif

key_list_t *keys_oaes;

PyObject *dap_enc_oaes_key_new_py(PyObject *self, PyObject *args);//struct dap_enc_key * a_key);
PyObject *dap_enc_oaes_key_delete_py(PyObject *self, PyObject *args);//struct dap_enc_key *a_key);
PyObject *dap_enc_oaes_key_generate_py(PyObject *self, PyObject *args);//struct dap_enc_key * a_key, const void *kex_buf,
        //size_t kex_size, const void * seed, size_t seed_size, size_t key_size);

PyObject *dap_enc_oaes_calc_decode_size_py(PyObject *self, PyObject *args);//const size_t size_in);
PyObject *dap_enc_oaes_calc_encode_size_py(PyObject *self, PyObject *args);//const size_t size_in);

PyObject *dap_enc_oaes_decrypt_py(PyObject *self, PyObject *args);//struct dap_enc_key * a_key, const void * a_in, size_t a_in_size, void ** a_out);
PyObject *dap_enc_oaes_encrypt_py(PyObject *self, PyObject *args);//struct dap_enc_key * a_key, const void * a_in, size_t a_in_size, void ** a_out);

// Writes result ( out ) in already allocated buffer
PyObject *dap_enc_oaes_decrypt_fast_py(PyObject *self, PyObject *args);//struct dap_enc_key * a_key, const void * a_in,
        //size_t a_in_size, void * buf_out, size_t buf_out_size);
// Writes result ( out ) in already allocated buffer
PyObject *dap_enc_oaes_encrypt_fast_py(PyObject *self, PyObject *args);//struct dap_enc_key * a_key, const void * a_in,
        //size_t a_in_size, void * buf_out, size_t buf_out_size);


#ifdef __cplusplus
}
#endif
