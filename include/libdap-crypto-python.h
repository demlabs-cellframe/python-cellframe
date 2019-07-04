#define PY_SSIZE_T_CLEAN
#include <python3.7/Python.h>
#include "dap_enc.h"
#include "dap_enc_key.h"
#include "dap_common.h"
#include "wrapping_base58.h"
#include "wrapping_base64.h"



#ifdef __cplusplus
extern "C" {
#endif

#define LOG_TAG "libdap-python-crypto"

static PyObject *dap_crypto_init(PyObject *self, PyObject *args);

static PyObject *dap_crypto_deinit();

static PyMethodDef DapCryptoMethods[] = {
        {"init", dap_crypto_init, METH_VARARGS, "Initialization of the DAP (Deus Applicaions Prototypes) crypto library"},
        {"deinit", dap_crypto_deinit, METH_NOARGS, "Deinitialization of the DAP (Deus Applicaions Prototypes) crypto library"},
        {"encodeBase58", dap_encode_base58_py, METH_VARARGS, "Encrypts information using the base58 algorithm from the DAP crypto library"},
        {"decodeBase58", dap_decode_base58_py, METH_VARARGS, "Dencrypts information using the base58 algorithm from the DAP crypto library"},
        {"encodeBase64", dap_encode_base64_py, METH_VARARGS, "Encrypts information using the base64 algorithm from the DAP crypto library"},
        {"decodeBase64", dap_decode_base64_py, METH_VARARGS, "Dencrypts information using the base64 algorithm from the DAP crypto library"},
        {NULL, NULL, 0, NULL}
};

static struct PyModuleDef dapcryptomodule = {
        PyModuleDef_HEAD_INIT,
        "libdap_crypto_python_module",   /* name of module */
        NULL, /* module documentation, may be NULL */
        -1,       /* size of per-interpreter state of the module,
                 or -1 if the module keeps state in global variables. */
        DapCryptoMethods
};

PyMODINIT_FUNC PyInit_libdap_crypto_python_module(void);

#ifdef  __cplusplus
}
#endif