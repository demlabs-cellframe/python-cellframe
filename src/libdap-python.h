#define PY_SSIZE_T_CLEAN
#include <python3.7/Python.h>
#include "dap_config.h"
#include "dap_common.h"

#ifdef __cplusplus
extern "C" {
#endif

#define LOG_TAG "libdap-python"

static PyObject *dap_init(PyObject *self, PyObject *args);

static PyObject *dap_deinit();

static PyObject *dap_set_log_level(PyObject *self, PyObject *args);

static PyObject* dap_log_it(PyObject* self, PyObject* args);

static dap_log_level_t convert_const_char_to_dap_log_level(const char* string);

static PyObject* py_m_dap_config_get_item(PyObject *self, PyObject *args);

static PyObject* py_m_dap_config_get_item_default(PyObject *self, PyObject *args);

static PyMethodDef DapMethods[] = {
        {"init", dap_init, METH_VARARGS, "Initialization of the DAP (Deus Applicaions Prototypes) library"},
        {"deinit", dap_deinit, METH_NOARGS, "Deinitialization of the DAP (Deus Applicaions Prototypes) library"},
        {"setLogLevel", dap_set_log_level, METH_VARARGS, "Setting the logging level"},
        {"logIt", dap_log_it, METH_VARARGS, "The wrapper of the log_it function for the libdap library"},
        {"configGetItem", py_m_dap_config_get_item, METH_VARARGS, ""},
        {"configGetItemDefault", py_m_dap_config_get_item_default, METH_VARARGS, ""},
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
