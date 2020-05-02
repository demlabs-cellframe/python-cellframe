#pragma once

#define PY_SSIZE_T_CLEAN
#include <Python.h>
#include "dap_common.h"
#include "dap_app_cli.h"
#include "dap_strfuncs.h"


typedef struct PyAppCli{
    PyObject_HEAD
}PyAppCliObject;


PyObject* dap_app_cli_main_py(PyObject *self, PyObject *args);

static PyMethodDef AppCliMethods[] = {
        {"main", dap_app_cli_main_py, METH_VARARGS | METH_STATIC, "Main CLI function"},
        {NULL, NULL, 0, NULL}
};

static PyTypeObject DapAppCli_dapAppCliType = {
    PyVarObject_HEAD_INIT(NULL, 0)
    "AppCli",             /* tp_name */
    sizeof(PyAppCliObject),         /* tp_basicsize */
    0,                         /* tp_itemsize */
    0,                         /* tp_dealloc */
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
    "AppCli object",           /* tp_doc */
    0,		               /* tp_traverse */
    0,		               /* tp_clear */
    0,		               /* tp_richcompare */
    0,		               /* tp_weaklistoffset */
    0,		               /* tp_iter */
    0,		               /* tp_iternext */
    AppCliMethods,             /* tp_methods */
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
    0,                         /* tp_free */
    0,                         /* tp_is_gc*/
    0,                          /* tp_bases*/
    0,                           /* tp_mro */
    0,                           /* tp_cache */
    0,                           /* tp_subclasses */
    0,                           /* tp_weaklist */
    0,                           /* tp_del */
    0,                          /* tp_version_tag*/
    0,                         /* tp_finalize*/
};

