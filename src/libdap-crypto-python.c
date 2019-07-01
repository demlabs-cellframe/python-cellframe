#include "libdap-crypto-python.h"

static PyObject* dap_crypto_init(PyObject *self, PyObject *args){
    if(dap_enc_init()!=0){
        log_it(L_CRITICAL,"Can't init encryption module");
        return PyLong_FromLong(-1);
    }
    if(dap_enc_key_init()!=0){
        log_it(L_CRITICAL,"Can't init encryption key module");
        return PyLong_FromLong(-2);
    }
    return PyLong_FromLong(0);
}

static PyObject* dap_crypto_deinit(){
    dap_enc_key_deinit();
    dap_enc_deinit();
    return PyLong_FromLong(0);
}

PyMODINIT_FUNC PyInit_libdap_crypto_python_module(void){
    return PyModule_Create(&dapcryptomodule);
}

int main(int argc, char **argv) {
    wchar_t *program = Py_DecodeLocale(argv[0], NULL);
    if (program == NULL) {
        fprintf(stderr, "Fatal error: cannot decode argv[0]\n");
        exit(1);
    }

    /* Add a built-in module, before Py_Initialize */
    PyImport_AppendInittab("libdap_crypto_python_module", PyInit_libdap_crypto_python_module);

    /* Pass argv[0] to the Python interpreter */
    Py_SetProgramName(program);

    /* Initialize the Python interpreter.  Required. */
    Py_Initialize();

    /* Optionally import the module; alternatively,
       import can be deferred until the embedded script
       imports it. */
    PyImport_ImportModule("libdap_crypto_python_module");

    PyMem_RawFree(program);
    return 0;
}
