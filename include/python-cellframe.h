#define PY_SSIZE_T_CLEAN
#include "Python.h"

#ifdef __cplusplus
extern "C" {
#endif

PyObject *python_cellframe_init(PyObject *self, PyObject *args);

static PyMethodDef CellFramePythonMethods[] = {
        {"init", python_cellframe_init, METH_VARARGS, "Initialization of the python-cellframe interface DAP (Deus Applicaions Prototypes)"},
        //{"deinit", dap_server_core_deinit, METH_NOARGS, "Deinitialization of the DAP (Deus Applicaions Prototypes) server core library"},
        //{"loop", dap_server_core_loop, METH_VARARGS, ""},
        //{"listen", dap_server_core_listen, METH_VARARGS, ""},
        {NULL, NULL, 0, NULL}
};

static struct PyModuleDef CellFramePythonModule = {
        PyModuleDef_HEAD_INIT,
        "libCellFrame",   /* name of module */
        NULL, /* module documentation, may be NULL */
        -1,       /* size of per-interpreter state of the module,
                 or -1 if the module keeps state in global variables. */
        CellFramePythonMethods
};

PyMODINIT_FUNC PyInit_libCellFrame(void);


#ifdef __cplusplus
}
#endif


