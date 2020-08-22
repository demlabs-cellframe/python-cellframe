#pragma once
#define PY_SSIZE_T_CLEAN
#include "Python.h"
#include "dap_server_python.h"
#include "dap_server.h"
#include "dap_events.h"
#include "dap_common.h"
#include "dap_events_python.h"
#include "dap_events_socket_python.h"

#ifdef __cplusplus
extern "C" {
#endif


typedef struct SeverCore{
    PyObject_HEAD
}PyServerCoreObject;

int dap_server_core_init(uint32_t l_thread_cnt, size_t conn_t);
void dap_server_core_deinit(void);

PyObject *dap_server_core_loop(PyObject *self, PyObject *args);
PyObject *dap_server_core_listen(PyObject *self, PyObject *args);

static PyMethodDef ServerCorePythonMethods[] = {
        {"listen", dap_server_core_listen, METH_VARARGS | METH_STATIC, ""},
        {NULL, NULL, 0, NULL}
};


static PyTypeObject ServerCore_ServerCoreType = {
    PyVarObject_HEAD_INIT(NULL, 0)
    "ServerCore",             /* tp_name */
    sizeof(PyServerCoreObject),         /* tp_basicsize */
    0,                                  /* tp_itemsize */
    0,                                  /* tp_dealloc */
    0,                                  /* tp_print */
    0,                                  /* tp_getattr */
    0,                                  /* tp_setattr */
    0,                                  /* tp_reserved */
    0,                                  /* tp_repr */
    0,                                  /* tp_as_number */
    0,                                  /* tp_as_sequence */
    0,                                  /* tp_as_mapping */
    0,                                  /* tp_hash  */
    0,                                  /* tp_call */
    0,                                  /* tp_str */
    0,                                  /* tp_getattro */
    0,                                  /* tp_setattro */
    0,                                  /* tp_as_buffer */
    Py_TPFLAGS_DEFAULT |
        Py_TPFLAGS_BASETYPE,            /* tp_flags */
    "ServerCore objects",               /* tp_doc */
    0,		                        /* tp_traverse */
    0,         		                /* tp_clear */
    0,		                        /* tp_richcompare */
    0,		                        /* tp_weaklistoffset */
    0,		                        /* tp_iter */
    0,         		                /* tp_iternext */
    ServerCorePythonMethods,            /* tp_methods */
    0,                                  /* tp_members */
    0,                                  /* tp_getset */
    0,                                  /* tp_base */
    0         ,                         /* tp_dict */
    0,                                  /* tp_descr_get */
    0,                                  /* tp_descr_set */
    0,                                  /* tp_dictoffset */
    0,                                  /* tp_init */
    0,                                  /* tp_alloc */
    PyType_GenericNew,                  /* tp_new */
};



#ifdef __cplusplus
}
#endif
