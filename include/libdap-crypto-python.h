#define PY_SSIZE_T_CLEAN
#include <Python.h>
#include "dap_enc.h"
#include "dap_enc_key.h"
#include "dap_common.h"
#include "wrapping_base58.h"
#include "wrapping_base64.h"
#include "wrapping_dap_enc_key.h"
#include "wrapping_dap_enc_iaes.h"
#include "wrapping_dap_enc_oaes.h"

#ifdef __cplusplus
extern "C" {
#endif

#undef LOG_TAG
#define LOG_TAG "libdap-python-crypto"

typedef struct PyCrypto{
    PyObject_HEAD
} PyCryptoObject;

int dap_crypto_init(void);
void dap_crypto_deinit(void);

static PyMethodDef DapCryptoMethods[] = {
        {"encodeBase58", (PyCFunction)dap_encode_base58_py, METH_VARARGS | METH_STATIC, "Encrypts information using the base58 algorithm from the DAP crypto library"},
        {"decodeBase58", (PyCFunction)dap_decode_base58_py, METH_VARARGS | METH_STATIC, "Dencrypts information using the base58 algorithm from the DAP crypto library"},
        {"encodeBase64", (PyCFunction)dap_encode_base64_py, METH_VARARGS | METH_STATIC, "Encrypts information using the base64 algorithm from the DAP crypto library"},
        {"decodeBase64", (PyCFunction)dap_decode_base64_py, METH_VARARGS | METH_STATIC, "Dencrypts information using the base64 algorithm from the DAP crypto library"},
        {"newKey", (PyCFunction)dap_enc_key_new_py, METH_VARARGS | METH_STATIC, "The function creates a new key, and returns it with PyObject."},
        {"delKey", (PyCFunction)dap_enc_key_delete_py, METH_VARARGS | METH_STATIC, ""},
        {"generateNewKey", (PyCFunction)dap_enc_key_new_generate_py, METH_VARARGS | METH_STATIC, ""},
        {"getEncSizeKey", (PyCFunction)dap_enc_key_get_enc_size_py, METH_VARARGS | METH_STATIC, ""},
        {"getDecSizeKey", (PyCFunction)dap_enc_key_get_dec_size_py, METH_VARARGS | METH_STATIC, ""},
        /*IAES256*/
        {"newKeyIAES", (PyCFunction)dap_enc_iaes_key_new_py, METH_VARARGS | METH_STATIC, ""},
        {"deleteKeyIAES", (PyCFunction)dap_enc_iaes_key_delete_py, METH_VARARGS | METH_STATIC, ""},
        {"generateKeyIAES", (PyCFunction)dap_enc_iaes_key_generate_py, METH_VARARGS | METH_STATIC, ""},
        {"encodeSizeIAES256", (PyCFunction)dap_enc_iaes256_calc_encode_size_py, METH_VARARGS | METH_STATIC, ""},
        {"decodeSizeIAES256", (PyCFunction)dap_enc_iaes256_calc_decode_size_py, METH_VARARGS | METH_STATIC, ""},
        {"encryptIAES256CBCFast", (PyCFunction)dap_enc_iaes256_cbc_encrypt_fast_py, METH_VARARGS | METH_STATIC, ""},
        {"decryptIAES256CBCFast", (PyCFunction)dap_enc_iaes256_cbc_decrypt_fast_py, METH_VARARGS | METH_STATIC, ""},
        /*OAES*/
        {"newKeyOAES", (PyCFunction)dap_enc_oaes_key_new_py, METH_VARARGS | METH_STATIC, ""},
        {"deleteKeyOAES", (PyCFunction)dap_enc_oaes_key_delete_py, METH_VARARGS | METH_STATIC, ""},
        {"generateKeyOAES", (PyCFunction)dap_enc_oaes_key_generate_py, METH_VARARGS | METH_STATIC, ""},
        {"encodeSizeOAES", (PyCFunction)dap_enc_oaes_calc_encode_size_py, METH_VARARGS | METH_STATIC, ""},
        {"decodeSizeOAES", (PyCFunction)dap_enc_oaes_calc_decode_size_py, METH_VARARGS | METH_STATIC, ""},
        {"encryptOAESFast", (PyCFunction)dap_enc_oaes_encrypt_fast_py, METH_VARARGS | METH_VARARGS | METH_STATIC, ""},
        {"decryptOAESFast", (PyCFunction)dap_enc_oaes_decrypt_fast_py, METH_VARARGS | METH_STATIC, ""},
        {NULL, NULL, 0, NULL}
};

PyTypeObject dapCrypto_dapCryptoType = {
    PyVarObject_HEAD_INIT(NULL, 0)
    "libCellFrame.crypto",             /* tp_name */
    sizeof(PyCryptoObject),             /* tp_basicsize */
    0,                         /* tp_itemsize */
    0,//(destructor)Noddy_dealloc, /* tp_dealloc */
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
    "Dap crypto objects",           /* tp_doc */
    0,		               /* tp_traverse */
    0,		               /* tp_clear */
    0,		               /* tp_richcompare */
    0,		               /* tp_weaklistoffset */
    0,		               /* tp_iter */
    0,		               /* tp_iternext */
    DapCryptoMethods,//Noddy_methods,             /* tp_methods */
    0,//Noddy_members,             /* tp_members */
    0,//Noddy_getseters,           /* tp_getset */
    0,                         /* tp_base */
    0,                         /* tp_dict */
    0,                         /* tp_descr_get */
    0,                         /* tp_descr_set */
    0,                         /* tp_dictoffset */
    0,//(initproc)PyDapEventsObject_init,//(initproc)Noddy_init,      /* tp_init */
    0,                         /* tp_alloc */
    PyType_GenericNew,//Noddy_new,                 /* tp_new */
};


//static struct PyModuleDef dapcryptomodule = {
//        PyModuleDef_HEAD_INIT,
//        "libdap_crypto_python_module",   /* name of module */
//        NULL, /* module documentation, may be NULL */
//        -1,       /* size of per-interpreter state of the module,
//                 or -1 if the module keeps state in global variables. */
//        DapCryptoMethods
//};

//PyMODINIT_FUNC PyInit_libdap_crypto_python_module(void);

#ifdef  __cplusplus
}
#endif
