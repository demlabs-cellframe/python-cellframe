#define PY_SSIZE_T_CLEAN
#include <python3.7/Python.h>
#include "dap_config.h"
#include "dap_common.h"

#ifdef __cplusplus
extern "C" {
#endif

#define LOG_TAG "libdap-python"

static PyObject *dap_init(PyObject *self, PyObject *args);

static PyObject *dap_deinit(PyObject *self);

static PyObject *dap_set_log_level(PyObject *self, PyObject *args);

static PyMethodDef DapMethods[] = {
        {"init", dap_init, METH_VARARGS, "Initialization of the DAP (Deus Applicaions Prototypes) library"},
        {"deinit", dap_deinit, METH_NOARGS, "Deinitialization of the DAP (Deus Applicaions Prototypes) library"},
        {"setLogLevel", dap_set_log_level, METH_VARARGS, "Setting the logging level"},
        {NULL, NULL, 0, NULL}
};

static struct PyModuleDef dapmodule = {
        PyModuleDef_HEAD_INIT,
        "libdap_python_module",   /* name of module */
        NULL, /* module documentation, may be NULL */
        -1,       /* size of per-interpreter state of the module,
                 or -1 if the module keeps state in global variables. */
        DapMethods
};

PyMODINIT_FUNC PyInit_libdap_python_module(void);

#ifdef  __cplusplus
}
#endif