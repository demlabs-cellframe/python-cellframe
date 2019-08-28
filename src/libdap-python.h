#pragma once
#define PY_SSIZE_T_CLEAN
#include <Python.h>
#include "dap_config.h"
#include "dap_common.h"

#ifdef __cplusplus
extern "C" {
#endif

#define LOG_TAG "libdap-python"

typedef struct PyDap{
    PyObject_HEAD
}PyDapObject;

//static PyObject *dap_init(PyObject *self, PyObject *args);

//static PyObject *dap_deinit(PyObject *self, PyObject *args);

PyObject *dap_set_log_level(PyObject *self, PyObject *args);

PyObject* dap_log_it(PyObject* self, PyObject* args);

PyObject* dap_log_it_debug(PyObject* self, PyObject* args);
PyObject* dap_log_it_info(PyObject* self, PyObject* args);
PyObject* dap_log_it_notice(PyObject* self, PyObject* args);
PyObject* dap_log_it_message(PyObject* self, PyObject* args);
PyObject* dap_log_it_dap(PyObject* self, PyObject* args);
PyObject* dap_log_it_warning(PyObject* self, PyObject* args);
PyObject* dap_log_it_att(PyObject* self, PyObject* args);
PyObject* dap_log_it_error(PyObject* self, PyObject* args);
PyObject* dap_log_it_critical(PyObject* self, PyObject* args);

PyObject* py_m_dap_config_get_item(PyObject *self, PyObject *args);

PyObject* py_m_dap_config_get_item_default(PyObject *self, PyObject *args);

static PyMethodDef DapMethods[] = {
        //{"init", dap_init, METH_VARARGS, "Initialization of the DAP (Deus Applicaions Prototypes) library"},
        //{"deinit", dap_deinit, METH_NOARGS, "Deinitialization of the DAP (Deus Applicaions Prototypes) library"},
        //{"setLogLevel", (PyCFunction)dap_set_log_level, METH_STATIC, "Setting the logging level"},
        //{"logIt", (PyCFunction)dap_log_it, METH_STATIC, "The wrapper of the log_it function for the libdap library"},
        //{"configGetItem", (PyCFunction)py_m_dap_config_get_item, METH_STATIC, ""},
        //{"configGetItemDefault", (PyCFunction)py_m_dap_config_get_item_default, METH_STATIC, ""},
        {NULL, NULL, 0, NULL}
};

static PyTypeObject DapObject_DapObjectType = {
    PyVarObject_HEAD_INIT(NULL, 0)
    "libCellFrame.Dap",             /* tp_name */
    sizeof(PyDapObject),             /* tp_basicsize */
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
    "Dap objects",           /* tp_doc */
    0,		               /* tp_traverse */
    0,		               /* tp_clear */
    0,		               /* tp_richcompare */
    0,		               /* tp_weaklistoffset */
    0,		               /* tp_iter */
    0,		               /* tp_iternext */
    DapMethods,//Noddy_methods,             /* tp_methods */
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




//static struct PyModuleDef dapmodule = {
//        PyModuleDef_HEAD_INIT,
//        "libdap_python_module",   /* name of module */
//        NULL, /* module documentation, may be NULL */
//        -1,       /* size of per-interpreter state of the module,
//                 or -1 if the module keeps state in global variables. */
//        DapMethods
//};

//PyMODINIT_FUNC PyInit_libdap_python_module(void);

#ifdef  __cplusplus
}
#endif
