#define PY_SSIZE_T_CLEAN
#include "Python.h"
#include "dap_server.h"

#ifdef __cplusplus
extern "C" {
#endif

static PyObject *dap_server_core_init(PyObject *self, PyObject *args);
static PyObject *dap_server_core_deinit();

static PyMethodDef DapServerCorePythonMethods[] = {
        {"init", dap_server_core_init, METH_VARARGS, "Initialization of the DAP (Deus Applicaions Prototypes) server core library"},
        {"deinit", dap_server_core_deinit, METH_NOARGS, "Deinitialization of the DAP (Deus Applicaions Prototypes) server core library"},
        {NULL, NULL, 0, NULL}
};

static struct PyModuleDef dapservercorepythonmodule = {
        PyModuleDef_HEAD_INIT,
        "libdap_python_module",   /* name of module */
        NULL, /* module documentation, may be NULL */
        -1,       /* size of per-interpreter state of the module,
                 or -1 if the module keeps state in global variables. */
        DapServerCorePythonMethods
};

PyMODINIT_FUNC PyInit_libdap_server_core_python_module(void);






#ifdef __cplusplus
}
#endif
