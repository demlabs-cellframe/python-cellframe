#define PY_SSIZE_T_CLEAN
#include <Python.h>
#include "dap_enc.h"
#include "dap_enc_key.h"
#include "dap_common.h"
#include "wrapping_cert.h"
#include "wrapping_base58.h"
#include "wrapping_base64.h"
#include "wrapping_dap_enc_key.h"
#include "wrapping_dap_enc_iaes.h"
#include "wrapping_dap_enc_oaes.h"

#ifdef __cplusplus
extern "C" {
#endif


typedef struct PyCrypto{
    PyObject_HEAD
}PyCryptoObject;

int dap_crypto_init(void);

void dap_crypto_deinit(void);

static PyMethodDef CryptoMethods[] = {
        {"encodeBase58", dap_encode_base58_py, METH_VARARGS | METH_STATIC, "Encrypts information using the base58 algorithm from the DAP crypto library"},
        {"decodeBase58", dap_decode_base58_py, METH_VARARGS | METH_STATIC, "Dencrypts information using the base58 algorithm from the DAP crypto library"},
        {"encodeBase64", dap_encode_base64_py, METH_VARARGS | METH_STATIC, "Encrypts information using the base64 algorithm from the DAP crypto library"},
        {"decodeBase64", dap_decode_base64_py, METH_VARARGS | METH_STATIC, "Dencrypts information using the base64 algorithm from the DAP crypto library"},
        {"newKey", dap_enc_key_new_py, METH_VARARGS | METH_STATIC, "The function creates a new key, and returns it with PyObject."},
        {"generateNewKey", dap_enc_key_new_generate_py, METH_VARARGS | METH_STATIC, ""},
        {"getEncSizeKey", dap_enc_key_get_enc_size_py, METH_VARARGS | METH_STATIC, ""},
        {"getDecSizeKey", dap_enc_key_get_dec_size_py, METH_VARARGS | METH_STATIC, ""},
        /*IAES256*/
        {"newKeyIAES", dap_enc_iaes_key_new_py, METH_VARARGS | METH_STATIC, ""},
        {"deleteKeyIAES", dap_enc_iaes_key_delete_py, METH_VARARGS | METH_STATIC, ""},
        {"generateKeyIAES", dap_enc_iaes_key_generate_py, METH_VARARGS | METH_STATIC, ""},
        {"encodeSizeIAES256", dap_enc_iaes256_calc_encode_size_py, METH_VARARGS | METH_STATIC, ""},
        {"decodeSizeIAES256", dap_enc_iaes256_calc_decode_size_py, METH_VARARGS | METH_STATIC, ""},
        {"encryptIAES256CBCFast", dap_enc_iaes256_cbc_encrypt_fast_py, METH_VARARGS | METH_STATIC, ""},
        {"decryptIAES256CBCFast", dap_enc_iaes256_cbc_decrypt_fast_py, METH_VARARGS | METH_STATIC, ""},
        /*OAES*/
        {"newKeyOAES", dap_enc_oaes_key_new_py, METH_VARARGS | METH_STATIC, ""},
        {"deleteKeyOAES", dap_enc_oaes_key_delete_py, METH_VARARGS | METH_STATIC, ""},
        {"generateKeyOAES", dap_enc_oaes_key_generate_py, METH_VARARGS | METH_STATIC, ""},
        {"encodeSizeOAES", dap_enc_oaes_calc_encode_size_py, METH_VARARGS | METH_STATIC, ""},
        {"decodeSizeOAES", dap_enc_oaes_calc_decode_size_py, METH_VARARGS | METH_STATIC, ""},
        {"encryptOAESFast", dap_enc_oaes_encrypt_fast_py, METH_VARARGS | METH_STATIC, ""},
        {"decryptOAESFast", dap_enc_oaes_decrypt_fast_py, METH_VARARGS | METH_STATIC, ""},
        {NULL, NULL, 0, NULL}
};

static PyTypeObject dapCrypto_dapCryptoType = {
    PyVarObject_HEAD_INIT(NULL, 0)
    "CellFrame.Crypto",             /* tp_name */
    sizeof(PyCryptoObject),         /* tp_basicsize */
    0,                         /* tp_itemsize */
    0,                         /* tp_dealloc */
    0,                         /* tp_print */
    0,                         /* tp_getattr */
    0,                         /* tp_setattr */
    0,                         /* tp_reserved */
    0,                         /* tp_repr */
    0,                         /* tp_as_number */
    0,                         /* tp_as_sequence */
    0,                         /* tp_as_mapping */
    0,                         /* tp_hash  */
    0,                         /* tp_call */
    0,                         /* tp_str */
    0,                         /* tp_getattro */
    0,                         /* tp_setattro */
    0,                         /* tp_as_buffer */
    Py_TPFLAGS_DEFAULT |
        Py_TPFLAGS_BASETYPE,   /* tp_flags */
    "Crypto objects",           /* tp_doc */
    0,		               /* tp_traverse */
    0,		               /* tp_clear */
    0,		               /* tp_richcompare */
    0,		               /* tp_weaklistoffset */
    0,		               /* tp_iter */
    0,		               /* tp_iternext */
    CryptoMethods,             /* tp_methods */
    0,                         /* tp_members */
    0,                         /* tp_getset */
    0,                         /* tp_base */
    0,                         /* tp_dict */
    0,                         /* tp_descr_get */
    0,                         /* tp_descr_set */
    0,                         /* tp_dictoffset */
    0,                         /* tp_init */
    0,                         /* tp_alloc */
    PyType_GenericNew,         /* tp_new */
};

#ifdef  __cplusplus
}
#endif
