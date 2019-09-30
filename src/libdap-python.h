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
        {NULL, NULL, 0, NULL}
};

static PyTypeObject DapObject_DapObjectType = {
    PyVarObject_HEAD_INIT(NULL, 0)
    "CellFrame.Dap",             /* tp_name */
    sizeof(PyDapObject),             /* tp_basicsize */
    0,                         /* tp_itemsize */
    0,			       /* tp_dealloc */
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
    DapMethods,                /* tp_methods */
    0,                         /* tp_members */
    0,                         /* tp_getset */
    0,                         /* tp_base */
    0,                         /* tp_dict */
    0,                         /* tp_descr_get */
    0,                         /* tp_descr_set */
    0,                         /* tp_dictoffset */
    0,                         /* tp_init */
    0,                         /* tp_alloc */
    PyType_GenericNew,         /* tp_new */
};

#ifdef  __cplusplus
}
#endif
