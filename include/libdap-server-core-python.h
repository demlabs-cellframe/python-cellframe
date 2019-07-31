#define PY_SSIZE_T_CLEAN
#include "Python.h"
#include "dap_server.h"
#include "dap_client_remote.h"
#include "dap_events.h"
#include "dap_common.h"

#ifdef __cplusplus
extern "C" {
#endif

dap_server_t *t_server;



#define LOG_TAG "libdap-server-core-python"

static PyObject *dap_server_core_init(PyObject *self, PyObject *args);
static PyObject *dap_server_core_deinit(void);

static PyObject *dap_server_core_loop(PyObject *self, PyObject *args);
static PyObject *dap_server_core_listen(PyObject *self, PyObject *args);

static PyMethodDef DapServerCorePythonMethods[] = {
        {"init", dap_server_core_init, METH_VARARGS, "Initialization of the DAP (Deus Applicaions Prototypes) server core library"},
        {"deinit", dap_server_core_deinit, METH_NOARGS, "Deinitialization of the DAP (Deus Applicaions Prototypes) server core library"},
        {"loop", dap_server_core_loop, METH_VARARGS, ""},
        {"listen", dap_server_core_listen, METH_VARARGS, ""},
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
