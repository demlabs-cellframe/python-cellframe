#define PY_SSIZE_T_CLEAN
#include <python3.7/Python.h>

#ifdef __cplusplus
extern "C" {
#endif

#define LOG_TAG "libdap-python-crypto"

static PyObject *dap_crypto_init(PyObject *self, PyObject *args);

static PyObject *dap_crypto_deinit();

static PyMethodDef DapCryptoMethods[] = {
        {"init", dap_crypto_init, METH_VARARGS, "Initialization of the DAP (Deus Applicaions Prototypes) crypto library"},
        {"deinit", dap_crypto_init, METH_NOARGS, "Deinitialization of the DAP (Deus Applicaions Prototypes) crypto library"},
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